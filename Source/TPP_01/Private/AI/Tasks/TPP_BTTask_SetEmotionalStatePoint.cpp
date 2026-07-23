// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/TPP_BTTask_SetEmotionalStatePoint.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "Item/Interactable/TPP_InteractableItem.h"

UTPP_BTTask_SetEmotionalStatePoint::UTPP_BTTask_SetEmotionalStatePoint()
{
	NodeName = TEXT("SetEmotionalStateObject");
	bNotifyTick = true;
}
EBTNodeResult::Type UTPP_BTTask_SetEmotionalStatePoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ATPP_Pongo* Pongo = Cast<ATPP_Pongo>(AIController->GetPawn());
	if (!Pongo) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	UWorld* World = Pongo->GetWorld();
	if (!World) return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSys) return EBTNodeResult::Failed;
	
	ATPP_InteractableItem* CurrentItem = Cast<ATPP_InteractableItem>(BlackboardComp->GetValueAsObject(EmotionalStateObject.SelectedKeyName));
	const FVector StartLocation = Pongo->GetActorLocation();
	
	if (CurrentItem == nullptr) return EBTNodeResult::Failed;

	float MinDistance = TNumericLimits<float>::Max();
	FVector BestWorldPoint = FVector::ZeroVector;
	FNavLocation BestProjectedLocation;
	bool bFoundValidPoint = false;
     
       
	for(FAttachPointData& Data : CurrentItem->AttachPoints)
	{
		FVector CurrentWorldPoint = CurrentItem->GetActorTransform().TransformPosition(Data.Position);
		float DistanceSq = FVector::DistSquared(CurrentWorldPoint, StartLocation);
       	
		if (DistanceSq >= MinDistance) continue;

           
		FNavLocation TempProjectedLocation;
		bool bOnNav = NavSys->ProjectPointToNavigation(
				CurrentWorldPoint, TempProjectedLocation, FVector(50.f, 50.f, 250.f));
		
           if (bOnNav)
           {
               MinDistance = DistanceSq;
               BestWorldPoint = CurrentWorldPoint;
               BestProjectedLocation = TempProjectedLocation;
           	   CurrentItem->SavedAttachPointData = Data;
               bFoundValidPoint = true;
           }
       }

       if (bFoundValidPoint)
       {
           FHitResult HitResult;
           FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(SearchForInteractTrace), false);
           TraceParams.AddIgnoredActor(Pongo);

           bool bLineHit = World->LineTraceSingleByChannel(
               HitResult, StartLocation, BestWorldPoint, ECC_Visibility, TraceParams
           );
           AActor* HitActor = HitResult.GetActor();
           bool bVisible = !bLineHit || (HitActor && (HitActor == CurrentItem|| HitActor->IsOwnedBy(CurrentItem)));
       	
           if (bVisible)
           {
               BlackboardComp->SetValueAsVector(LocationStateObject.SelectedKeyName, BestProjectedLocation.Location);
               return EBTNodeResult::Succeeded;
           }
       }
	return EBTNodeResult::Succeeded;
}
