#include "AI/Tasks/Base/TPP_BTService_Base.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/Engine.h"

UTPP_BTService_Base::UTPP_BTService_Base()
{
	NodeName = TEXT("BTService Base");
}

void UTPP_BTService_Base::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (!CacheObjects(OwnerComp))
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::White,
				TEXT("Service CacheObjects Failed - OnBecomeRelevant"));
		}
#endif
	}
}

bool UTPP_BTService_Base::CacheObjects(UBehaviorTreeComponent& OwnerComp) const
{
	CachedPongoController = Cast<ATPP_PongoController>(OwnerComp.GetAIOwner());
    CachedBlackboard = OwnerComp.GetBlackboardComponent(); 
	CachedPongo = CachedPongoController.IsValid() 
		? Cast<ATPP_Pongo>(CachedPongoController->GetPawn()) 
		: nullptr;
	return CachedPongoController.IsValid()
		&& CachedPongo.IsValid()
		&& CachedBlackboard.IsValid();
}