#include "AI/Tasks/TPP_BTTask_PlayAnimationMontage.h"

#include "GameFramework/CharacterMovementComponent.h"

UTPP_BTTask_PlayAnimationMontage::UTPP_BTTask_PlayAnimationMontage()
{
    NodeName = "Play Animation Montage";
    bCreateNodeInstance = true; 
}

EBTNodeResult::Type UTPP_BTTask_PlayAnimationMontage::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (!CacheObjects(OwnerComp))
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayAnimMontage: CacheObjects failed"));
        return EBTNodeResult::Failed;
    }

    UAnimationAsset* AnimAsset =
        AnimationToPlay.GetValue<UAnimationAsset>(OwnerComp);

    if (!AnimAsset)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("PlayAnimMontage: AnimationAsset null"));
        return EBTNodeResult::Failed;
    }

    CachedPongoController->StopMovement();

    if (UCharacterMovementComponent* CMC =
        CachedPongo->GetCharacterMovement())
    {
        CMC->StopMovementImmediately();
        CMC->Velocity = FVector::ZeroVector;
    }

    UAnimInstance* AnimInst =
        CachedPongo->GetMesh()->GetAnimInstance();

    if (!AnimInst)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("PlayAnimMontage: AnimInstance null"));
        return EBTNodeResult::Failed;
    }

    UAnimMontage* Montage = nullptr;

    if (UAnimMontage* SourceMontage =
        Cast<UAnimMontage>(AnimAsset))
    {
        float Duration =
            AnimInst->Montage_Play(SourceMontage, 1.0f);

        if (Duration <= 0.f)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("PlayAnimMontage: Montage_Play fallita"));
            return EBTNodeResult::Failed;
        }

        Montage = SourceMontage;
    }
    else if (UAnimSequenceBase* SequenceBase =
             Cast<UAnimSequenceBase>(AnimAsset))
    {
        Montage = AnimInst->PlaySlotAnimationAsDynamicMontage(
            SequenceBase,
            TEXT("DefaultSlot"),
            0.1f,
            0.3f,
            1.0f,
            bLooping.GetValue(OwnerComp) ? MAX_int32 : 1);

        if (!Montage)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("PlayAnimMontage: impossibile creare Dynamic Montage"));
            return EBTNodeResult::Failed;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning,
            TEXT("PlayAnimMontage: tipo asset non supportato (%s)"),
            *AnimAsset->GetClass()->GetName());

        return EBTNodeResult::Failed;
    }

    CachedMontage = Montage;

    UE_LOG(LogTemp, Warning,
        TEXT("PlayAnimMontage: Montage avviato con successo"));

    if (!bNonBlocking.GetValue(OwnerComp) &&
        !bLooping.GetValue(OwnerComp))
    {
        AnimInst->OnMontageEnded.AddDynamic(
            this,
            &UTPP_BTTask_PlayAnimationMontage::OnMontageEnded);

        return EBTNodeResult::InProgress;
    }

    return EBTNodeResult::Succeeded;
}

void UTPP_BTTask_PlayAnimationMontage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != CachedMontage) return; 

    if (!CachedOwnerComp.IsValid()) return;

    if (UAnimInstance* AnimInst = CachedPongo->GetMesh()->GetAnimInstance())
        AnimInst->OnMontageEnded.RemoveDynamic(
            this, &UTPP_BTTask_PlayAnimationMontage::OnMontageEnded);

    CachedMontage = nullptr;
    FinishLatentTask(*CachedOwnerComp, 
        bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UTPP_BTTask_PlayAnimationMontage::AbortTask(
    UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (CacheObjects(OwnerComp))
    {
        if (UAnimInstance* AnimInst = CachedPongo->GetMesh()->GetAnimInstance())
        {
            AnimInst->OnMontageEnded.RemoveDynamic(
                this, &UTPP_BTTask_PlayAnimationMontage::OnMontageEnded);
            if (CachedMontage)
                AnimInst->Montage_Stop(0.2f, CachedMontage);
        }
    }
    CachedMontage = nullptr;
    return EBTNodeResult::Aborted;
}
