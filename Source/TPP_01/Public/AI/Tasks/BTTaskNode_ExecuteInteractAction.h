#pragma once

#include "CoreMinimal.h"
#include "Base/TPP_BTTaskNode_Base.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "BTTaskNode_ExecuteInteractAction.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExecuteInteraction, Log, All);


USTRUCT()
struct FBTExecuteInteractMemory
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ATPP_InteractableItem> Item = nullptr;
	
	UPROPERTY()
	FVector FinalDestination = FVector::ZeroVector;

	UPROPERTY()
	FVector DragDestination = FVector::ZeroVector;
	
	UPROPERTY()
	bool bIsWaitingForPlayer = false;

	UPROPERTY()
	FVector CachedDragDir = FVector::ZeroVector;
};

UCLASS()
class TPP_01_API UTPP_BTTaskNode_ExecuteInteractAction : public UTPP_BTTaskNode_Base
{
	GENERATED_BODY()

public:
	UTPP_BTTaskNode_ExecuteInteractAction();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;
	void CleanupInteraction(UBehaviorTreeComponent& OwnerComp, const FBTExecuteInteractMemory* Memory, bool bClearItemKey) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ItemToInteractKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsInteractingKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	float DistanceToEnd = 100.f;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float TraceStartOffset = 20.f; 

	UPROPERTY(EditAnywhere, Category = "AI")
	float TraceLength = 150.f;
};