#include "Gameplay/GameModes/TPP_TestingGameMode.h"

#include "Camera/TPP_Camera.h"
#include "Characters/Player/TPP_PlayerCharacter.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "Kismet/GameplayStatics.h"

void ATPP_TestingGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (const auto Camera = Cast<ATPP_Camera>(UGameplayStatics::GetActorOfClass(GetWorld(), ATPP_Camera::StaticClass())))
	{
		const auto PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		PC->SetViewTarget(Camera);
		Camera->bIsInUse = true;
		Camera->SetPlayerRef(PC->GetPawn());
		Camera->CalculateEdgePointsFromSpline();
	}

	if (AActor* PongoActor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPP_Pongo::StaticClass()))
		PongoRef = Cast<ATPP_Pongo>(PongoActor);

	PlayerControllerRef = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerRef = Cast<ATPP_PlayerCharacter>(PlayerControllerRef.Get()->GetCharacter());
}
