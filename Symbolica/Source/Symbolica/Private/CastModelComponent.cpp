// Fill out your copyright notice in the Description page of Project Settings.


#include "CastModelComponent.h"
#include "Symbolica/Public/Utils.h"

UCastModelComponent::UCastModelComponent() {
	
}

void UCastModelComponent::BeginPlay() {
	Super::BeginPlay();
	if (this->ModelData != nullptr) {
		this->ModelHelper = MakeUnique<TModelHelper<float, float>>(TModelHelper<float, float>(this->ModelData));
	} else {
		UE_LOG(LogTemp, Error, TEXT("Cast Model Component is not selected"));
	}
}

bool UCastModelComponent::IsModelReady() const {
	return this->ModelHelper.IsValid() && this->ModelHelper->IsValid();
}


int32 UCastModelComponent::GetDrawnSymbol(const TArray<FVector2D> InputData, bool& bSuccess, FString& Error) const {
	const TArray<float> FlattenedData = UUtils::Flatten2D(InputData);
	if (InputData.Num() <= 0) {
		bSuccess = false;
		Error = "Input data is empty";
		return -1;
	}
	
	// TArray<float> FloatArray = UUtils::Flatten(FlattenedData);
	TArray<float> Result;
	// try {
		Result = this->ModelHelper->RunModelSync(FlattenedData);
	// } catch (std::runtime_error e) {
	// 	bSuccess = false;
	// 	Error = e.what();
	// 	return -1;
	// }
	
	bSuccess = true;
	UE_LOG(LogTemp, Log, TEXT("%f"), Result[0]);
	return UUtils::MaxArg(Result);
}


void UCastModelComponent::PrintDebugInfo() {
	this->ModelHelper->PrintDebugInfo();
}

