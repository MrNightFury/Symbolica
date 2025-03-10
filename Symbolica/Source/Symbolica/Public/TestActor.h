// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NNE.h"
#include "NNERuntimeCPU.h"
#include "NNEModelData.h"

#include "TestActor.generated.h"

UCLASS()
class SYMBOLICA_API ATestActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Input = 0;
	
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<UNNEModelData> ModelData;
	TWeakInterfacePtr<INNERuntimeCPU> Runtime;
	TSharedPtr<UE::NNE::IModelInstanceCPU> ModelInstance;

private:
	TArray<UE::NNE::FTensorBindingCPU> InputBindings;
	TArray<UE::NNE::FTensorBindingCPU> OutputBindings;
	TArray<float> InputData;
	TArray<float> OutputData;
	
public:	
	// Sets default values for this actor's properties
	ATestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor, BlueprintCallable)
	void Run();
};
