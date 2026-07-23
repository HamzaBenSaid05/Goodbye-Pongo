#include "AI/Tasks/Decorator/Base/TPP_BTDecorator_Base.h"

bool UTPP_BTDecorator_Base::CacheObjects(UBehaviorTreeComponent& OwnerComp) const
{
	CachedController = OwnerComp.GetAIOwner();
	CachedBlackboard = OwnerComp.GetBlackboardComponent();
	CachedPawn = CachedController.IsValid() ? CachedController->GetPawn() : nullptr;

	return CachedController.IsValid() && CachedPawn.IsValid() && CachedBlackboard.IsValid();
}

bool UTPP_BTDecorator_Base::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	CacheObjects(OwnerComp);

	return CachedController.IsValid()
		&& CachedPawn.IsValid()
		&& CachedBlackboard.IsValid();
}
