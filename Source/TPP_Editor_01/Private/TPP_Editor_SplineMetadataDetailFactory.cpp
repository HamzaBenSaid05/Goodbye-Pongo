#pragma once

#include "TPP_Editor_SplineMetadataDetailFactory.h"
#include "Camera/TPP_CameraSplineMetadata.h"
#include "IDetailGroup.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "ScopedTransaction.h"
#include "Editor.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "TPP_CameraSplineMetadataDetails"

// Factory Implementation
TSharedPtr<ISplineMetadataDetails> UTPP_Editor_SplineMetadataDetailFactory::Create() { return MakeShared<FTPP_Editor_SplineMetadataDetails>(); }

UClass* UTPP_Editor_SplineMetadataDetailFactory::GetMetadataClass() const { return UTPP_CameraSplineMetadata::StaticClass(); }

// UI Implementation
FName FTPP_Editor_SplineMetadataDetails::GetName() const { return FName(TEXT("TPP_CameraMetadata")); }
FText FTPP_Editor_SplineMetadataDetails::GetDisplayName() const { return LOCTEXT("DisplayName", "Camera Data"); }

template <class T>
bool UpdateMultipleValue(TOptional<T>& CurrentValue, T InValue)
{
	if (!CurrentValue.IsSet()) { CurrentValue = InValue; }
	else if (CurrentValue.IsSet() && CurrentValue.GetValue() != InValue)
	{
		CurrentValue.Reset();
		return false;
	}
	return true;
}

void FTPP_Editor_SplineMetadataDetails::Update(USplineComponent* InSplineComponent, const TSet<int32>& InSelectedKeys)
{
	SplineComp = InSplineComponent;
	SelectedKeys = InSelectedKeys;
	// Reset of all values
	BiasValue.Reset();
	PitchValue.Reset();
	YawValue.Reset();
	RollValue.Reset();
	SpringArmLengthValue.Reset();
	YOffsetValue.Reset();
	ZOffsetValue.Reset();
	BiasActorValue.Reset();

	if (UTPP_CameraSplineMetadata* Metadata = GetMetadata())
	{
		for (int32 Index : InSelectedKeys)
		{
			if (Metadata->CameraBias.Points.IsValidIndex(Index))
			{
				UpdateMultipleValue(BiasValue, Metadata->CameraBias.Points[Index].OutVal);

				UpdateMultipleValue(PitchValue, Metadata->CameraRotationPitch.Points[Index].OutVal);
				UpdateMultipleValue(YawValue, Metadata->CameraRotationYaw.Points[Index].OutVal);
				UpdateMultipleValue(RollValue, Metadata->CameraRotationRoll.Points[Index].OutVal);

				UpdateMultipleValue(SpringArmLengthValue, Metadata->CameraSpringArmLength.Points[Index].OutVal);

				UpdateMultipleValue(YOffsetValue, Metadata->CameraYOffset.Points[Index].OutVal);
				UpdateMultipleValue(ZOffsetValue, Metadata->CameraZOffset.Points[Index].OutVal);

				UpdateMultipleValue(BiasInterpSpeedValue, Metadata->CameraBiasInterpSpeed.Points[Index].OutVal);
				UpdateMultipleValue(LocationInterpSpeedValue, Metadata->CameraLocationInterpSpeed.Points[Index].OutVal);
				UpdateMultipleValue(RotationInterpSpeedValue, Metadata->CameraRotationInterpSpeed.Points[Index].OutVal);

				UpdateMultipleValue(BiasActorValue, Metadata->CameraBiasActor[Index].Get());
			}
		}
	}
}

bool FTPP_Editor_SplineMetadataDetails::OnShouldFilterActor(const AActor* Actor)
{
	if (Actor->IsA(AActor::StaticClass()))
		return true;
	return false;
}

void FTPP_Editor_SplineMetadataDetails::GenerateChildContent(IDetailGroup& DetailGroup)
{
	// CAMERA BIAS
	DetailGroup.AddWidgetRow()
	           .NameContent()[SNew(STextBlock).Text(LOCTEXT("BiasLabel", "Camera Bias")).Font(IDetailLayoutBuilder::GetDetailFont())]
		.ValueContent()[
			SNew(SNumericEntryBox<float>)
			.Value(this, &FTPP_Editor_SplineMetadataDetails::GetBias)
			.OnValueCommitted(this, &FTPP_Editor_SplineMetadataDetails::OnSetBias)
			.UndeterminedString(LOCTEXT("Multiple", "Multiple"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];

	// PITCH
	DetailGroup.AddWidgetRow()
	           .NameContent()[SNew(STextBlock).Text(LOCTEXT("PitchLabel", "Rotation Pitch")).Font(IDetailLayoutBuilder::GetDetailFont())]
		.ValueContent()[
			SNew(SNumericEntryBox<float>)
			.Value(this, &FTPP_Editor_SplineMetadataDetails::GetPitch)
			.OnValueCommitted(this, &FTPP_Editor_SplineMetadataDetails::OnSetPitch)
			.UndeterminedString(LOCTEXT("Multiple", "Multiple"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];

	// YAW
	DetailGroup.AddWidgetRow()
	           .NameContent()[SNew(STextBlock).Text(LOCTEXT("YawLabel", "Rotation Yaw")).Font(IDetailLayoutBuilder::GetDetailFont())]
		.ValueContent()[
			SNew(SNumericEntryBox<float>)
			.Value(this, &FTPP_Editor_SplineMetadataDetails::GetYaw)
			.OnValueCommitted(this, &FTPP_Editor_SplineMetadataDetails::OnSetYaw)
			.UndeterminedString(LOCTEXT("Multiple", "Multiple"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];

	// ROLL
	DetailGroup.AddWidgetRow()
	           .NameContent()[SNew(STextBlock).Text(LOCTEXT("RollLabel", "Rotation Roll")).Font(IDetailLayoutBuilder::GetDetailFont())]
		.ValueContent()[
			SNew(SNumericEntryBox<float>)
			.Value(this, &FTPP_Editor_SplineMetadataDetails::GetRoll)
			.OnValueCommitted(this, &FTPP_Editor_SplineMetadataDetails::OnSetRoll)
			.UndeterminedString(LOCTEXT("Multiple", "Multiple"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];

	// CAMERA SPRING ARM LENGHT
	DetailGroup.AddWidgetRow()
	           .NameContent()[SNew(STextBlock).Text(LOCTEXT("SpringArmLengthLabel", "Camera Spring Arm Length")).Font(IDetailLayoutBuilder::GetDetailFont())]
		.ValueContent()[
			SNew(SNumericEntryBox<float>)
			.Value(this, &FTPP_Editor_SplineMetadataDetails::GetSpringArmLenght)
			.OnValueCommitted(this, &FTPP_Editor_SplineMetadataDetails::OnSetSpringArmLenght)
			.UndeterminedString(LOCTEXT("Multiple", "Multiple"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];

	// CAMERA Y OFFSET
	DetailGroup.AddWidgetRow()
	           .NameContent()[SNew(STextBlock).Text(LOCTEXT("YOffsetLabel", "Camera Y Offset")).Font(IDetailLayoutBuilder::GetDetailFont())]
		.ValueContent()[
			SNew(SNumericEntryBox<float>)
			.Value(this, &FTPP_Editor_SplineMetadataDetails::GetYOffset)
			.OnValueCommitted(this, &FTPP_Editor_SplineMetadataDetails::OnSetYOffset)
			.UndeterminedString(LOCTEXT("Multiple", "Multiple"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];

	// CAMERA Z OFFSET
	DetailGroup.AddWidgetRow()
	           .NameContent()[SNew(STextBlock).Text(LOCTEXT("ZOffsetLabel", "Camera Z Offset")).Font(IDetailLayoutBuilder::GetDetailFont())]
		.ValueContent()[
			SNew(SNumericEntryBox<float>)
			.Value(this, &FTPP_Editor_SplineMetadataDetails::GetZOffset)
			.OnValueCommitted(this, &FTPP_Editor_SplineMetadataDetails::OnSetZOffset)
			.UndeterminedString(LOCTEXT("Multiple", "Multiple"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];

	// CAMERA BIAS INTERP SPEED
	DetailGroup.AddWidgetRow()
	           .NameContent()[SNew(STextBlock).Text(LOCTEXT("BiasInterpSpeedLabel", "Camera Bias Interp Speed")).Font(IDetailLayoutBuilder::GetDetailFont())]
		.ValueContent()[
			SNew(SNumericEntryBox<float>)
			.Value(this, &FTPP_Editor_SplineMetadataDetails::GetBiasInterpSpeed)
			.OnValueCommitted(this, &FTPP_Editor_SplineMetadataDetails::OnSetBiasInterpSpeed)
			.UndeterminedString(LOCTEXT("Multiple", "Multiple"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];

	// CAMERA LOCATION INTERP SPEED
	DetailGroup.AddWidgetRow()
	           .NameContent()[
			SNew(STextBlock).Text(LOCTEXT("LocationInterpSpeedLabel", "Camera Location Interp Speed")).Font(IDetailLayoutBuilder::GetDetailFont())]
		.ValueContent()[
			SNew(SNumericEntryBox<float>)
			.Value(this, &FTPP_Editor_SplineMetadataDetails::GetLocationInterpSpeed)
			.OnValueCommitted(this, &FTPP_Editor_SplineMetadataDetails::OnSetLocationInterpSpeed)
			.UndeterminedString(LOCTEXT("Multiple", "Multiple"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];

	// CAMERA ROTATION INTERP SPEED
	DetailGroup.AddWidgetRow()
	           .NameContent()[
			SNew(STextBlock).Text(LOCTEXT("RotationInterpSpeedLabel", "Camera Rotation Interp Speed")).Font(IDetailLayoutBuilder::GetDetailFont())]
		.ValueContent()[
			SNew(SNumericEntryBox<float>)
			.Value(this, &FTPP_Editor_SplineMetadataDetails::GetRotationInterpSpeed)
			.OnValueCommitted(this, &FTPP_Editor_SplineMetadataDetails::OnSetRotationInterpSpeed)
			.UndeterminedString(LOCTEXT("Multiple", "Multiple"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];

	// BIAS ACTOR VALUE
	DetailGroup.AddWidgetRow()
	           .NameContent()[SNew(STextBlock).Text(LOCTEXT("BiasActorLabel", "Bias Actor")).Font(IDetailLayoutBuilder::GetDetailFont())]
		.ValueContent()
		[
			SNew(SObjectPropertyEntryBox)
			.AllowedClass(AActor::StaticClass())
			.AllowClear(true)
			.ObjectPath(this, &FTPP_Editor_SplineMetadataDetails::GetBiasActorPath)
			.OnObjectChanged(this, &FTPP_Editor_SplineMetadataDetails::OnBiasActorChanged)
			.OnShouldFilterActor(this, &FTPP_Editor_SplineMetadataDetails::OnShouldFilterActor)
		];
}

FString FTPP_Editor_SplineMetadataDetails::GetBiasActorPath() const
{
	if (BiasActorValue.IsSet() && BiasActorValue.GetValue() != nullptr) { return BiasActorValue.GetValue()->GetPathName(); }
	return FString();
}

void FTPP_Editor_SplineMetadataDetails::OnSetBias(float NewValue, ETextCommit::Type CommitInfo)
{
	if (UTPP_CameraSplineMetadata* Metadata = GetMetadata())
	{
		const FScopedTransaction Transaction(LOCTEXT("SetBias", "Set Camera Bias"));
		Metadata->Modify();

		for (int32 Index : SelectedKeys) { if (Metadata->CameraBias.Points.IsValidIndex(Index)) { Metadata->CameraBias.Points[Index].OutVal = NewValue; } }
		OnSetValues();
	}
}

void FTPP_Editor_SplineMetadataDetails::OnSetPitch(float NewValue, ETextCommit::Type CommitInfo)
{
	if (UTPP_CameraSplineMetadata* Metadata = GetMetadata())
	{
		const FScopedTransaction Transaction(LOCTEXT("SetPitch", "Set Camera Pitch"));
		Metadata->Modify();

		for (int32 Index : SelectedKeys)
		{
			if (Metadata->CameraRotationPitch.Points.IsValidIndex(Index)) { Metadata->CameraRotationPitch.Points[Index].OutVal = NewValue; }
		}
		OnSetValues();
	}
}

void FTPP_Editor_SplineMetadataDetails::OnSetYaw(float NewValue, ETextCommit::Type CommitInfo)
{
	if (UTPP_CameraSplineMetadata* Metadata = GetMetadata())
	{
		const FScopedTransaction Transaction(LOCTEXT("SetYaw", "Set Camera Yaw"));
		Metadata->Modify();

		for (int32 Index : SelectedKeys)
		{
			if (Metadata->CameraRotationYaw.Points.IsValidIndex(Index)) { Metadata->CameraRotationYaw.Points[Index].OutVal = NewValue; }
		}
		OnSetValues();
	}
}

void FTPP_Editor_SplineMetadataDetails::OnSetRoll(float NewValue, ETextCommit::Type CommitInfo)
{
	if (UTPP_CameraSplineMetadata* Metadata = GetMetadata())
	{
		const FScopedTransaction Transaction(LOCTEXT("SetRoll", "Set Camera Roll"));
		Metadata->Modify();

		for (int32 Index : SelectedKeys)
		{
			if (Metadata->CameraRotationRoll.Points.IsValidIndex(Index)) { Metadata->CameraRotationRoll.Points[Index].OutVal = NewValue; }
		}
		OnSetValues();
	}
}

void FTPP_Editor_SplineMetadataDetails::OnSetSpringArmLenght(float NewValue, ETextCommit::Type CommitInfo)
{
	if (UTPP_CameraSplineMetadata* Metadata = GetMetadata())
	{
		const FScopedTransaction Transaction(LOCTEXT("SetArmLenght", "Set Arm Lenght"));
		Metadata->Modify();

		for (int32 Index : SelectedKeys)
		{
			if (Metadata->CameraSpringArmLength.Points.IsValidIndex(Index)) { Metadata->CameraSpringArmLength.Points[Index].OutVal = NewValue; }
		}
		OnSetValues();
	}
}

void FTPP_Editor_SplineMetadataDetails::OnSetYOffset(float NewValue, ETextCommit::Type CommitInfo)
{
	if (UTPP_CameraSplineMetadata* Metadata = GetMetadata())
	{
		const FScopedTransaction Transaction(LOCTEXT("SetYOffset", "Set Y Offset"));
		Metadata->Modify();

		for (int32 Index : SelectedKeys)
		{
			if (Metadata->CameraYOffset.Points.IsValidIndex(Index)) { Metadata->CameraYOffset.Points[Index].OutVal = NewValue; }
		}
		OnSetValues();
	}
}

void FTPP_Editor_SplineMetadataDetails::OnSetZOffset(float NewValue, ETextCommit::Type CommitInfo)
{
	if (UTPP_CameraSplineMetadata* Metadata = GetMetadata())
	{
		const FScopedTransaction Transaction(LOCTEXT("SetZOffset", "Set Z Offset"));
		Metadata->Modify();

		for (int32 Index : SelectedKeys)
		{
			if (Metadata->CameraZOffset.Points.IsValidIndex(Index)) { Metadata->CameraZOffset.Points[Index].OutVal = NewValue; }
		}
		OnSetValues();
	}
}

void FTPP_Editor_SplineMetadataDetails::OnSetBiasInterpSpeed(float NewValue, ETextCommit::Type CommitInfo)
{
	if (UTPP_CameraSplineMetadata* Metadata = GetMetadata())
	{
		const FScopedTransaction Transaction(LOCTEXT("SetBiasInterpSpeed", "Set Bias Interp Speed"));
		Metadata->Modify();

		for (int32 Index : SelectedKeys)
		{
			if (Metadata->CameraBiasInterpSpeed.Points.IsValidIndex(Index)) { Metadata->CameraBiasInterpSpeed.Points[Index].OutVal = NewValue; }
		}
		OnSetValues();
	}
}

void FTPP_Editor_SplineMetadataDetails::OnSetLocationInterpSpeed(float NewValue, ETextCommit::Type CommitInfo)
{
	if (UTPP_CameraSplineMetadata* Metadata = GetMetadata())
	{
		const FScopedTransaction Transaction(LOCTEXT("SetLocationInterpSpeed", "Set Location Interp Speed"));
		Metadata->Modify();

		for (int32 Index : SelectedKeys)
		{
			if (Metadata->CameraLocationInterpSpeed.Points.IsValidIndex(Index)) { Metadata->CameraLocationInterpSpeed.Points[Index].OutVal = NewValue; }
		}
		OnSetValues();
	}
}

void FTPP_Editor_SplineMetadataDetails::OnSetBiasActor(const FAssetData& AssetData)
{
	AActor* NewActor = Cast<AActor>(AssetData.GetAsset());

	if (UTPP_CameraSplineMetadata* Metadata = GetMetadata())
	{
		const FScopedTransaction Transaction(LOCTEXT("SetBiasActor", "Set Bias Actor"));
		Metadata->Modify();

		for (int32 Index : SelectedKeys)
		{
			if (Metadata->CameraBiasActor.IsValidIndex(Index))
			{
				UE_LOG(LogTemp, Display, TEXT("Set Actor"));
				Metadata->CameraBiasActor[Index] = NewActor;
			}
		}

		OnSetValues();
	}
}

void FTPP_Editor_SplineMetadataDetails::OnSetRotationInterpSpeed(float NewValue, ETextCommit::Type CommitInfo)
{
	if (UTPP_CameraSplineMetadata* Metadata = GetMetadata())
	{
		const FScopedTransaction Transaction(LOCTEXT("SetRotationInterpSpeed", "Set Rotation Interp Speed"));
		Metadata->Modify();

		for (int32 Index : SelectedKeys)
		{
			if (Metadata->CameraRotationInterpSpeed.Points.IsValidIndex(Index)) { Metadata->CameraRotationInterpSpeed.Points[Index].OutVal = NewValue; }
		}
		OnSetValues();
	}
}

void FTPP_Editor_SplineMetadataDetails::OnBiasActorChanged(const FAssetData& AssetData)
{
	UObject* Object = AssetData.GetAsset();
	if (Object && Object->IsA(AActor::StaticClass())) { OnSetBiasActor(Cast<AActor>(Object)); }
	else { OnSetBiasActor(nullptr); }
}

void FTPP_Editor_SplineMetadataDetails::OnSetValues()
{
	if (USplineComponent* SC = SplineComp.Get())
	{
		if (UTPP_CameraSplineMetadata* Metadata = GetMetadata())
		{
			Metadata->Modify();
			Metadata->PostEditChange();
		}

		SC->Modify();
		SC->UpdateSpline();
		SC->bSplineHasBeenEdited = true;

		Update(SC, SelectedKeys);
		GEditor->RedrawLevelEditingViewports(true);
	}
}

UTPP_CameraSplineMetadata* FTPP_Editor_SplineMetadataDetails::GetMetadata() const
{
	return SplineComp.IsValid() ? Cast<UTPP_CameraSplineMetadata>(SplineComp->GetSplinePointsMetadata()) : nullptr;
}

#undef LOCTEXT_NAMESPACE
