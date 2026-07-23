#include "AI/Tasks/TPP_BTTaskNode_CalculateEscapeRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/TPP_EscapeFearRange.h"

UTPP_BTTaskNode_CalculateEscapeRange::UTPP_BTTaskNode_CalculateEscapeRange() { NodeName = "CalculateEscapeRange"; }

EBTNodeResult::Type UTPP_BTTaskNode_CalculateEscapeRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp))
	{
		return EBTNodeResult::Failed;
	}

	AActor* CurrentEmotionalStateObject = Cast<AActor>(CachedBlackboard->GetValueAsObject(EmotionalStateObjectKey.SelectedKeyName));
	if (!CurrentEmotionalStateObject)
		return EBTNodeResult::Failed;

	if (UTPP_EscapeFearRange* EscapeFearRange = CurrentEmotionalStateObject->GetComponentByClass<UTPP_EscapeFearRange>())
	{
		CachedBlackboard->SetValueAsFloat(OuterRangeKey.SelectedKeyName, EscapeFearRange->GetScaledSphereRadius() + EscapeOffset);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
