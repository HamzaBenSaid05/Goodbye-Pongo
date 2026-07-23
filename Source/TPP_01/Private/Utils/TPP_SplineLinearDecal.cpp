#include "Utils/TPP_SplineLinearDecal.h"

#include "Components/SplineComponent.h"
#include "Components/DecalComponent.h"

ATPP_SplineLinearDecal::ATPP_SplineLinearDecal()
{
    Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
    RootComponent = Spline;
}

void ATPP_SplineLinearDecal::GenerateSingleDecal()
{
    ClearDecals();

    if (Spline->GetNumberOfSplinePoints() < 2)
    {
        UE_LOG(LogTemp, Warning, TEXT("Need at least 2 spline points"));
        return;
    }

    SplineLinearDecalData.CheckData(SplineLinearDecalData);
    
    FVector Start = Spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);

    int32 LastIndex = Spline->GetNumberOfSplinePoints() - 1;

    FVector End = Spline->GetLocationAtSplinePoint(LastIndex, ESplineCoordinateSpace::World);

    FVector Direction = End - Start;

    float Length = Direction.Length();

    FVector MidPoint = (Start + End) * 0.5f; // Because the strech start from the pivot (center)

    FRotator Rotation = Direction.Rotation();

    UDecalComponent* Decal = NewObject<UDecalComponent>(this);

    if (!Decal)
    {
        return;
    }

    Decal->RegisterComponent();

    AddInstanceComponent(Decal);

    Decal->AttachToComponent(
        RootComponent,
        FAttachmentTransformRules::KeepWorldTransform
    );

    Decal->SetWorldLocation(MidPoint);

    Decal->SetWorldRotation(Rotation);

    // X = depth
    // Y = length
    // Z = width
    Decal->DecalSize = FVector(
        Length * 0.5f,
        SplineLinearDecalData.Width,
        SplineLinearDecalData.Width
    );
    
    if (SplineLinearDecalData.Material)
    {
        Decal->SetDecalMaterial(SplineLinearDecalData.Material);
    }
    
    Decal->SetDecalColor(SplineLinearDecalData.Color);
    
    Decal->SetFadeScreenSize(0.001f);
}

void ATPP_SplineLinearDecal::ClearDecals() const
{
    TArray<UDecalComponent*> Decals;
    GetComponents<UDecalComponent>(Decals);

    for (UDecalComponent* Decal : Decals)
    {
        Decal->DestroyComponent();
#if WITH_EDITOR
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Destroying old decals"); }
#endif
    }
}

void ATPP_SplineLinearDecal::BeginPlay()
{
    Super::BeginPlay();
    RemoveFromRoot();
    Spline->DestroyComponent();
}