#include "AI/Tasks/Decorator/TPP_BTDecorator_IsInEmotionalState.h"

#include "AI/Enum/TPP_PongoEnum.h"

UTPP_BTDecorator_IsInEmotionalState::UTPP_BTDecorator_IsInEmotionalState() { NodeName = "Is In Emotional State";}

bool UTPP_BTDecorator_IsInEmotionalState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
		return false;

	ETPP_PongoState CurrentState = static_cast<ETPP_PongoState>(CachedBlackboard->GetValueAsEnum(BlackboardKey.SelectedKeyName));
	return CurrentState == ETPP_PongoState::Fear || CurrentState == ETPP_PongoState::Curiosity;
}
