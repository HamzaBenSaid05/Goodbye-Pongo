#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/TPP_BaseWidget.h"
#include "Components/Image.h"
#include "TPP_Comic_Base.generated.h"

USTRUCT(BlueprintType)
struct FComicData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cutscene")
	TObjectPtr<UImage> Image = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cutscene")
	TMap<FGameplayTag, TObjectPtr<UWidgetAnimation>> WidgetAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cutscene")
	FGameplayTag ComicTag;
};

UCLASS()
class TPP_01_API UTPP_Comic_Base : public UTPP_BaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable, Category="UI")
	TArray<UWidgetAnimation*> GetAllWidgetAnimationsByImage(const UImage* ImageName) const;
	UFUNCTION(BlueprintCallable, Category="UI")
	TArray<UImage*> GetAllImage() const;
	UFUNCTION(BlueprintCallable, Category="UI")
	void SetUpComicData(UImage* Image, const TArray<UWidgetAnimation*>& Animations);
	TSoftObjectPtr<UTexture2D> GetSoftTextureFromImage(const UImage* Image);
	UFUNCTION(BlueprintCallable, Category="UI") 
	UWidgetAnimation* GetWidgetAnimationByTaG(const TArray<UWidgetAnimation*> WidgetAnimations, const FGameplayTag AnimTag);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cutscene")
	TMap<TSoftObjectPtr<UTexture2D>,FComicData> ComicData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cutscene")
	TArray<FGameplayTag> ComicAnimationTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cutscene")
	FGameplayTag ComicTag;
};

