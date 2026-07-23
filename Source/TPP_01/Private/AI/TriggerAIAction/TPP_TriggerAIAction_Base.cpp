#include "AI/TriggerAIAction/TPP_TriggerAIAction_Base.h"

#include "Components/ShapeComponent.h"

ATPP_TriggerAIAction_Base::ATPP_TriggerAIAction_Base()
{
	GetCollisionComponent()->SetGenerateOverlapEvents(true);
	GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCollisionComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCollisionComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	GetCollisionComponent()->SetCanEverAffectNavigation(false);
	PrimaryActorTick.bCanEverTick = false;
}

void ATPP_TriggerAIAction_Base::BeginPlay()
{
	Super::BeginPlay();
	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(
	                                                            this,
	                                                            &ATPP_TriggerAIAction_Base::OnOverlapBegin
	                                                           );
}

void ATPP_TriggerAIAction_Base::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		PongoAIAction(OtherActor);
		if (bShouldDestroyAfterTrigger)
		{
			Destroy();
		}
	}
}

void ATPP_TriggerAIAction_Base::PongoAIAction(AActor* OtherActor)
{
#if WITH_EDITOR
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::White, TEXT("Pongo Enter Trigger AI Action")); }
#endif
}
