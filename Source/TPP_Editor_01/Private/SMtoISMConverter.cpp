#include "SMtoISMConverter.h"

#include "Editor.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "ScopedTransaction.h"

struct FCPDLayout
{
	static constexpr int32 U_Tiling = 0;
	static constexpr int32 V_Tiling = 1;
	static constexpr int32 Emissive = 2;
};

struct FSMSnapshot
{
	FTransform                  WorldTransform;
	UStaticMesh*                Mesh           = nullptr;
	TArray<UMaterialInterface*> Materials;

	bool  bCastShadow           = true;
	bool  bCastDynamicShadow    = true;
	bool  bCastStaticShadow     = true;
	bool  bCastContactShadow    = false;
	bool  bSelfShadowOnly       = false;
	bool  bReceivesDecals       = true;
	bool  bRenderCustomDepth    = false;
	int32 CustomDepthStencil    = 0;
	float BoundsScale           = 1.f;
	int32 ForcedLOD             = 0;
	int32 MinLOD                = 0;
	float MaxDrawDistance       = 0.f;
	int32 OverrideLightmapRes   = 64;
	bool  bOverrideLightmapRes  = false;
	EComponentMobility::Type Mobility = EComponentMobility::Static;
	TArray<float> CustomDataFloats;
};

// ---------------------------------------------------------------------------
static FSMSnapshot SnapshotActor(UStaticMeshComponent* C)
{
	FSMSnapshot S;

	S.WorldTransform = C->GetComponentTransform();
	S.Mesh = C->GetStaticMesh();

	const int32 N = C->GetNumMaterials();

	S.Materials.Reserve(N);

	for (int32 i = 0; i < N; ++i)
	{
		S.Materials.Add(C->GetMaterial(i));
	}

	S.bCastShadow          = C->CastShadow;
	S.bCastDynamicShadow   = C->bCastDynamicShadow;
	S.bCastStaticShadow    = C->bCastStaticShadow;
	S.bCastContactShadow   = C->bCastContactShadow;
	S.bSelfShadowOnly      = C->bSelfShadowOnly;

	S.bReceivesDecals      = C->bReceivesDecals;
	S.bRenderCustomDepth   = C->bRenderCustomDepth;
	S.CustomDepthStencil   = C->CustomDepthStencilValue;
	S.BoundsScale          = C->BoundsScale;

	S.ForcedLOD            = C->ForcedLodModel;
	S.MinLOD               = C->MinLOD;
	S.MaxDrawDistance      = C->LDMaxDrawDistance;

	S.OverrideLightmapRes  = C->OverriddenLightMapRes;
	S.bOverrideLightmapRes = C->bOverrideLightMapRes;

	S.Mobility             = C->Mobility;

	const FCustomPrimitiveData& CPD = C->GetCustomPrimitiveData();

	auto SafeGet  = [&](int32 Index)
	{
		return CPD.Data.IsValidIndex(Index) ? CPD.Data[Index] : 0.0f;
	};
	
	S.CustomDataFloats.SetNum(3);

	S.CustomDataFloats[FCPDLayout::U_Tiling] = SafeGet(0);
	S.CustomDataFloats[FCPDLayout::V_Tiling] = SafeGet(1);
	S.CustomDataFloats[FCPDLayout::Emissive] = SafeGet(2);
	return S;
}

// ---------------------------------------------------------------------------
static void ApplyToHISM(
	UHierarchicalInstancedStaticMeshComponent* H,
	const FSMSnapshot& Src,
	const FSMtoISMSettings& Cfg)
{
	H->SetStaticMesh(Src.Mesh);
	

	for (int32 i = 0; i < Src.Materials.Num(); ++i)
		H->SetMaterial(i, Src.Materials[i]);
	H->MarkRenderStateDirty();

	// Shadow
	H->CastShadow           = Cfg.bReadFromSource ? Src.bCastShadow        : Cfg.bCastShadow;
	H->bCastDynamicShadow   = Cfg.bReadFromSource ? Src.bCastDynamicShadow : Cfg.bCastDynamicShadow;
	H->bCastStaticShadow    = Cfg.bReadFromSource ? Src.bCastStaticShadow  : Cfg.bCastStaticShadow;
	H->bCastContactShadow   = Cfg.bReadFromSource ? Src.bCastContactShadow : Cfg.bCastContactShadow;
	H->bSelfShadowOnly      = Cfg.bReadFromSource ? Src.bSelfShadowOnly    : Cfg.bSelfShadowOnly;

	// Rendering
	H->bReceivesDecals      = Cfg.bReadFromSource ? Src.bReceivesDecals    : Cfg.bReceivesDecals;
	H->bRenderCustomDepth   = Cfg.bReadFromSource ? Src.bRenderCustomDepth : Cfg.bRenderCustomDepth;
	H->CustomDepthStencilValue = Cfg.bReadFromSource ? Src.CustomDepthStencil : Cfg.CustomDepthStencil;
	H->BoundsScale          = Cfg.bReadFromSource ? Src.BoundsScale        : Cfg.BoundsScale;

	// LOD
	H->ForcedLodModel       = Cfg.bReadFromSource ? Src.ForcedLOD          : Cfg.ForcedLOD;
	H->MinLOD               = Cfg.bReadFromSource ? Src.MinLOD             : Cfg.MinLOD;
	H->LDMaxDrawDistance    = Cfg.bReadFromSource ? Src.MaxDrawDistance     : Cfg.MaxDrawDistance;

	// Lightmap (sempre da Cfg)
	H->bOverrideLightMapRes  = Cfg.bOverrideLightmapRes;
	H->OverriddenLightMapRes = Cfg.OverrideLightmapRes;
	
	H->SetNumCustomDataFloats(Src.CustomDataFloats.Num());

	H->SetMobility(Src.Mobility);
}


FORCEINLINE uint32 GetTypeHash(const FHISMGroupKey& K)
{
    uint32 Hash = ::GetTypeHash(K.Mesh);
	Hash = HashCombine(Hash, ::GetTypeHash(K.Level)); 
    for (UMaterialInterface* Mat : K.Materials)
    {
        Hash = HashCombine(Hash, ::GetTypeHash(Mat));
    }

    for (float Value : K.CustomData)
    {
        Hash = HashCombine(Hash, ::GetTypeHash(Value));
    }

    return Hash;
}

FSMtoISMResult FSMtoISMConverter::Convert(const FSMtoISMSettings& Cfg)
{
	FSMtoISMResult Result;

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World)
	{
		Result.Log.Add(TEXT("[ERR] No editor world."));
		return Result;
	}

	UEditorActorSubsystem* Sub = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	TArray<AActor*> Selected   = Sub->GetSelectedLevelActors();

	// Snapshot on selected actors, store in Sources and Snapshots
	TArray<UStaticMeshComponent*> Sources;
	TArray<FSMSnapshot> Snapshots;


	for (AActor* Actor : Selected)
	{
		if (!Actor)
		{
			continue;
		}

		TArray<UStaticMeshComponent*> Components;
		Actor->GetComponents<UStaticMeshComponent>(Components);

		for (UStaticMeshComponent* Comp : Components)
		{
			if (!Comp)
			{
				continue;
			}

			if (!Comp->GetStaticMesh())
			{
				continue;
			}

			Sources.Add(Comp);
			Snapshots.Add(SnapshotActor(Comp));
		}
	}

	if (Sources.IsEmpty())
	{
		Result.Log.Add(TEXT("[WARN] No valid StaticMeshComponents in selection."));
		return Result;
	}

	Result.Log.Add(FString::Printf(
		TEXT("[INFO] %d mesh components found, processing..."),
		Sources.Num()));

	const FScopedTransaction Tx(
		NSLOCTEXT("SMtoISM", "Convert", "SM to ISM Conversion"));

	// Group for mesh and create HISM actor
	TMap<FHISMGroupKey, UHierarchicalInstancedStaticMeshComponent*> MeshMap;
	
	for (int32 i = 0; i < Sources.Num(); ++i)
	{
		const FSMSnapshot& Snap = Snapshots[i];

		if (!Snap.Mesh)
		{
			continue;
		}

		FHISMGroupKey Key;
		Key.Mesh = Snap.Mesh;
		Key.Materials = Snap.Materials;
		Key.CustomData = Snap.CustomDataFloats;
		Key.Level     = Sources[i]->GetOwner()->GetLevel(); 
		UHierarchicalInstancedStaticMeshComponent* HISM = nullptr;
		AActor* Container = nullptr;
		
		if (UHierarchicalInstancedStaticMeshComponent** Existing = MeshMap.Find(Key))
		{
			HISM = *Existing;
			Container = HISM ? HISM->GetOwner() : nullptr;

			if (!Container)
			{
				Result.Log.Add(TEXT("[ERR] HISM has no valid owner."));
				continue;
			}
		}
		else
		{
			FActorSpawnParameters P;
			P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			P.OverrideLevel = Key.Level;
			Container = World->SpawnActor<AActor>(AActor::StaticClass(), FTransform::Identity, P);

			if (!Container)
			{
				Result.Log.Add(FString::Printf(
					TEXT("[ERR] SpawnActor failed for mesh: %s"),
					*Snap.Mesh->GetName()));
				continue;
			}

			Container->SetActorLabel(FString::Printf(TEXT("ISM_%s"), *Snap.Mesh->GetName()));

			USceneComponent* Root = NewObject<USceneComponent>(Container, TEXT("Root"));
			Root->RegisterComponent();
			Container->SetRootComponent(Root);

			HISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(
				Container,
				*FString::Printf(TEXT("HISM_%s"), *Snap.Mesh->GetName()));

			HISM->SetupAttachment(Root);
			Container->AddInstanceComponent(HISM);
			ApplyToHISM(HISM, Snap, Cfg);
			HISM->RegisterComponent();
			HISM->RecreateRenderState_Concurrent();

			MeshMap.Add(Key, HISM);

			Result.ISMActorsCreated++;
			Result.Log.Add(FString::Printf(TEXT("[OK] Created ISM_%s"), *Snap.Mesh->GetName()));
		}

		const FTransform LocalTransform =
			Snap.WorldTransform.GetRelativeTransform(Container->GetActorTransform());
		
		
		const int32 InstanceIdx = HISM->AddInstance(LocalTransform, /*bWorldSpace=*/false);

		if (InstanceIdx != INDEX_NONE)
		{
			for (int32 j = 0; j < Snap.CustomDataFloats.Num(); ++j)
			{
				HISM->SetCustomDataValue(InstanceIdx, j, Snap.CustomDataFloats[j], /*bMarkDirty=*/false);
			}
		}
		
		HISM->AddInstance(LocalTransform, false);
		Result.SourcesProcessed++;
	}

	// Destroy source actors only if bDeleteSourceActors is true
	if (Cfg.bDeleteSourceActors)
	{
		TSet<AActor*> ActorsToDelete;

		for (UStaticMeshComponent* Comp : Sources)
		{
			if (Comp && Comp->GetOwner())
			{
				ActorsToDelete.Add(Comp->GetOwner());
			}
		}
		for (AActor* Actor : ActorsToDelete)
		{
			if (IsValid(Actor))
			{
				Actor->Modify();
				World->DestroyActor(Actor);
			}
		}
		Result.Log.Add(FString::Printf(
			TEXT("[OK] Destroyed %d source actors."), Sources.Num()));
	}

	// Select new ISM actors in the editor
	Sub->SelectNothing();
	for (auto& Pair : MeshMap)
		if (Pair.Value && Pair.Value->GetOwner())
			Sub->SetActorSelectionState(Pair.Value->GetOwner(), true);

	World->MarkPackageDirty();

	Result.Log.Add(FString::Printf(
		TEXT("[DONE] %d actors → %d ISM(s)."),
		Result.SourcesProcessed, Result.ISMActorsCreated));

	return Result;
}

TArray<UMaterialInterface*> FSMtoISMConverter::NormalizeMaterials(const TArray<UMaterialInterface*>& In)
{
	TArray<UMaterialInterface*> Out;
	Out.Reserve(In.Num());

	for (UMaterialInterface* Mat : In)
	{
		if (Mat)
		{
			Out.Add(Mat);
		}
	}

	return Out;
}
