// Fill out your copyright notice in the Description page of Project Settings.


#include "Symbolica/Public/Utils.h"

#include "FileHelper.h"
#include "MatrixTypes.h"

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
	UFileHelper::WriteToFile("Test" + FString::FromInt(Counter++), FString::JoinBy(FloatArray, TEXT(", "), [](float Value) { return FString::SanitizeFloat(Value); }));
	return FloatArray;
}


FVector UUtils::GetCentroid(TArray<FVector> Points) {
	FVector Centroid = FVector::ZeroVector;
	for (const FVector& Vec : Points) {
		Centroid += Vec;
	}
	return Centroid / Points.Num();
}


FVector UUtils::GetBestFitPlane(TArray<FVector> Points, double& D, FVector& Centroid) {
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

FVector UUtils::GetBestFitPlaneMinSquares(TArray<FVector> Points, double& D, FVector& Centroid) {
	Centroid = FVector::ZeroVector;
	if (Points.Num() < 3) {
		D = 0;
		return FVector::ZeroVector;
	}
	
	double XX = 0, YY = 0, ZZ = 0, XY = 0, YZ = 0, XZ = 0;
	Centroid = GetCentroid(Points);
	for (const FVector& Point: Points) {
		FVector P = Point - Centroid;
		XX += P.X * P.X;
		YY += P.Y * P.Y;
		ZZ += P.Z * P.Z;
		XY += P.X * P.Y;
		YZ += P.Y * P.Z;
		XZ += P.Z * P.X;
	}

	UE::Geometry::FMatrix3d A = UE::Geometry::FMatrix3d(
		XX, XY, XZ,
		XY, YY, YZ,
		XZ, YZ, ZZ);

	UE::Geometry::FMatrix3d UR = UE::Geometry::FMatrix3d::Identity();

	for (int iter = 0; iter < 5; iter++) {
		double MM = abs(A(0, 1));
		int i = 0, j = 1;
		for (int n = 0; n <= 1; n++) {
			if (abs(A(n, 2)) > MM) {
				MM = abs(A(n, 2));
				i = n; j = 2;
			}
		}

		if (MM > 1e-5) {
			double angle = 0;
			if (abs(A(i, i) - A(j, j)) > 1e-10) {
				angle = .5 * atan(2 * A(i, j) / (A(i, i) - A(j, j)));
			} else {
				angle = PI / 2;
			}

			double U[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
			U[i][i] = cos(angle); U[i][j] = -sin(angle);
			U[j][i] = -U[i][j];   U[j][j] = U[i][i];
			UE::Geometry::FMatrix3d R = UE::Geometry::FMatrix3d(U[0][0], U[0][1], U[0][2],
																U[1][0], U[1][1], U[1][2],
																U[2][0], U[2][1], U[2][2]);
			UR = UR * R;
			A = A * R;
			R = R.Transpose();
			A = R * A;
		}
	}

	double MM = A(0, 0);
	int j = 0;
	for (int n = 1; n <= 2; n++) {
		if (abs(A(n, n)) < MM) {
			MM = abs(A(n, n));
			j = n;
		}
	}
	FVector Normal = FVector(UR(0, j), UR(1, j), UR(2, j));
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


TArray<FVector2D> UUtils::ProjectPointsToPlane(FVector Normal, double D, TArray<FVector> Points, FVector Right, TArray<FVector>& ProjectedPoints, FVector& PlaneOrigin, FVector Centroid = FVector::Zero()) {
	ProjectedPoints = TArray<FVector>();
	PlaneOrigin = FVector::Zero();
	TArray<FVector2D> ProjectedPoints2D;
	if (Points.Num() == 0) return ProjectedPoints2D;

	double CentroidDistance = FVector::DotProduct(Centroid, Normal) + D;
	PlaneOrigin = Centroid - Normal * CentroidDistance;
	
	
	FVector X_Axis, Y_Axis;
	{
		FVector Arbitrary = (FMath::Abs(Normal.X) > FMath::Abs(Normal.Z)) ? FVector(0, 0, 1) : FVector(1, 0, 0);
		FVector FirstAxis = FVector::CrossProduct(Arbitrary, Normal).GetSafeNormal();
		FVector SecondAxis = FVector::CrossProduct(Normal, FirstAxis).GetSafeNormal();

		// UE_LOG(LogTemp, Error, TEXT("%f, %f, %f"), FirstAxis.X, FirstAxis.Y, FirstAxis.Z);
		// UE_LOG(LogTemp, Error, TEXT("%f, %f, %f"), SecondAxis.X, SecondAxis.Y, SecondAxis.Z);

		double FirstAxisProjected = FVector::DotProduct(FirstAxis, FVector(0, 0, 1));
		double SecondAxisProjected = FVector::DotProduct(SecondAxis, FVector(0, 0, 1));
		
		if (abs(FirstAxisProjected) < abs(SecondAxisProjected)) {
			X_Axis = FirstAxis;
			Y_Axis = SecondAxis;
		} else {
			X_Axis = SecondAxis;
			Y_Axis = FirstAxis;
		}

		if (FVector::DotProduct(Y_Axis, FVector(0, 0, 1)) < 0) {
			Y_Axis = -Y_Axis;
		}
		if (FVector::DotProduct(X_Axis, Right) < 0) {
			X_Axis = -X_Axis;
		}
	}

	for (const FVector& Point : Points) {
		double Distance = FVector::DotProduct(Point, Normal) + D;
		FVector ProjectedPoint = Point - Normal * Distance;
		ProjectedPoints.Add(ProjectedPoint);

		double U = FVector::DotProduct(ProjectedPoint - PlaneOrigin, X_Axis);
		double V = FVector::DotProduct(ProjectedPoint - PlaneOrigin, Y_Axis);
		ProjectedPoints2D.Add(FVector2D(U, V));
	}

	return ProjectedPoints2D;
}

int UUtils::GetMaxInt() {
	return MAX_int32;
}
