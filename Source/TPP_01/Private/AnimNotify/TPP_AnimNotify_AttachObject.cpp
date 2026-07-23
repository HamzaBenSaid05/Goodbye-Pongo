#include "AnimNotify/TPP_AnimNotify_AttachObject.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UTPP_AnimNotify_AttachObject::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	AController* Controller = nullptr;
	if (APawn* Pawn = Cast<APawn>(Owner))
		Controller = Pawn->GetController();
	if (!Controller) return;

	UBlackboardComponent* Blackboard = nullptr;
	if (AAIController* AIC = Cast<AAIController>(Controller))
		Blackboard = AIC->GetBlackboardComponent();
	if (!Blackboard) return;

	UObject* BlackboardObject = Blackboard->GetValueAsObject(EmotionalObjectKeyName);
	if (!BlackboardObject) return;

	AActor* TargetActor = Cast<AActor>(BlackboardObject);
	if (!TargetActor) return;

	// Disable Physics  on the target actor's root component
	if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(TargetActor->GetRootComponent()))
	{
		RootPrim->SetSimulatePhysics(false);
		RootPrim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Attach
	FAttachmentTransformRules Rules(
		EAttachmentRule::SnapToTarget,   
		EAttachmentRule::SnapToTarget,   
		EAttachmentRule::SnapToTarget,   
		false                            
	);
	if (!MeshComp->DoesSocketExist(SocketHandName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket %s not found"), *SocketHandName.ToString());
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("MeshComp: %s"), *GetNameSafe(MeshComp));
	UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh: %s"), *GetNameSafe(MeshComp->GetSkeletalMeshAsset()));
	UE_LOG(LogTemp, Warning, TEXT("Skeleton: %s"), *MeshComp->SkeletalMesh->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Socket: %s"), *SocketHandName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Exists: %d"), MeshComp->DoesSocketExist(SocketHandName));
	TargetActor->AttachToComponent(
		MeshComp,
		Rules,
		SocketHandName  
	);
}