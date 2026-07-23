#pragma once
#if WITH_EDITOR
#include "CoreMinimal.h"
#include "SplineMetadataDetailsFactory.h"
#include "TPP_Editor_SplineMetadataDetailFactory.generated.h"

// Factory
UCLASS()
class TPP_EDITOR_01_API UTPP_Editor_SplineMetadataDetailFactory : public USplineMetadataDetailsFactoryBase
{
	GENERATED_BODY()

public:
	virtual ~UTPP_Editor_SplineMetadataDetailFactory() {}
	virtual TSharedPtr<ISplineMetadataDetails> Create() override;
	virtual UClass* GetMetadataClass() const override;
};

// UI
class TPP_EDITOR_01_API FTPP_Editor_SplineMetadataDetails : public ISplineMetadataDetails, public TSharedFromThis<FTPP_Editor_SplineMetadataDetails>
{
public:
	virtual ~FTPP_Editor_SplineMetadataDetails() {}

	virtual FName GetName() const override;
	virtual FText GetDisplayName() const override;
	virtual void Update(USplineComponent* InSplineComponent, const TSet<int32>& InSelectedKeys) override;
	bool OnShouldFilterActor(const AActor* Actor);
	virtual void GenerateChildContent(class IDetailGroup& DetailGroup) override;

private:
	class UTPP_CameraSplineMetadata* GetMetadata() const;
	void OnSetValues();

	// Helpers per i widget
	TOptional<float> GetBias() const { return BiasValue; }

	TOptional<float> GetPitch() const { return PitchValue; }
	TOptional<float> GetYaw() const { return YawValue; }
	TOptional<float> GetRoll() const { return RollValue; }

	TOptional<float> GetSpringArmLenght() const { return SpringArmLengthValue; }

	TOptional<float> GetYOffset() const { return YOffsetValue; }
	TOptional<float> GetZOffset() const { return ZOffsetValue; }

	TOptional<float> GetBiasInterpSpeed() const { return BiasInterpSpeedValue; }
	TOptional<float> GetLocationInterpSpeed() const { return LocationInterpSpeedValue; }
	TOptional<float> GetRotationInterpSpeed() const { return RotationInterpSpeedValue; }

	FString GetBiasActorPath() const;

	void OnSetBias(float NewValue, ETextCommit::Type CommitInfo);
	void OnSetPitch(float NewValue, ETextCommit::Type CommitInfo);
	void OnSetYaw(float NewValue, ETextCommit::Type CommitInfo);
	void OnSetRoll(float NewValue, ETextCommit::Type CommitInfo);
	void OnSetSpringArmLenght(float NewValue, ETextCommit::Type CommitInfo);
	void OnSetYOffset(float NewValue, ETextCommit::Type CommitInfo);
	void OnSetZOffset(float NewValue, ETextCommit::Type CommitInfo);
	void OnSetBiasInterpSpeed(float NewValue, ETextCommit::Type CommitInfo);
	void OnSetLocationInterpSpeed(float NewValue, ETextCommit::Type CommitInfo);
	void OnSetRotationInterpSpeed(float NewValue, ETextCommit::Type CommitInfo);

	void OnSetBiasActor(const FAssetData& AssetData);
	void OnBiasActorChanged(const FAssetData& AssetData);

	TWeakObjectPtr<USplineComponent> SplineComp;
	TSet<int32> SelectedKeys;

	TOptional<float> BiasValue;
	TOptional<float> PitchValue;
	TOptional<float> YawValue;
	TOptional<float> RollValue;
	TOptional<float> SpringArmLengthValue;
	TOptional<float> YOffsetValue;
	TOptional<float> ZOffsetValue;
	TOptional<float> BiasInterpSpeedValue;
	TOptional<float> LocationInterpSpeedValue;
	TOptional<float> RotationInterpSpeedValue;

	TOptional<AActor*> BiasActorValue;
};
#endif
