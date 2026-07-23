#include "AI/Tasks/TPP_BTTask_SearchForInteract.h"
#include "Item/Interactable/TPP_InteractableItem.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "NavigationSystem.h"
#include "Characters/Player/TPP_PlayerCharacter.h"
#include "Components/TPP_EmotionalObjectComp.h"
#include "DataAsset/InteractActions/TPP_InteractAction_DA.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

UTPP_BTTask_SearchForInteract::UTPP_BTTask_SearchForInteract()
{
    NodeName = TEXT("Search for Interact");
    bNotifyTick = false;
}

EBTNodeResult::Type UTPP_BTTask_SearchForInteract::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (!CacheObjects(OwnerComp)) { return EBTNodeResult::Failed; }

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys) return EBTNodeResult::Failed;

    ATPP_PlayerCharacter* Player = Cast<ATPP_PlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    auto ClearInteractionData = [&]()
    {
       CachedBlackboard->ClearValue(ItemToInteractKey.SelectedKeyName);
       CachedBlackboard->ClearValue(TargetLocationKey.SelectedKeyName);
       CachedBlackboard->ClearValue(TargetRotationKey.SelectedKeyName);
       CachedBlackboard->SetValueAsBool(IsInteractingKey.SelectedKeyName, false);
    };

    // Pongo is already holding an item
    CachedPongo = Cast<ATPP_Pongo>(CachedPongo.Get());

  
    if (ATPP_InteractableItem* CurrentItem = CachedPongo->GetInteractableItem())
    {
       if (IsValid(CurrentItem))
       {
          CachedBlackboard->SetValueAsObject(ItemToInteractKey.SelectedKeyName, CurrentItem);
          CachedBlackboard->SetValueAsVector(TargetLocationKey.SelectedKeyName, CachedPongo->GetActorLocation());
          CachedBlackboard->SetValueAsRotator(TargetRotationKey.SelectedKeyName, CurrentItem->SavedAttachPointData.Rotation);
          return EBTNodeResult::Succeeded;
       }
    }

    // Sphere overlap to find nearby interactable items
    TArray<FOverlapResult> Overlaps;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractionLookRadius);
    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
    ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

    if (!GetWorld()->OverlapMultiByObjectType(Overlaps, CachedPongo->GetActorLocation(), FQuat::Identity, ObjectQueryParams, Sphere))
    {
       ClearInteractionData();
       // Debug
#if WITH_EDITOR
       if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("[SearchForInteract] No overlaps found")); }
#endif
       return EBTNodeResult::Failed;
    }

    // Filter valid interactable items
    TArray<ATPP_InteractableItem*> Candidates;
    for (const FOverlapResult& Overlap : Overlaps)
    {
       if (ATPP_InteractableItem* Item = Cast<ATPP_InteractableItem>(Overlap.GetActor()))
       {
          // Skip if item is currently held by player
          if (Player && Player->GetInteractableItem() == Item)
             continue;

          if (Item->Tags.Contains(FName("OnPedestal")))
             continue;

          if (Item)
          {
              if (Item->GetInteractData()) 
              {
                  if(Item->GetInteractData()->GetObjectInteractionType() == EObjectInteractionType::ElectricSwitch)
                      continue;
              }   
          }
             
          // Skip items that are currently moving
          if (Item->GetVelocity().Size() > KINDA_SMALL_NUMBER)
             continue;
          // Skip items without interact data
          if (Item->GetInteractData() == nullptr)
             continue;
          
          if (IsValid(Item) && !Candidates.Contains(Item))
             Candidates.Add(Item);
       }
    }

    if (Candidates.Num() == 0)
    {
       ClearInteractionData();
       return EBTNodeResult::Failed;
    }

    // Sort candidates by distance to Pongo
    Candidates.Sort([this](const ATPP_InteractableItem& A, const ATPP_InteractableItem& B)
    {
       return FVector::DistSquared(A.GetActorLocation(), CachedPongo->GetActorLocation()) <
          FVector::DistSquared(B.GetActorLocation(), CachedPongo->GetActorLocation());
    });

    const FVector StartLocation = CachedPongo->GetMesh()->GetBoneLocation(BoneHeadName);

    // Otteniamo le dimensioni della capsula di Pongo per il cast
    float PongoRadius = 34.f;
    float PongoHalfHeight = 88.f;
    if (UCapsuleComponent* PongoCapsule = CachedPongo->FindComponentByClass<UCapsuleComponent>())
    {
        PongoRadius = PongoCapsule->GetScaledCapsuleRadius();
        PongoHalfHeight = PongoCapsule->GetScaledCapsuleHalfHeight();
    }
    FCollisionShape PongoCapsuleShape = FCollisionShape::MakeCapsule(PongoRadius, PongoHalfHeight);

    // For each candidate, find the closest attach point and check line of sight
    for (ATPP_InteractableItem* Item : Candidates)
    {
       if (!IsValid(Item)) continue;

       float MinDistance = TNumericLimits<float>::Max();
       FVector BestWorldPoint = FVector::ZeroVector;
       FNavLocation BestProjectedLocation;
       bool bFoundValidPoint = false;

       // Find closest attach point
       for (FAttachPointData& Data : Item->AttachPoints)
       {
          FVector CurrentWorldPoint = Item->GetActorTransform().TransformPosition(Data.Position);
          float DistSq = FVector::DistSquared(CurrentWorldPoint, StartLocation);

#if WITH_EDITOR
          DrawDebugSphere(GetWorld(), CurrentWorldPoint, 10.f, 12, FColor::Green, false, 1.f);
#endif
          if (DistSq >= MinDistance) continue;

          FNavLocation TempProjected;
          if (NavSys->ProjectPointToNavigation(CurrentWorldPoint, TempProjected, FVector(50.f, 50.f, 250.f)))
          {
             bool bHitPlayer = false;
             
             if (Item->GetInteractData()->InteractionType != EObjectInteractionType::Large)
             {
                 FVector CapsuleCheckLocation = TempProjected.Location + FVector(0.f, 0.f, PongoHalfHeight);
                 
                 TArray<FOverlapResult> CapsuleOverlaps;
                 FCollisionQueryParams CapsuleParams;
                 CapsuleParams.AddIgnoredActor(CachedPongo.Get()); 
                 
                 if (GetWorld()->OverlapMultiByChannel(CapsuleOverlaps, CapsuleCheckLocation, FQuat::Identity, ECC_Pawn, PongoCapsuleShape, CapsuleParams))
                 {
                     for (const FOverlapResult& CapsuleOverlap : CapsuleOverlaps)
                     {
                         if (CapsuleOverlap.GetActor() == Player)
                         {
                             bHitPlayer = true;
                             break;
                         }
                     }
                 }

#if WITH_EDITOR
                 DrawDebugCapsule(GetWorld(), CapsuleCheckLocation, PongoHalfHeight, PongoRadius, FQuat::Identity, bHitPlayer ? FColor::Red : FColor::Cyan, false, 1.f);
#endif
             }
             
             // Se la capsula compenetra il Player (e il check è stato fatto), questo attach point non è valido
             if (bHitPlayer) continue;
             // === FINE CAPSULE CAST ===

             MinDistance = DistSq;
             BestWorldPoint = CurrentWorldPoint;
             BestProjectedLocation = TempProjected;
             Item->SavedAttachPointData = Data;
             bFoundValidPoint = true;
          }
       }

       if (!bFoundValidPoint) continue;

       // Line of sight check
       FHitResult HitResult;
       FCollisionQueryParams TraceParams;
       TraceParams.AddIgnoredActor(CachedPongo.Get());
       TraceParams.AddIgnoredActor(Player);

       const bool bLineHit = GetWorld()->LineTraceSingleByChannel(
                                                                  HitResult, StartLocation, BestWorldPoint, ECC_Visibility, TraceParams);
#if WITH_EDITOR
       DrawDebugLine(GetWorld(), StartLocation, BestWorldPoint, bLineHit ? FColor::Red : FColor::Green, false, 1.f, 0, 2.f);
#endif
       const AActor* HitActor = HitResult.GetActor();
       const bool bVisible = !bLineHit || (HitActor && (HitActor == Item || HitActor->IsOwnedBy(Item)));
       
       // Screen message if hit something
#if WITH_EDITOR
       if (bVisible)
       {
          if (GEngine)
          {
             GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("[SearchForInteract] Visible candidate: %s"), *Item->GetName()));
          }
       }
       else
       {
          if (GEngine)
          {
             GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("[SearchForInteract] Obstructed candidate: %s"), *HitActor->GetName()));
          }
       }
#endif

       if (!bVisible) continue;

       // Emotional item guard
       if (Player && Player->GetInteractableItem() == Item)
       {
          if (Item->FindComponentByClass<UTPP_EmotionalObjectComp>())
             return EBTNodeResult::Failed;
       }

       CachedPongo->Execute_TogglePlayerCollision(CachedPongo.Get(), false);
       // Saving data on blackboard 
       CachedBlackboard->SetValueAsObject(ItemToInteractKey.SelectedKeyName, Item);
       CachedBlackboard->SetValueAsVector(TargetLocationKey.SelectedKeyName, BestProjectedLocation.Location);
       CachedBlackboard->SetValueAsRotator(TargetRotationKey.SelectedKeyName, Item->SavedAttachPointData.Rotation);

#if WITH_EDITOR
       DrawDebugSphere(GetWorld(), BestProjectedLocation.Location, 10.f, 12, FColor::Yellow, false, 1.f);
#endif
       return EBTNodeResult::Succeeded;
    }

    ClearInteractionData();
    // Debug
#if WITH_EDITOR
    if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("[SearchForInteract] No valid interactable items found")); }
#endif
    return EBTNodeResult::Failed;
}