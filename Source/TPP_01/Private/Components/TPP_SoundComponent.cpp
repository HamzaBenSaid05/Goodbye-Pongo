#include "Components/TPP_SoundComponent.h"

#include "GameplayTagContainer.h"
#include "Components/AudioComponent.h"
#include "EventProxies/TPP_AudioProxySubsystem.h"
#include "SoundSystem/Data/TPP_AudioRow.h"

USoundBase* UTPP_SoundComponent::FindSound(FGameplayTag Tag)
{
	FTPP_AudioRow* Row = FindGamplayTag(Tag);
	if (Row == nullptr || Row->Sound == nullptr) return nullptr;
	return Row->Sound;
}

FTPP_AudioRow* UTPP_SoundComponent::FindGamplayTag(FGameplayTag Tag) const
{
	static const FString ContextString(TEXT("Tag Search Context"));

	TArray<FTPP_AudioRow*> AllRows;
	Table->GetAllRows<FTPP_AudioRow>(ContextString, AllRows);
	for (FTPP_AudioRow* Row : AllRows)
	{
		if (Row && Row->ItemTag == Tag)
		{
			return Row;
		}
	}
	return nullptr;
}

void UTPP_SoundComponent::PlaySound(FGameplayTag Tag, FVector SourceLocation, UAudioComponent*& AudioComp, FTPP_AudioParameter Parameter, USceneComponent* SourceActor)
{
	if (Tag == FGameplayTag::EmptyTag || Table == nullptr) { return; }

	FTPP_AudioRow* Row = FindGamplayTag(Tag);
	
	if (Row == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Sound doesn't exist in table"));
		return;
	}
	Parameter.Name = Row->Parameter.Name;
	GetWorld()->GetSubsystem<UTPP_AudioProxySubsystem>()->NotifyAudioTableEvent(*Row, AudioComp, SourceLocation,Parameter,SourceActor);
}

void UTPP_SoundComponent::SetAudioComponent(UAudioComponent*& AudioComponent, FGameplayTag Tag)
{
	if (!AudioComponent)
	{
		AudioComponent = NewObject<UAudioComponent>(this);

		AudioComponent->RegisterComponent();

		AudioComponent->bAutoActivate = false;
		FTPP_AudioRow* Row = FindGamplayTag(Tag);
	}
}

//TPP_AudioSpatialization UTPP_SoundComponent::SetSpatializationType(FTPP_AudioRow* Row,UAudioComponent*& AudioComponent) const
//{
//	switch (Row->SpatializationType)
//	{
//	case TPP_AudioSpatialization::TwoD:
//		AudioComponent = 
//		break;
//		
//	case TPP_AudioSpatialization::ThreeDs:
//
//		break;
//		
//	default:
//
//		break;
//	}
//}
