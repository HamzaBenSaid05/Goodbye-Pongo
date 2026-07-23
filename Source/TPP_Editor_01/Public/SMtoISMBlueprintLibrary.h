#pragma once

#include "CoreMinimal.h"
#include "SMtoISMBlueprintLibrary.generated.h"

USTRUCT(BlueprintType)
struct FSMtoISMSettingsBP
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behaviour")
	bool bReadFromSource = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behaviour")
	bool bDeleteSourceActors = true;

	// Shadow
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow")
	bool bCastShadow = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow")
	bool bCastDynamicShadow = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow")
	bool bCastStaticShadow = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow")
	bool bCastContactShadow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shadow")
	bool bSelfShadowOnly = false;

	// Rendering
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rendering")
	bool bReceivesDecals = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rendering")
	bool bRenderCustomDepth = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rendering")
	int32 CustomDepthStencil = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rendering")
	float BoundsScale = 1.0f;

	// LOD
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOD")
	int32 ForcedLOD = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOD")
	int32 MinLOD = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LOD")
	float MaxDrawDistance = 0.f;

	// Lightmap / UV
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lightmap")
	bool bOverrideLightmapRes = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lightmap")
	int32 OverrideLightmapRes = 64;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lightmap")
	int32 LightmapUVChannel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lightmap")
	float UVTileU = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lightmap")
	float UVTileV = 1.0f;
};

USTRUCT(BlueprintType)
struct FSMtoISMResultBP
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Result")
	int32 ISMActorsCreated = 0;

	UPROPERTY(BlueprintReadOnly, Category="Result")
	int32 SourcesProcessed = 0;

	UPROPERTY(BlueprintReadOnly, Category="Result")
	TArray<FString> Log;
};

UCLASS()
class TPP_EDITOR_01_API USMtoISMBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="SM to ISM",
		meta=(DisplayName="Convert Static Mesh To ISM"))
	static void ConvertStaticMeshToISM(
		const FSMtoISMSettingsBP Settings,
		FSMtoISMResultBP& Result);
};
