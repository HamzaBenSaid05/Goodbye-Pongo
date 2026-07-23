#include "Utils/TPP_SplineDecalActor.h"

#include "Components/SplineComponent.h"
#include "Components/DecalComponent.h"

ATPP_SplineDecalActor::ATPP_SplineDecalActor()
{
    Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
    RootComponent = Spline;
}

void ATPP_SplineDecalActor::ClearDecals() const
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

void ATPP_SplineDecalActor::BeginPlay()
{
    Super::BeginPlay();
    RemoveFromRoot();
    Spline->DestroyComponent();
}

void ATPP_SplineDecalActor::GenerateDecals()
{
    if (SplineDecalData.Num() == 0 || Spline->GetNumberOfSplinePoints() == 0)
    {
#if WITH_EDITOR
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Generating Decals ..."); }
#endif    
        return;
    }

    if (SplineDecalData.Num() != Spline->GetNumberOfSplinePoints())
    {
#if WITH_EDITOR
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Number of spline point and spline decal data are different"); }
#endif    
        return;
    }
    

    // Clean existing decals
    ClearDecals();
    
    int32 LastIndex = Spline->GetNumberOfSplinePoints() - 1;

    for (int32 i = 0; i <= LastIndex; i++)
    {
        FTransform PointTransform = Spline->GetTransformAtSplinePoint(i, ESplineCoordinateSpace::Local);
        
        const FSplineDecalData& Data = SplineDecalData[i];
        
        Data.CheckData(Data);

        SetUpDecalComponent(PointTransform, Data);
    }

    // Notify editor the actor update
#if WITH_EDITOR
    MarkPackageDirty();
    GetWorld()->GetCurrentLevel()->MarkPackageDirty();
#endif
}

void ATPP_SplineDecalActor::SetUpDecalComponent(const FTransform& RelativeTransform, const FSplineDecalData& Data)
{
    UDecalComponent* Decal = NewObject<UDecalComponent>(this, UDecalComponent::StaticClass(), NAME_None, RF_Transactional);

    if (!Decal) return;

    // Component registration
    Decal->RegisterComponent();

    // Add instance in the scene that will persist in editor 
    AddInstanceComponent(Decal);

    // Attach actor
    Decal->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    Decal->SetRelativeTransform(RelativeTransform);

    // Rotation offset
    FRotator CurrentRot = Decal->GetRelativeRotation();
    Decal->SetRelativeRotation(CurrentRot + Data.RotationOffset);

    // Location offset
    FVector CurrentLoc= Decal->GetRelativeLocation();
    Decal->SetRelativeLocation(CurrentLoc + Data.LocationOffset);

    // Decal Size
    Decal->DecalSize = FVector(Data.Height, Data.Width, Data.Width);

    // Material and color
    if (Data.Material)
    {
        Decal->SetDecalMaterial(Data.Material);
    }

    Decal->SetFadeScreenSize(0.001f);

    Decal->SetDecalColor(Data.Color);
}           