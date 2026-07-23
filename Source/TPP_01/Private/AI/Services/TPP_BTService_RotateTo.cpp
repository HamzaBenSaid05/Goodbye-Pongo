#include "AI/Services/TPP_BTService_RotateTo.h"

#include "BehaviorTree/BlackboardComponent.h"

UTPP_BTService_RotateTo::UTPP_BTService_RotateTo()
{
	NodeName = "Rotate To";
}

void UTPP_BTService_RotateTo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	
	if (!CacheObjects(OwnerComp))
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
											 "Cache Objects  Failed");
		}
#endif
		return;
	}
	
	// Rotate toward target only if we are still
	const float VelocitySizeSquared = CachedPongo->GetVelocity().SizeSquared();
	
	if (CachedPongo.Get() && VelocitySizeSquared < FMath::Square(RotationThreshold))
	{
		//UAnimInstance* AnimInst = CachedPongo->GetMesh()->GetAnimInstance();
		//if (AnimInst && AnimInst->IsAnyMontagePlaying())
		//	return;
		AActor* TargetActor = Cast<AActor>(
										   CachedBlackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
		if (!TargetActor) return;
		if (const bool bTargetMoving = !TargetActor->GetVelocity().IsNearlyZero(1.0f))
		{
			return;
		}
		if (TargetActor->GetAttachParentActor() == CachedPongo.Get())
		{
			return;
		}
		FVector ToTarget =
			TargetActor->GetActorLocation() - CachedPongo->GetActorLocation();
		ToTarget.Z = 0.f;

		if (ToTarget.IsNearlyZero()) return;
		ToTarget.Normalize();
		const FRotator CurrentRot = CachedPongo->GetActorRotation();
		FRotator TargetRot = ToTarget.Rotation();
		TargetRot.Pitch = 0.f;
		TargetRot.Roll  = 0.f;
		
		const float YawDiff = FMath::Abs(FRotator::NormalizeAxis(CurrentRot.Yaw - TargetRot.Yaw));

		if (YawDiff < AlignmentThreshold) 
		{
			return; 
		}
		UE_LOG(LogTemp, Warning,
			TEXT("CurrentYaw=%.2f TargetYaw=%.2f Diff=%.2f"),
			CurrentRot.Yaw,
			TargetRot.Yaw,
			YawDiff);
		
		FRotator NewRot = FMath::RInterpTo(
												 CurrentRot,
												 TargetRot,
												 DeltaSeconds,
												 RotationInterpSpeed);
		NewRot.Pitch = 0.f;
		NewRot.Roll = 0.f;
		CachedPongo->SetActorRotation(NewRot);
	}	
}
