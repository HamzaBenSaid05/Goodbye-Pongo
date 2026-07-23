#include "TPP_01/Public/EventProxies/TPP_AudioProxySubsystem.h"

void UTPP_AudioProxySubsystem::NotifyAudioTableEvent(struct FTPP_AudioRow& Row, class UAudioComponent *& AudioComp, FVector & Location,struct FTPP_AudioParameter& Parameter,class USceneComponent * SourceActor) const
{OnAudioTableEvent.Broadcast(Row,AudioComp,Location,Parameter,SourceActor);
}

void UTPP_AudioProxySubsystem::NotifyAudioTableEventUI(FGameplayTag Tag, struct FTPP_AudioParameter Parameter,USoundSubmix* Submix, ButtonType Button) const {OnAudioTableEventUI.Broadcast(Tag,Parameter,Submix,Button);}
