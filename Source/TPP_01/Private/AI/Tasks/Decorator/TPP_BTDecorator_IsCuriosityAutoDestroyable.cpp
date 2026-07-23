#include "AI/Tasks/Decorator/TPP_BTDecorator_IsCuriosityAutoDestroyable.h"

#include "Components/TPP_EmotionalObjectComp.h"

UTPP_BTDecorator_IsCuriosityAutoDestroyable::UTPP_BTDecorator_IsCuriosityAutoDestroyable() { NodeName = "Is Curiosity Auto Destroyable"; }

bool UTPP_BTDecorator_IsCuriosityAutoDestroyable::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
		return false;
	const AActor* Actor = Cast<AActor>(CachedBlackboard->GetValueAsObject(BlackboardKey.SelectedKeyName));
	if (!Actor)
		return false;
	const UTPP_EmotionalObjectComp* EmotionalObjectComp = Actor->GetComponentByClass<UTPP_EmotionalObjectComp>();
	if (!EmotionalObjectComp)
		return false;
	if (EmotionalObjectComp->GetEmotionalState() != ETPP_PongoState::Curiosity)
		return false;
	return EmotionalObjectComp->GetbIsAutoDestroy();
}
