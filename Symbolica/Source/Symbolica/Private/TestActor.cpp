// Fill out your copyright notice in the Description page of Project Settings.


#include "TestActor.h"

#include "ShaderPrintParameters.h"
#include "Kismet/KismetStringLibrary.h"

// Sets default values
ATestActor::ATestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestActor::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 5., FColor::Yellow,
		LexToString(this->ModelData.Get() == nullptr));
	this->Runtime = UE::NNE::GetRuntime<INNERuntimeCPU>(FString("NNERuntimeORTCpu"));
	GEngine->AddOnScreenDebugMessage(-1, 5., FColor::Yellow, FString::FromInt((int)this->Runtime->CanCreateModelCPU(this->ModelData)));

	TSharedPtr<UE::NNE::IModelCPU> Model = this->Runtime->CreateModelCPU(this->ModelData);
	GEngine->AddOnScreenDebugMessage(-1, 5., FColor::Cyan, FString::FromInt(Model.IsValid()));
	this->ModelInstance = Model->CreateModelInstanceCPU();

	if (!this->ModelInstance) {
		GEngine->AddOnScreenDebugMessage(-1, 1., FColor::Red, "WTF");
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 1., FColor::Green, "Works!");
	
	TConstArrayView<UE::NNE::FTensorDesc> InputTensorDescs = ModelInstance->GetInputTensorDescs();
	checkf(InputTensorDescs.Num() == 1, TEXT("The current example supports only models with a single input tensor"));
	UE::NNE::FSymbolicTensorShape SymbolicInputTensorShape = InputTensorDescs[0].GetShape();
	checkf(SymbolicInputTensorShape.IsConcrete(), TEXT("The current example supports only models without variable input tensor dimensions"));
	TArray InputTensorShapes = { UE::NNE::FTensorShape::MakeFromSymbolic(SymbolicInputTensorShape) };

	ModelInstance->SetInputTensorShapes(InputTensorShapes);

	TConstArrayView<UE::NNE::FTensorDesc> OutputTensorDescs = ModelInstance->GetOutputTensorDescs();
	checkf(OutputTensorDescs.Num() == 1, TEXT("The current example supports only models with a single output tensor"));
	UE::NNE::FSymbolicTensorShape SymbolicOutputTensorShape = OutputTensorDescs[0].GetShape();
	checkf(SymbolicOutputTensorShape.IsConcrete(), TEXT("The current example supports only models without variable output tensor dimensions"));
	TArray OutputTensorShapes = { UE::NNE::FTensorShape::MakeFromSymbolic(SymbolicOutputTensorShape) };

	
	this->InputData.SetNumZeroed(InputTensorShapes[0].Volume());
	this->InputBindings.SetNumZeroed(1);
	this->InputBindings[0].SizeInBytes = InputData.Num() * sizeof(float);
	this->InputBindings[0].Data = InputData.GetData();

	this->OutputData.SetNumZeroed(OutputTensorShapes[0].Volume());
	this->OutputBindings.SetNumZeroed(1);
	this->OutputBindings[0].Data = OutputData.GetData();
	this->OutputBindings[0].SizeInBytes = OutputData.Num() * sizeof(float);
}

// Called every frame
void ATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
// 	UE_LOG(LogActor, Log, TEXT("TestActor::Tick"));
}

void ATestActor::Run() {
	this->InputData[0] = this->Input;
	if (ModelInstance->RunSync(InputBindings, OutputBindings) != UE::NNE::EResultStatus::Ok) {
		UE_LOG(LogTemp, Error, TEXT("Failed to run the model"));
	}
	GEngine->AddOnScreenDebugMessage(-1, 5., FColor::White, FString::SanitizeFloat(this->OutputData[0]));
}

