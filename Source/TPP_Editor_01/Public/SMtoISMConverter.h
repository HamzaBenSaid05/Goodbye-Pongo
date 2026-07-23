#pragma once

#include "CoreMinimal.h"

struct FSMtoISMSettings
{
	bool  bReadFromSource       = true;   
	bool  bDeleteSourceActors   = true;   

	// --- Shadow 
	bool  bCastShadow           = true;
	bool  bCastDynamicShadow    = true;
	bool  bCastStaticShadow     = true;
	bool  bCastContactShadow    = false;
	bool  bSelfShadowOnly       = false;

	// --- Rendering 
	bool  bReceivesDecals       = true;
	bool  bRenderCustomDepth    = false;
	int32 CustomDepthStencil    = 0;
	float BoundsScale           = 1.0f;

	// --- LOD 
	int32 ForcedLOD             = 0;
	int32 MinLOD                = 0;
	float MaxDrawDistance       = 0.f;

	// --- Lightmap / UV 
	bool  bOverrideLightmapRes  = false;
	int32 OverrideLightmapRes   = 64;
	int32 LightmapUVChannel     = 1;
	float UVTileU               = 1.0f;
	float UVTileV               = 1.0f;
};

struct FSMtoISMResult
{
	int32          ISMActorsCreated  = 0;
	int32          SourcesProcessed  = 0;
	TArray<FString> Log;           
};

struct FHISMGroupKey
{
    UStaticMesh* Mesh = nullptr;
    TArray<UMaterialInterface*> Materials;
    TArray<float> CustomData;
	ULevel* Level = nullptr; 
    bool operator==(const FHISMGroupKey& Other) const
    {
    	if (Mesh != Other.Mesh)       return false;
    	if (Materials != Other.Materials) return false;
    	if (Level != Other.Level)     return false; 

    	if (CustomData.Num() != Other.CustomData.Num()) return false;
    	for (int32 i = 0; i < CustomData.Num(); ++i)
    		if (!FMath::IsNearlyEqual(CustomData[i], Other.CustomData[i]))
    			return false;

    	return true;
    }
};

FORCEINLINE uint32 GetTypeHash(const FHISMGroupKey& K);

class TPP_EDITOR_01_API  FSMtoISMConverter
{
public:
	static FSMtoISMResult Convert(const FSMtoISMSettings& Settings = FSMtoISMSettings());
	static TArray<UMaterialInterface*> NormalizeMaterials(const TArray<UMaterialInterface*>& In);
};
