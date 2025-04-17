// Fill out your copyright notice in the Description page of Project Settings.


#include "NNModel/DataPreprocessing.h"

#include "MatrixTypes.h"
#include "Symbolica/Public/Utils.h"

FVector UDataPreprocessing::GetBestFitPlane(TArray<FVector> Points, double& D, FVector& Centroid) {
	Centroid = FVector::ZeroVector;
	if (Points.Num() < 3) {
		D = 0;
		return FVector::ZeroVector;
	}
	
	double XX = 0, YY = 0, ZZ = 0, XY = 0, YZ = 0, XZ = 0;
	Centroid = UUtils::GetCentroid(Points);
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


TArray<FVector2D> UDataPreprocessing::ProjectPointsToPlane(FVector Normal, double D, TArray<FVector> Points, FVector Right, TArray<FVector>& ProjectedPoints, FVector& PlaneOrigin, FVector Centroid = FVector::Zero()) {
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


TArray<FVector> UDataPreprocessing::GetEvenlySpacedPoints(USplineComponent* Spline, const int PointsNum) {
	TArray<FVector> Points;
	if (PointsNum <= 0) {
		return Points;
	}
	const float Step = Spline->GetSplineLength() / (PointsNum - 1);
	for (int i = 0; i < PointsNum; i++) {
		Points.Add(Spline->GetLocationAtDistanceAlongSpline(Step * i, ESplineCoordinateSpace::Local));
	}
	return Points;
}