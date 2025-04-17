// Fill out your copyright notice in the Description page of Project Settings.


#include "Symbolica/Public/Utils.h"

#include "FileHelper.h"
#include "MatrixTypes.h"
#include "Kismet/KismetSystemLibrary.h"

int UUtils::MaxArg(TArray<float> Array) {
	int MaxIndex = 0;
	for (int i = 0; i < Array.Num(); i++) {
		if (Array[MaxIndex] < Array[i]) {
			MaxIndex = i;
		}
	}
	return MaxIndex;
}

TArray<float> UUtils::Flatten(TArray<FVector> Array) {
	TArray<float> FloatArray;
	FloatArray.Reserve(Array.Num() * 3);
	for (const FVector& Vec : Array) {
		FloatArray.Add(Vec.X); FloatArray.Add(Vec.Y); FloatArray.Add(Vec.Z);
	}
	return FloatArray;
}

int Counter = 0;
TArray<float> UUtils::Flatten2D(TArray<FVector2D> Array) {
	TArray<float> FloatArray;
	FloatArray.Reserve(Array.Num() * 2);
	for (const FVector2D& Vec : Array) {
		FloatArray.Add(Vec.X); FloatArray.Add(Vec.Y);
	}
	// UFileHelper::WriteToFile("Test" + FString::FromInt(Counter++), FString::JoinBy(FloatArray, TEXT(", "), [](float Value) { return FString::SanitizeFloat(Value); }));
	return FloatArray;
}


FVector UUtils::GetCentroid(TArray<FVector> Points) {
	FVector Centroid = FVector::ZeroVector;
	for (const FVector& Vec : Points) {
		Centroid += Vec;
	}
	return Centroid / Points.Num();
}


FVector UUtils::GetBestFitPlane_old(TArray<FVector> Points, double& D, FVector& Centroid) {
	Centroid = FVector::ZeroVector;
	if (Points.Num() == 0) {
		D = 0;
		return FVector::ZeroVector;
	}
	
	for (const FVector& Point: Points) {
		Centroid += Point;
	}
	Centroid /= Points.Num();

	TArray<FVector> CenteredPoints;
	CenteredPoints.Reserve(Points.Num());
	for (const FVector& Point: Points) {
		CenteredPoints.Add(Point - Centroid);
	}
	
	FMatrix Cov = FMatrix::Identity;
	
	for (const FVector& P : CenteredPoints) {
		Cov.M[0][0] += P.X * P.X;
		Cov.M[0][1] += P.X * P.Y;
		Cov.M[0][2] += P.X * P.Z;

		Cov.M[1][0] += P.Y * P.X;
		Cov.M[1][1] += P.Y * P.Y;
		Cov.M[1][2] += P.Y * P.Z;

		Cov.M[2][0] += P.Z * P.X;
		Cov.M[2][1] += P.Z * P.Y;
		Cov.M[2][2] += P.Z * P.Z;
	}

	FVector Normal;
	{
		FVector Row1 = FVector(Cov.M[0][0], Cov.M[0][1], Cov.M[0][2]);
		FVector Row2 = FVector(Cov.M[1][0], Cov.M[1][1], Cov.M[1][2]);
		FVector Row3 = FVector(Cov.M[2][0], Cov.M[2][1], Cov.M[2][2]);
		
		FVector U = Row1 ^ Row2;
		FVector V = Row1 ^ Row3;

		Normal = (U.SizeSquared() < V.SizeSquared()) ? U : V;
		Normal.Normalize();
	}

	D = -FVector::DotProduct(Normal, Centroid);

	return Normal;
}



TArray<FVector> UUtils::SplineToPointsArray(USplineComponent* Spline) {
	TArray<FVector> Points;
	int PointsNum = Spline->GetNumberOfSplinePoints();
	for (int i = 0; i < PointsNum; i++) {
		Points.Add(Spline->GetSplinePointAt(i, ESplineCoordinateSpace::Local).Position);
	}
	return Points;
}

double UUtils::CalculatePlaneFitMSE(FVector Normal, double D, TArray<FVector> Points, double& MaxError) {
	double SumSquaredError = 0;
	MaxError = 0;

	if (Normal.Size() == 0) {
		return DBL_MAX;
	}

	for (const FVector& Point : Points) {
		double Distance = FMath::Abs((FVector::DotProduct(Normal, Point) + D) / Normal.Size());

		SumSquaredError += Distance * Distance;

		if (Distance > MaxError) {
			MaxError = Distance;
		}
	}

	return SumSquaredError / Points.Num();
}


int UUtils::GetMaxInt() {
	return MAX_int32;
}

TArray<FHitResult> UUtils::TraceCone(const UObject* World, FVector Origin, FVector Direction, float Angle, float Length,
									 int NumSteps, TArray<TEnumAsByte<EObjectTypeQuery>> TraceChannel) {
	TArray<FHitResult> OutHits;
	if (!World) return OutHits;

	for (int i = 0; i < NumSteps; i++) {
		const float Distance = (i / static_cast<float>(NumSteps)) * Length;
		const float CurrentRadius = FMath::Tan(FMath::DegreesToRadians(Angle)) * Distance;
        
		const FVector TraceLocation = Origin + (Direction * Distance);
		TArray<FHitResult> SphereHits;

		UKismetSystemLibrary::SphereTraceMultiForObjects(World, 
			TraceLocation, TraceLocation, CurrentRadius, 
			TraceChannel, false, TArray<AActor*>(), 
			EDrawDebugTrace::None, SphereHits, true);

		OutHits.Append(SphereHits);
	}
	return OutHits;
}


float UUtils::GetAverageCoord(const FVector& Vector) {
	return (Vector.X + Vector.Y + Vector.Z) / 3;
}