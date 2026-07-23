#include "AI/Tasks/TPP_BTTaskNode_WaitUntilItemIsNotPossessed.h"

#include "Item/Interactable/TPP_InteractableItem.h"

UTPP_BTTaskNode_WaitUntilItemIsNotPossessed::UTPP_BTTaskNode_WaitUntilItemIsNotPossessed()
{
	NodeName = "Wait Until Item Is Not Possessed";

 	bNotifyTick = true;
}

EBTNodeResult::Type UTPP_BTTaskNode_WaitUntilItemIsNotPossessed::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp))
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Cache component set"));
		}
#endif
		return EBTNodeResult::Failed;
	}
	
	Item = Cast<ATPP_InteractableItem>(CachedBlackboard->GetValueAsObject(ItemKey.SelectedKeyName));
	
	if (!Item)
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Item Null"));
		}
#endif
		return EBTNodeResult::Succeeded;
	}
	
	if (Item->InteractWith() != nullptr)
	{
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::InProgress;
}

void UTPP_BTTaskNode_WaitUntilItemIsNotPossessed::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                           float DeltaSeconds)
{
	if (!Item)
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Item Null Tick"));
		}
#endif
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	
	if (Item->InteractWith() != nullptr)
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Succedded Tick"));
		}
#endif
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	
	// Print debug 

	FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
}

