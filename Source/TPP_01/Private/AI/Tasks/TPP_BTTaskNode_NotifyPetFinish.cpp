#include "AI/Tasks/TPP_BTTaskNode_NotifyPetFinish.h"

UTPP_BTTaskNode_NotifyPetFinish::UTPP_BTTaskNode_NotifyPetFinish()
{
	NodeName = "Notify Pet Finish";
}

EBTNodeResult::Type UTPP_BTTaskNode_NotifyPetFinish::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!CacheObjects(OwnerComp))
	{
		return EBTNodeResult::Failed;
	}

	if (CachedPongoController->Implements<UTPP_AIPettable>())
	{
		ITPP_AIPettable::Execute_PetFinished(CachedPongoController.Get());
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
