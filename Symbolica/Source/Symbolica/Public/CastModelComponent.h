// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModelHelper.h"
#include "NNEModelData.h"
#include "UObject/Object.h"
#include "CastModelComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class SYMBOLICA_API UCastModelComponent : public UActorComponent {
	GENERATED_BODY()
	TUniquePtr<TModelHelper<float, float>> ModelHelper;
	
protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UNNEModelData> ModelData;

	UCastModelComponent();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsModelReady() const;
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	void PrintDebugInfo();
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "NNModel", meta = (ExpandBoolAsExecs="bSuccess"))
	int32 GetDrawnSymbol(TArray<float> InputData, bool& bSuccess, FString& Error) const;
};