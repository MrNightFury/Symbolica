// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataPreprocessing.generated.h"

/**
 * 
 */
UCLASS()
class SYMBOLICA_API UDataPreprocessing : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static TArray<FVector> GetEvenlySpacedPoints(USplineComponent* Spline, int PointsNum);

	UFUNCTION(BlueprintCallable, meta=(ReturnDisplayName="Normal"))
	static FVector GetBestFitPlane(TArray<FVector> Points, double& D, FVector& Centroid);

	UFUNCTION(BlueprintCallable)
	static TArray<FVector2D> ProjectPointsToPlane(FVector Normal, double D, TArray<FVector> Points, FVector Right,
											TArray<FVector>& ProjectedPoints, FVector& PlaneOrigin, FVector Centroid);
};
