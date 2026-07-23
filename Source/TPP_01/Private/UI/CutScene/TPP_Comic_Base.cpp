#include "UI/CutScene/TPP_Comic_Base.h"

#include "MovieScene.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"

void UTPP_Comic_Base::NativeConstruct()
{
	Super::NativeConstruct();

	//ComicData.Empty();

	TArray<UImage*> Images = GetAllImage();

	for (UImage* Image : Images)
	{
		if (!Image) { continue; }

		TArray<UWidgetAnimation*> Animations =
			GetAllWidgetAnimationsByImage(Image);

		SetUpComicData(Image, Animations);
	}
	// Print all comics data
	for (const auto& ComicEntry : ComicData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Comic Texture: %s"), *ComicEntry.Key.GetAssetName());

		const FComicData& Data = ComicEntry.Value;
		UE_LOG(LogTemp, Warning, TEXT("  - Image: %s"), Data.Image ? *Data.Image->GetName() : TEXT("None"));

		for (const auto& AnimEntry : Data.WidgetAnimations)
		{
			UE_LOG(LogTemp, Warning, TEXT("    - Animation Tag: %s, Animation: %s"),
			       *AnimEntry.Key.ToString(),
			       AnimEntry.Value ? *AnimEntry.Value->GetName() : TEXT("None"));
		}
	}
}

TArray<UWidgetAnimation*> UTPP_Comic_Base::GetAllWidgetAnimationsByImage(const UImage* ImageName) const
{
	TArray<UWidgetAnimation*> Animations;
	UWidgetBlueprintGeneratedClass* WidgetClass =
		Cast<UWidgetBlueprintGeneratedClass>(GetClass());

	if (!WidgetClass) { return Animations; }

	for (UWidgetAnimation* Anim : WidgetClass->Animations)
	{
		if (Anim && Anim->GetName().Contains(ImageName->GetName()))
		{
			Animations.Add(Anim);

			UE_LOG(LogTemp, Warning,
			       TEXT("FOUND ANIM: %s"),
			       *Anim->GetName());
		}
	}
	return Animations;
}

TArray<UImage*> UTPP_Comic_Base::GetAllImage() const
{
	TArray<UWidget*> AllWidgets;
	TArray<UImage*> Images;
	WidgetTree->GetAllWidgets(AllWidgets);

	for (UWidget* Widget : AllWidgets)
	{
		if (UImage* Image = Cast<UImage>(Widget))
		{
			Images.Add(Image);

			UE_LOG(LogTemp, Warning,
			       TEXT("FOUND IMAGE: %s"),
			       *Image->GetName());
		}
	}
	return Images;
}

void UTPP_Comic_Base::SetUpComicData(
	UImage* Image,
	const TArray<UWidgetAnimation*>& Animations)
{
	if (!Image) { return; }

	TSoftObjectPtr<UTexture2D> SoftTexture =
		GetSoftTextureFromImage(Image);

	FComicData NewComicData;
	NewComicData.Image = Image;

	for (const FGameplayTag& Tag : ComicAnimationTags)
	{
		FString TagString = Tag.ToString();

		int32 LastDotIndex;
		FString LastSegment;

		if (TagString.FindLastChar(TEXT('.'), LastDotIndex)) { LastSegment = TagString.Mid(LastDotIndex + 1); }
		else { LastSegment = TagString; }

		for (UWidgetAnimation* Anim : Animations)
		{
			if (!Anim || !Anim->MovieScene) { continue; }

			if (Anim->MovieScene->GetName().Contains(LastSegment)) { NewComicData.WidgetAnimations.Add(Tag, Anim); }
		}
	}

	ComicData.Add(SoftTexture, NewComicData);
}

TSoftObjectPtr<UTexture2D> UTPP_Comic_Base::GetSoftTextureFromImage(const UImage* Image)
{
	const FSlateBrush& Brush = Image->GetBrush();

	UObject* ResourceObject = Brush.GetResourceObject();

	if (UTexture2D* Texture = Cast<UTexture2D>(ResourceObject)) { return TSoftObjectPtr<UTexture2D>(Texture); }

	return nullptr;
}

UWidgetAnimation* UTPP_Comic_Base::GetWidgetAnimationByTaG(const TArray<UWidgetAnimation*> WidgetAnimations, const FGameplayTag AnimTag)
{
	FString TagString = AnimTag.ToString();

	int32 LastDotIndex;
	FString LastSegment;

	if (TagString.FindLastChar(TEXT('.'), LastDotIndex)) { LastSegment = TagString.Mid(LastDotIndex + 1); }
	else { LastSegment = TagString; }

	for (UWidgetAnimation* Anim : WidgetAnimations)
	{
		if (!Anim || !Anim->MovieScene) { continue; }

		if (Anim->MovieScene->GetName().Contains(LastSegment))
		{
			return Anim;
		}
	}
	return nullptr;
}
