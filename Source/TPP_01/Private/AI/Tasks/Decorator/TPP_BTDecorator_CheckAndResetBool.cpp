#include "AI/Tasks/Decorator/TPP_BTDecorator_CheckAndResetBool.h"

#include "BehaviorTree/BlackboardComponent.h"

UTPP_BTDecorator_CheckAndResetBool::UTPP_BTDecorator_CheckAndResetBool()
{
	bNotifyDeactivation = true;
	NodeName = "Check And Reset Bool";
}

bool UTPP_BTDecorator_CheckAndResetBool::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                    uint8* NodeMemory) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
		return false;
	return CachedBlackboard->GetValueAsBool(BlackboardKey.SelectedKeyName);
}

void UTPP_BTDecorator_CheckAndResetBool::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData,
                                                            EBTNodeResult::Type NodeResult)
{
	Super::OnNodeDeactivation(SearchData, NodeResult);
	if (CachedBlackboard.IsValid())
		CachedBlackboard->SetValueAsBool(BlackboardKey.SelectedKeyName, false);
}

