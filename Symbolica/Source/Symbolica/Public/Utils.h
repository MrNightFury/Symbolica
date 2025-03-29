// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utils.generated.h"

/**
 * 
 */
UCLASS()
class SYMBOLICA_API UUtils : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	static int MaxArg(TArray<float> Array);

	UFUNCTION(BlueprintPure, Category = "Utils")
	static TArray<float> Flatten(TArray<FVector> Array);
	
	UFUNCTION(BlueprintPure, Category = "Utils")
	static TArray<float> Flatten2D(TArray<FVector2D> Array);

	UFUNCTION(BlueprintCallable, meta=(ReturnDisplayName="Normal"))
	static FVector GetBestFitPlane(TArray<FVector> Points, double& D, FVector& Centroid);

	UFUNCTION(BlueprintCallable, meta=(ReturnDisplayName="Normal"))
	static FVector GetBestFitPlaneMinSquares(TArray<FVector> Points, double& D, FVector& Centroid);

	UFUNCTION(BlueprintCallable)
	static FVector GetCentroid(TArray<FVector> Points);

	UFUNCTION(BlueprintCallable)
	static TArray<FVector> SplineToPointsArray(USplineComponent* Spline);

	UFUNCTION(BlueprintCallable)
	static double CalculatePlaneFitMSE(FVector Normal, double D, TArray<FVector> Points, double& MaxError);

	UFUNCTION(BlueprintCallable)
	static TArray<FVector2D> ProjectPointsToPlane(FVector Normal, double D, TArray<FVector> Points, FVector Right, TArray<FVector>& ProjectedPoints, FVector& PlaneOrigin, FVector Centroid);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(CompactNodeTitle="Max int"))
	static int GetMaxInt();
};
