#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "TPP_BTService_Base.generated.h"

UCLASS(Abstract)
class TPP_01_API UTPP_BTService_Base : public UBTService
{
	GENERATED_BODY()

public:
	UTPP_BTService_Base();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	bool CacheObjects(UBehaviorTreeComponent& OwnerComp) const;

protected:
	mutable TWeakObjectPtr<ATPP_PongoController> CachedPongoController;

	UPROPERTY(Transient)
	mutable TWeakObjectPtr<ATPP_Pongo> CachedPongo;

	UPROPERTY(Transient)
	mutable TWeakObjectPtr<UBlackboardComponent> CachedBlackboard;
};