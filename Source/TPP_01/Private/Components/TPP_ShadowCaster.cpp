#include "Components/TPP_ShadowCaster.h"

UTPP_ShadowCaster::UTPP_ShadowCaster()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceLength = 500.0f;
}

void UTPP_ShadowCaster::BeginPlay()
{
	Super::BeginPlay();

	DecalSize.X = TraceLength;
	StartingSize = DecalSize.Y;
	LengthMultiplier = 1 / TraceLength;
	
	TraceDelegate.BindUObject(this, &UTPP_ShadowCaster::OnTraceCompleted);
}

void UTPP_ShadowCaster::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	const FVector Start = GetOwner()->GetActorLocation();
	const FVector End = Start + FVector::DownVector * TraceLength;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	FCollisionResponseParams ResponseParams = FCollisionResponseParams::DefaultResponseParam;
	
	GetWorld()->AsyncSweepByChannel
	(
		EAsyncTraceType::Single,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeBox(FVector(10.0f)),
		QueryParams,
		ResponseParams,
		&TraceDelegate
	);
}

void UTPP_ShadowCaster::OnTraceCompleted(const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum) 
{
	if (TraceDatum.OutHits.IsEmpty())
	{
		return;
	}

	const float Distance = TraceDatum.OutHits[0].Distance;
	const float Alpha = Distance * LengthMultiplier * ScaleMultiplier;
	const float Size = FMath::Lerp(StartingSize, 0, Alpha);
	const FVector NewDecalSize = FVector(TraceLength, Size, Size);
	DecalSize = NewDecalSize;
}
