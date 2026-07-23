#include "AI/Tasks/Base/TPP_BTTaskNode_Base.h"

EBTNodeResult::Type UTPP_BTTaskNode_Base::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp))
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
											 "CacheObjects Failed");
		}
#endif
		return EBTNodeResult::Failed;
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

bool UTPP_BTTaskNode_Base::CacheObjects(UBehaviorTreeComponent& OwnerComp) const
{
	CachedPongoController =
		Cast<ATPP_PongoController>(OwnerComp.GetAIOwner());

	CachedBlackboard = OwnerComp.GetBlackboardComponent();

	if (!CachedPongoController.IsValid())
	{
		return false;
	}

	APawn* Pawn = CachedPongoController->GetPawn();

	CachedPongo = Cast<ATPP_Pongo>(Pawn);

	if (!CachedPongo.IsValid())
	{
		return false;
	}

	CachedOwnerComp = &OwnerComp;

	return CachedBlackboard.IsValid();
}
