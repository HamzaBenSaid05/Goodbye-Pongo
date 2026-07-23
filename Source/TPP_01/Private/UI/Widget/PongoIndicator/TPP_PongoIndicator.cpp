#include <UI/Widget/PongoIndicator/TPP_PongoIndicator.h>

#include "AI/Enum/TPP_PongoEnum.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Framework/Application/SlateApplication.h"
#include "UI/Widget/PongoIndicator/TPP_IndicatorFeedback.h"
#include <UI/HUD/TPP_HUDGame.h>

#include "MaterialHLSLTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Pongo/TPP_Pongo.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"


void UTPP_PongoIndicator::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ImageIndicator && ImageIndicator->GetBrush().GetResourceObject())
	{
		DynamicMaterialImageIndicator = ImageIndicator->GetDynamicMaterial();
	}

	if (PongoIcon && PongoIcon->GetBrush().GetResourceObject())
	{
		DynamicMaterialPongoIcon = PongoIcon->GetDynamicMaterial();
	}
}

void UTPP_PongoIndicator::ChangePongoIconBaseMaterial(UMaterialInterface* NewBaseMaterial)
{
	if (!PongoIcon || !NewBaseMaterial) return;
	
	PongoIcon->SetBrushFromMaterial(NewBaseMaterial);
	DynamicMaterialPongoIcon = PongoIcon->GetDynamicMaterial();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("New Material and Dynamic!"));
	}
}

void UTPP_PongoIndicator::SetAngle(float Angle)
{
	if (!SizeBox) return;
	const float Rad = FMath::DegreesToRadians(Angle);

	const FVector2D Pos(
		FMath::Cos(Rad) * Radius,
		FMath::Sin(Rad) * Radius);

	Arrow->SetRenderTranslation(Pos);
	Arrow->SetRenderTransformAngle(Angle);
}

void UTPP_PongoIndicator::SetImageSide(FVector2D Direction)
{
    if (!SizeBox || !Arrow) return;

    FVector2D ViewportSize;
    if (GEngine && GEngine->GameViewport)
    {
       GEngine->GameViewport->GetViewportSize(ViewportSize);
    }
    else
    {
       return;
    }

 
    float DPIScale = UWidgetLayoutLibrary::GetViewportScale(this);
    if (DPIScale > 0.0f)
    {
        ViewportSize /= DPIScale;
    }
   

    if (Direction.IsNearlyZero())
    {
       Direction = FVector2D(1.f, 0.f);
    }
    else
    {
       Direction.Normalize();
    }

    const FVector2D IndicatorSize = SizeBox->GetDesiredSize();
    const FVector2D ArrowSize = Arrow->GetDesiredSize();
	
    const float OffsetX = (IndicatorSize.X * 0.5f) + ArrowSize.X;
    const float OffsetY = (IndicatorSize.Y * 0.5f) + ArrowSize.Y;

  
    const float SafeHalfWidth = FMath::Max(0.f, ((ViewportSize.X * 0.5f) * Margin) - OffsetX);
    const float SafeHalfHeight = FMath::Max(0.f, ((ViewportSize.Y * 0.5f) * Margin) - OffsetY);

    const float ScaleX = Direction.X != 0.f
       ? SafeHalfWidth / FMath::Abs(Direction.X)
       : TNumericLimits<float>::Max();

    const float ScaleY = Direction.Y != 0.f
       ? SafeHalfHeight / FMath::Abs(Direction.Y)
       : TNumericLimits<float>::Max();

    const float FinalScale = FMath::Min(ScaleX, ScaleY);

    FinalPos = Direction * FinalScale;

    if (UOverlaySlot* SlotIndicator = Cast<UOverlaySlot>(SizeBox->Slot))
    {
       SlotIndicator->SetHorizontalAlignment(HAlign_Center);
       SlotIndicator->SetVerticalAlignment(VAlign_Center);
    }

    SideVectorX = Direction.X > 0.f ? -1 : 1;

    SizeBox->SetRenderTranslation(FinalPos);
}

void UTPP_PongoIndicator::SetIndicatorColorAndFrame(FIndicatorStatusData Data)
{
	if (!DynamicMaterialImageIndicator) return;

	DynamicMaterialImageIndicator->SetScalarParameterValue(TEXT("Anim Value"), Data.Frame);
	DynamicMaterialImageIndicator->SetVectorParameterValue(TEXT("ColorToApply"), Data.ColorToApplyAtMaterial);
	ChangePongoIconBaseMaterial(PongoIconOriginalMaterial);
	CachedAnimDuration = Data.AnimationDuration;
	CashedAnimFrame = Data.FrameAnimation;
	PlayFeedbackAnimation(CashedPongoState);
}



void UTPP_PongoIndicator::PlayPongoIconAnimation(bool bIsMoving,float DeltaTime)
{
	float CurrentAngle = PongoIcon->GetRenderTransformAngle();
	float TargetAngle = 0.0f;

	if (bIsMoving)
	{
		RunningTime += DeltaTime * OscillationSpeed;
		TargetAngle = FMath::Sin(RunningTime) * MaxRotationAngle;
		PongoIcon->SetRenderTransformAngle(TargetAngle);
	}
	else
	{
		TargetAngle = 0.0f;
		if (FMath::IsNearlyEqual(CurrentAngle, 0.0f, 0.1f))
		{
			PongoIcon->SetRenderTransformAngle(0.0f);
			RunningTime = 0.0f;
		}
		else
		{
			float SmoothedAngle = FMath::FInterpTo(CurrentAngle, TargetAngle, DeltaTime, OscillationSpeed);
			PongoIcon->SetRenderTransformAngle(SmoothedAngle);
			RunningTime = FMath::Asin(SmoothedAngle / MaxRotationAngle);
		}
	}
}

void UTPP_PongoIndicator::PlayFeedbackAnimation(ETPP_PongoState PongoState)
{
	ElapsedTime = 0.0f;
	CashedPongoState = PongoState;
	CurrentAnimState = ETPP_PongoAnimState::OriginalToZero;
	
	if (CashedPongoState == ETPP_PongoState::Stroll)
	{
		ATPP_GameMode* GameModeRef = Cast<ATPP_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		ATPP_PongoController* PongoController = Cast<ATPP_PongoController>(GameModeRef->GetPongoRef()->GetController());
		UBlackboardComponent* BlackboardComponent = PongoController->GetBlackboardComponent();
		if (BlackboardComponent)
		{
			CachedAnimDuration = BlackboardComponent->GetValueAsFloat(StrollTimeKey)/2;
		}
			
	}
	
	if (GetWorld())
	{
		GEngine->AddOnScreenDebugMessage(10,100,FColor::Red,"Pongo Playing");
		GetWorld()->GetTimerManager().ClearTimer(PongoAnimTimerHandle);
		

		if (CachedAnimDuration<= 0)
			return;
		
		GetWorld()->GetTimerManager().SetTimer(
			PongoAnimTimerHandle, 
			this, 
			&UTPP_PongoIndicator::UpdateFeedbackAnimation, 
			GetWorld()->GetDeltaSeconds(), 
			true
		);
	}
}

void UTPP_PongoIndicator::UpdateFeedbackAnimation()
{
	ElapsedTime += GetWorld()->GetDeltaSeconds();
	switch (CurrentAnimState)
	{
		case ETPP_PongoAnimState::OriginalToZero:
		if (CachedAnimDuration<=0)
			return;
		if (ElapsedTime < CachedAnimDuration)
		{
			float DecreasingAlpha = ElapsedTime / CachedAnimDuration;
			DecreasingAlpha = FMath::Clamp(DecreasingAlpha, 0.0f, 1.0f);
		
			if (GEngine)
			{
				FString TimeMsg = FString::Printf(TEXT("ElapsedTime: %.4f"), ElapsedTime);
				FString AlphaMsg = FString::Printf(TEXT("DecreasingAlpha: %.4f"), DecreasingAlpha);
        
				GEngine->AddOnScreenDebugMessage(1, 0.05f, FColor::Cyan, TimeMsg);
				GEngine->AddOnScreenDebugMessage(2, 0.05f, FColor::Green, AlphaMsg);
				GEngine->AddOnScreenDebugMessage(1000, 0.05f, FColor::Red, TEXT("Pongo UPDATE 0 TO 0"));
			}
		
			if (DynamicMaterialPongoIcon)
			{
				DynamicMaterialPongoIcon->SetScalarParameterValue(TEXT("AlphaToRemove"), DecreasingAlpha);
			}
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Pongo ANIMATION FINISHED (Timer Cleared)"));
			}

			if (DynamicMaterialPongoIcon)
			{
				ChangePongoIconBaseMaterial(PongoIconAnimationMaterial);
				DynamicMaterialPongoIcon->SetScalarParameterValue(TEXT("AlphaToRemove"), 1.0f);
				DynamicMaterialPongoIcon->SetScalarParameterValue(TEXT("Anim Value"), CashedAnimFrame);
				
			}
    
			if (GetWorld())
			{
				//GetWorld()->GetTimerManager().ClearTimer(PongoAnimTimerHandle);
				ElapsedTime = 0.0f;
				CurrentAnimState = ETPP_PongoAnimState::NewToOne;
			
			}
		}
		break;
	case ETPP_PongoAnimState::NewToOne:
		/*if (CachedAnimDuration<=0)
			return;
		if (CashedPongoState== ETPP_PongoState::Stroll)
			CachedAnimDuration = CachedAnimDuration*2;*/
		if (ElapsedTime < CachedAnimDuration)
		{
			float InvertedAlpha = 1.0f - (ElapsedTime / CachedAnimDuration);
			InvertedAlpha = FMath::Clamp(InvertedAlpha, 0.0f, 1.0f);
    
			if (GEngine)
			{
				FString TimeMsg = FString::Printf(TEXT("ElapsedTime: %.4f"), ElapsedTime);
				FString AlphaMsg = FString::Printf(TEXT("InvertedAlpha (1 -> 0): %.4f"), InvertedAlpha);
       
				GEngine->AddOnScreenDebugMessage(1, 0.05f, FColor::Cyan, TimeMsg);
				GEngine->AddOnScreenDebugMessage(2, 0.05f, FColor::Green, AlphaMsg);
				GEngine->AddOnScreenDebugMessage(1000, 0.05f, FColor::Red, TEXT("Pongo UPDATE N TO 1"));
			}
    
			if (DynamicMaterialPongoIcon)
			{
				DynamicMaterialPongoIcon->SetScalarParameterValue(TEXT("AlphaToRemove"), InvertedAlpha);
			}
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Pongo ANIMATION FINISHED (Timer Cleared)"));
			}

			if (DynamicMaterialPongoIcon)
			{
				DynamicMaterialPongoIcon->SetScalarParameterValue(TEXT("AlphaToRemove"), 0.0f);
				//ChangePongoIconBaseMaterial(PongoIconAnimationMaterial);
			}
   
			if (GetWorld())
			{
				//GetWorld()->GetTimerManager().ClearTimer(PongoAnimTimerHandle);
				ElapsedTime = 0.0f;
				CurrentAnimState = ETPP_PongoAnimState::NewToZero;
			}
		}
		break;
	case ETPP_PongoAnimState::NewToZero:
		/*if (CachedAnimDuration<=0)
			return;*/
		if (ElapsedTime < CachedAnimDuration)
		{
			float DecreasingAlpha = ElapsedTime / CachedAnimDuration;
			DecreasingAlpha = FMath::Clamp(DecreasingAlpha, 0.0f, 1.0f);
		
			if (GEngine)
			{
				FString TimeMsg = FString::Printf(TEXT("ElapsedTime: %.4f"), ElapsedTime);
				FString AlphaMsg = FString::Printf(TEXT("DecreasingAlpha: %.4f"), DecreasingAlpha);
        
				GEngine->AddOnScreenDebugMessage(1, 0.05f, FColor::Cyan, TimeMsg);
				GEngine->AddOnScreenDebugMessage(2, 0.05f, FColor::Green, AlphaMsg);
				GEngine->AddOnScreenDebugMessage(1000, 0.05f, FColor::Red, TEXT("Pongo UPDATE N TO 0"));
			}
		
			if (DynamicMaterialPongoIcon)
			{
				DynamicMaterialPongoIcon->SetScalarParameterValue(TEXT("AlphaToRemove"), DecreasingAlpha);
			}
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Pongo ANIMATION FINISHED (Timer Cleared)"));
			}

			if (DynamicMaterialPongoIcon)
			{
				ChangePongoIconBaseMaterial(PongoIconOriginalMaterial);
				DynamicMaterialPongoIcon->SetScalarParameterValue(TEXT("AlphaToRemove"), 1.0f);
			}
    
			if (GetWorld())
			{
				//GetWorld()->GetTimerManager().ClearTimer(PongoAnimTimerHandle);
				ElapsedTime = 0.0f;
				CurrentAnimState = ETPP_PongoAnimState::OriginalToOne;
			}
		}break;
		case ETPP_PongoAnimState::OriginalToOne:
		/*if (CachedAnimDuration<=0)
			return;
		if (CashedPongoState== ETPP_PongoState::Stroll)
			CachedAnimDuration = CachedAnimDuration/2;*/
		if (ElapsedTime < CachedAnimDuration)
		{
			float InvertedAlpha = 1.0f - (ElapsedTime / CachedAnimDuration);
			InvertedAlpha = FMath::Clamp(InvertedAlpha, 0.0f, 1.0f);
    
			if (GEngine)
			{
				FString TimeMsg = FString::Printf(TEXT("ElapsedTime: %.4f"), ElapsedTime);
				FString AlphaMsg = FString::Printf(TEXT("InvertedAlpha (1 -> 0): %.4f"), InvertedAlpha);
       
				GEngine->AddOnScreenDebugMessage(1, 0.05f, FColor::Cyan, TimeMsg);
				GEngine->AddOnScreenDebugMessage(2, 0.05f, FColor::Green, AlphaMsg);
				GEngine->AddOnScreenDebugMessage(1000, 0.05f, FColor::Red, TEXT("Pongo UPDATE O TO 1"));
			}
    
			if (DynamicMaterialPongoIcon)
			{
				DynamicMaterialPongoIcon->SetScalarParameterValue(TEXT("AlphaToRemove"), InvertedAlpha);
			}
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Pongo ANIMATION FINISHED (Timer Cleared)"));
			}

			if (DynamicMaterialPongoIcon)
			{
				DynamicMaterialPongoIcon->SetScalarParameterValue(TEXT("AlphaToRemove"), 0.0f);
				//ChangePongoIconBaseMaterial(PongoIconAnimationMaterial);
			}
   
			if (GetWorld())
			{
				GetWorld()->GetTimerManager().ClearTimer(PongoAnimTimerHandle);
				ElapsedTime = 0.0f;
				CurrentAnimState = ETPP_PongoAnimState::OriginalToOne;
				
			}
		}
		default:
		break;
	}
	
}

void UTPP_PongoIndicator::EvaluateDistance(float Distance)
{
	if (!SizeBox) return;

	Distance = FMath::Max(0.f, Distance);

	const float Alpha = FMath::Clamp(
		Distance / ScalabilityTreshOld,
		0.f,
		1.f);

	const float ScaleValue = FMath::Lerp(
		MaxScale,
		MinScale,
		Alpha);

	SizeBox->SetRenderScale(
		FVector2D(ScaleValue, ScaleValue));
}

FVector2D UTPP_PongoIndicator::GetImageRatio()
{
	if (!SizeBox)
	{
		return FVector2D(1.f, 1.f);
	}

	return SizeBox->GetRenderTransform().Scale;
}

float UTPP_PongoIndicator::GetAngle()
{
	if (!SizeBox)
	{
		return 0.f;
	}

	return SizeBox->GetRenderTransform().Angle;
}
