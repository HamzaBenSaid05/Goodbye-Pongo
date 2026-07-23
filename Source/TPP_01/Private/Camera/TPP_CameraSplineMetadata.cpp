#include "Camera/TPP_CameraSplineMetadata.h"

UTPP_CameraSplineMetadata::UTPP_CameraSplineMetadata(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {}

void UTPP_CameraSplineMetadata::InsertPoint(int32 Index, float t, bool bClosedLoop)
{
	check(Index >= 0);
	Modify();

	const int32 NumPoints = CameraBias.Points.Num();
	const float InputKey = static_cast<float>(Index);

	if (Index >= NumPoints) { AddPoint(InputKey); }
	else
	{
		int32 SourceIndex = FMath::Clamp(Index - 1, 0, NumPoints - 1);

		// Retrieve the value from the previous point
		float NewBias = CameraBias.Points[Index].OutVal;

		float NewPitch = CameraRotationPitch.Points[Index].OutVal;
		float NewYaw = CameraRotationYaw.Points[Index].OutVal;
		float NewRoll = CameraRotationRoll.Points[Index].OutVal;

		float NewArmLength = CameraSpringArmLength.Points[Index].OutVal;

		float NewYOffset = CameraYOffset.Points[Index].OutVal;
		float NewZOffset = CameraZOffset.Points[Index].OutVal;

		float NewBiasInterpSpeed = CameraBiasInterpSpeed.Points[Index].OutVal;
		float NewLocationInterpSpeed = CameraLocationInterpSpeed.Points[Index].OutVal;
		float NewRotationInterpSpeed = CameraRotationInterpSpeed.Points[Index].OutVal;

		CameraBias.Points.Insert(FInterpCurvePoint<float>(InputKey, NewBias), Index);

		CameraRotationPitch.Points.Insert(FInterpCurvePoint<float>(InputKey, NewPitch), Index);
		CameraRotationYaw.Points.Insert(FInterpCurvePoint<float>(InputKey, NewYaw), Index);
		CameraRotationRoll.Points.Insert(FInterpCurvePoint<float>(InputKey, NewRoll), Index);

		CameraSpringArmLength.Points.Insert(FInterpCurvePoint<float>(InputKey, NewArmLength), Index);

		CameraYOffset.Points.Insert(FInterpCurvePoint<float>(InputKey, NewYOffset), Index);
		CameraZOffset.Points.Insert(FInterpCurvePoint<float>(InputKey, NewZOffset), Index);

		CameraBiasInterpSpeed.Points.Insert(FInterpCurvePoint<float>(InputKey, NewBiasInterpSpeed), Index);
		CameraLocationInterpSpeed.Points.Insert(FInterpCurvePoint<float>(InputKey, NewLocationInterpSpeed), Index);
		CameraRotationInterpSpeed.Points.Insert(FInterpCurvePoint<float>(InputKey, NewRotationInterpSpeed), Index);

		CameraBiasActor.Insert(CameraBiasActor.IsValidIndex(SourceIndex) ? CameraBiasActor[SourceIndex] : nullptr, Index);

		// Index Correction (InVal) for all subsequent points
		for (int32 i = 0; i < CameraBias.Points.Num(); ++i)
		{
			float Key = static_cast<float>(i);
			CameraBias.Points[i].InVal = Key;

			CameraRotationPitch.Points[i].InVal = Key;
			CameraRotationYaw.Points[i].InVal = Key;
			CameraRotationRoll.Points[i].InVal = Key;

			CameraSpringArmLength.Points[i].InVal = Key;

			CameraYOffset.Points[i].InVal = Key;
			CameraZOffset.Points[i].InVal = Key;

			CameraBiasInterpSpeed.Points[i].InVal = Key;
			CameraLocationInterpSpeed.Points[i].InVal = Key;
			CameraRotationInterpSpeed.Points[i].InVal = Key;
		}
	}
}

void UTPP_CameraSplineMetadata::UpdatePoint(int32 Index, float t, bool bClosedLoop)
{
	const int32 NumPoints = CameraBias.Points.Num();
	if (Index >= 0 && Index < NumPoints)
	{
		Modify();
		// Synchronize the time key (InVal) for security
		float Key = static_cast<float>(Index);

		CameraBias.Points[Index].InVal = Key;

		CameraRotationPitch.Points[Index].InVal = Key;
		CameraRotationYaw.Points[Index].InVal = Key;
		CameraRotationRoll.Points[Index].InVal = Key;

		CameraSpringArmLength.Points[Index].InVal = Key;

		CameraYOffset.Points[Index].InVal = Key;
		CameraZOffset.Points[Index].InVal = Key;

		CameraBiasInterpSpeed.Points[Index].InVal = Key;
		CameraLocationInterpSpeed.Points[Index].InVal = Key;
		CameraRotationInterpSpeed.Points[Index].InVal = Key;
	}
}

void UTPP_CameraSplineMetadata::AddPoint(float InputKey)
{
	Modify();

	float NewBias = 0.0f;
	float NewPitch = 0.0f;
	float NewYaw = 0.0f;
	float NewRoll = 0.0f;
	float NewArmLength = 0.0f;
	float NewYOffset = 0.0f;
	float NewZOffset = 0.0f;
	float NewBiasInterpSpeed = 0.0f;
	float NewLocationInterpSpeed = 0.0f;
	float NewRotationInterpSpeed = 0.0f;
	AActor* NewBiasActor = nullptr;

	// Dry copy from last point
	int32 LastIndex = CameraBias.Points.Num() - 1;
	if (LastIndex >= 0 && CameraBiasActor.IsValidIndex(LastIndex))
	{
		NewBias = CameraBias.Points[LastIndex].OutVal;

		NewPitch = CameraRotationPitch.Points[LastIndex].OutVal;
		NewYaw = CameraRotationYaw.Points[LastIndex].OutVal;
		NewRoll = CameraRotationRoll.Points[LastIndex].OutVal;

		NewArmLength = CameraSpringArmLength.Points[LastIndex].OutVal;

		NewYOffset = CameraYOffset.Points[LastIndex].OutVal;
		NewZOffset = CameraZOffset.Points[LastIndex].OutVal;

		NewBiasInterpSpeed = CameraBiasInterpSpeed.Points[LastIndex].OutVal;
		NewLocationInterpSpeed = CameraLocationInterpSpeed.Points[LastIndex].OutVal;
		NewRotationInterpSpeed = CameraRotationInterpSpeed.Points[LastIndex].OutVal;

		NewBiasActor = CameraBiasActor[LastIndex];
	}

	float NewInputKey = static_cast<float>(CameraBias.Points.Num());

	CameraBias.Points.Emplace(NewInputKey, NewBias);

	CameraRotationPitch.Points.Emplace(NewInputKey, NewPitch);
	CameraRotationYaw.Points.Emplace(NewInputKey, NewYaw);
	CameraRotationRoll.Points.Emplace(NewInputKey, NewRoll);

	CameraSpringArmLength.Points.Emplace(NewInputKey, NewArmLength);

	CameraYOffset.Points.Emplace(NewInputKey, NewYOffset);
	CameraZOffset.Points.Emplace(NewInputKey, NewZOffset);

	CameraBiasInterpSpeed.Points.Emplace(NewInputKey, NewBiasInterpSpeed);
	CameraLocationInterpSpeed.Points.Emplace(NewInputKey, NewLocationInterpSpeed);
	CameraRotationInterpSpeed.Points.Emplace(NewInputKey, NewRotationInterpSpeed);

	CameraBiasActor.Emplace(NewBiasActor);
}

void UTPP_CameraSplineMetadata::RemovePoint(int32 Index)
{
	if (!CameraBias.Points.IsValidIndex(Index)) return;

	Modify();

	CameraBias.Points.RemoveAt(Index);

	CameraRotationPitch.Points.RemoveAt(Index);
	CameraRotationYaw.Points.RemoveAt(Index);
	CameraRotationRoll.Points.RemoveAt(Index);

	CameraSpringArmLength.Points.RemoveAt(Index);

	CameraYOffset.Points.RemoveAt(Index);
	CameraZOffset.Points.RemoveAt(Index);

	CameraBiasInterpSpeed.Points.RemoveAt(Index);
	CameraLocationInterpSpeed.Points.RemoveAt(Index);
	CameraRotationInterpSpeed.Points.RemoveAt(Index);

	CameraBiasActor.RemoveAt(Index);

	// Re-indexing to maintain the sequence
	for (int32 i = 0; i < CameraBias.Points.Num(); ++i)
	{
		float Key = static_cast<float>(i);
		CameraBias.Points[i].InVal = Key;

		CameraRotationPitch.Points[i].InVal = Key;
		CameraRotationYaw.Points[i].InVal = Key;
		CameraRotationRoll.Points[i].InVal = Key;

		CameraSpringArmLength.Points[i].InVal = Key;

		CameraYOffset.Points[i].InVal = Key;
		CameraZOffset.Points[i].InVal = Key;

		CameraBiasInterpSpeed.Points[i].InVal = Key;
		CameraLocationInterpSpeed.Points[i].InVal = Key;
		CameraRotationInterpSpeed.Points[i].InVal = Key;
	}
}

void UTPP_CameraSplineMetadata::DuplicatePoint(int32 Index)
{
	if (!CameraBias.Points.IsValidIndex(Index)) return;

	Modify();

	const FInterpCurvePoint<float> TempBias = CameraBias.Points[Index];

	const FInterpCurvePoint<float> TempPitch = CameraRotationPitch.Points[Index];
	const FInterpCurvePoint<float> TempYaw = CameraRotationYaw.Points[Index];
	const FInterpCurvePoint<float> TempRoll = CameraRotationRoll.Points[Index];

	const FInterpCurvePoint<float> TempArmLength = CameraSpringArmLength.Points[Index];

	const FInterpCurvePoint<float> TempYOffset = CameraYOffset.Points[Index];
	const FInterpCurvePoint<float> TempZOffset = CameraZOffset.Points[Index];

	const FInterpCurvePoint<float> TempBiasInterpSpeed = CameraBiasInterpSpeed.Points[Index];
	const FInterpCurvePoint<float> TempLocationInterpSpeed = CameraLocationInterpSpeed.Points[Index];
	const FInterpCurvePoint<float> TempRotationInterpSpeed = CameraRotationInterpSpeed.Points[Index];

	AActor* TempBiasActor = CameraBiasActor[Index];

	CameraBias.Points.Insert(TempBias, Index + 1);

	CameraRotationPitch.Points.Insert(TempPitch, Index + 1);
	CameraRotationYaw.Points.Insert(TempYaw, Index + 1);
	CameraRotationRoll.Points.Insert(TempRoll, Index + 1);

	CameraSpringArmLength.Points.Insert(TempArmLength, Index + 1);

	CameraYOffset.Points.Insert(TempYOffset, Index + 1);
	CameraZOffset.Points.Insert(TempZOffset, Index + 1);

	CameraBiasInterpSpeed.Points.Insert(TempBiasInterpSpeed, Index + 1);
	CameraLocationInterpSpeed.Points.Insert(TempLocationInterpSpeed, Index + 1);
	CameraRotationInterpSpeed.Points.Insert(TempRotationInterpSpeed, Index + 1);

	CameraBiasActor.Insert(TempBiasActor, Index + 1);

	for (int32 i = 0; i < CameraBias.Points.Num(); ++i)
	{
		float Key = static_cast<float>(i);
		CameraBias.Points[i].InVal = Key;

		CameraRotationPitch.Points[i].InVal = Key;
		CameraRotationYaw.Points[i].InVal = Key;
		CameraRotationRoll.Points[i].InVal = Key;

		CameraSpringArmLength.Points[i].InVal = Key;

		CameraYOffset.Points[i].InVal = Key;
		CameraZOffset.Points[i].InVal = Key;

		CameraBiasInterpSpeed.Points[i].InVal = Key;
		CameraLocationInterpSpeed.Points[i].InVal = Key;
		CameraRotationInterpSpeed.Points[i].InVal = Key;
	}
}

void UTPP_CameraSplineMetadata::CopyPoint(const USplineMetadata* FromSplineMetadata, int32 FromIndex, int32 ToIndex)
{
	if (const UTPP_CameraSplineMetadata* FromMetadata = Cast<UTPP_CameraSplineMetadata>(FromSplineMetadata))
	{
		if (FromMetadata->CameraBias.Points.IsValidIndex(FromIndex) && CameraBias.Points.IsValidIndex(ToIndex))
		{
			Modify();

			CameraBias.Points[ToIndex].OutVal = FromMetadata->CameraBias.Points[FromIndex].OutVal;

			CameraRotationPitch.Points[ToIndex].OutVal = FromMetadata->CameraRotationPitch.Points[FromIndex].OutVal;
			CameraRotationYaw.Points[ToIndex].OutVal = FromMetadata->CameraRotationYaw.Points[FromIndex].OutVal;
			CameraRotationRoll.Points[ToIndex].OutVal = FromMetadata->CameraRotationRoll.Points[FromIndex].OutVal;

			CameraSpringArmLength.Points[ToIndex].OutVal = FromMetadata->CameraSpringArmLength.Points[FromIndex].OutVal;

			CameraYOffset.Points[ToIndex].OutVal = FromMetadata->CameraYOffset.Points[FromIndex].OutVal;
			CameraZOffset.Points[ToIndex].OutVal = FromMetadata->CameraZOffset.Points[FromIndex].OutVal;

			CameraBiasInterpSpeed.Points[ToIndex].OutVal = FromMetadata->CameraBiasInterpSpeed.Points[FromIndex].OutVal;
			CameraLocationInterpSpeed.Points[ToIndex].OutVal = FromMetadata->CameraLocationInterpSpeed.Points[FromIndex].OutVal;
			CameraRotationInterpSpeed.Points[ToIndex].OutVal = FromMetadata->CameraRotationInterpSpeed.Points[FromIndex].OutVal;
			CameraBiasActor[ToIndex] = FromMetadata->CameraBiasActor[FromIndex];
		}
	}
}

void UTPP_CameraSplineMetadata::Reset(int32 NumPoints)
{
	Modify();

	CameraBias.Points.Reset(NumPoints);

	CameraRotationPitch.Points.Reset(NumPoints);
	CameraRotationYaw.Points.Reset(NumPoints);
	CameraRotationRoll.Points.Reset(NumPoints);

	CameraSpringArmLength.Points.Reset(NumPoints);

	CameraYOffset.Points.Reset(NumPoints);
	CameraZOffset.Points.Reset(NumPoints);

	CameraBiasInterpSpeed.Points.Reset(NumPoints);
	CameraLocationInterpSpeed.Points.Reset(NumPoints);
	CameraRotationInterpSpeed.Points.Reset(NumPoints);

	CameraBiasActor.Reset(NumPoints);
}

void UTPP_CameraSplineMetadata::Fixup(int32 NumPoints, USplineComponent* SplineComp)
{
	auto FixupCurve = [NumPoints](FInterpCurveFloat& Curve)
	{
		// Sync InVal with indexes
		for (int32 i = 0; i < Curve.Points.Num(); ++i) { Curve.Points[i].InVal = static_cast<float>(i); }
		// Add missing points by copying the last available value
		while (Curve.Points.Num() < NumPoints)
		{
			float NewVal = Curve.Points.Num() > 0 ? Curve.Points.Last().OutVal : 0.0f;
			float NewKey = static_cast<float>(Curve.Points.Num());
			Curve.Points.Add(FInterpCurvePoint<float>(NewKey, NewVal));
		}
		// Remove excess points
		if (Curve.Points.Num() > NumPoints) { Curve.Points.RemoveAt(NumPoints, Curve.Points.Num() - NumPoints); }
	};

	Modify();

	FixupCurve(CameraBias);

	FixupCurve(CameraRotationPitch);
	FixupCurve(CameraRotationYaw);
	FixupCurve(CameraRotationRoll);

	FixupCurve(CameraSpringArmLength);

	FixupCurve(CameraYOffset);
	FixupCurve(CameraZOffset);

	FixupCurve(CameraBiasInterpSpeed);
	FixupCurve(CameraLocationInterpSpeed);
	FixupCurve(CameraRotationInterpSpeed);

	if (CameraBiasActor.Num() < NumPoints)
	{
		AActor* Last = CameraBiasActor.Num() > 0 ? CameraBiasActor.Last() : nullptr;
		while (CameraBiasActor.Num() < NumPoints) { CameraBiasActor.Add(Last); }
	}
	else if (CameraBiasActor.Num() > NumPoints) { CameraBiasActor.SetNum(NumPoints); }
}
