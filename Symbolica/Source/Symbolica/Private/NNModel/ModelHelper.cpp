#include "NNModel/ModelHelper.h"

#include <string>

#include "Misc/RuntimeErrors.h"

template class TModelHelper<float, float>;

template<typename InputType, typename OutputType>
TModelHelper<InputType, OutputType>::TModelHelper(TObjectPtr<UNNEModelData> ModelData) {
	this->ModelData = ModelData;
	// try {
		this->Runtime = UE::NNE::GetRuntime<INNERuntimeCPU>(FString("NNERuntimeORTCpu"));
        TSharedPtr<UE::NNE::IModelCPU> const Model = this->Runtime->CreateModelCPU(this->ModelData);
        this->ModelInstance = Model->CreateModelInstanceCPU();

		TArray<UE::NNE::FTensorShape> InputShape = this->GetTensorShape(this->ModelInstance->GetInputTensorDescs());
		TArray<UE::NNE::FTensorShape> OutputShape = this->GetTensorShape(this->ModelInstance->GetOutputTensorDescs());

		ModelInstance->SetInputTensorShapes(InputShape);
		
		this->InitBinding(this->InputData, this->InputBindings, InputShape);
		this->InitBinding(this->OutputData, this->OutputBindings, OutputShape);
	// } catch (...) {
	// 	UE_LOG(LogTemp, Error, TEXT("Model helper constructor failed!"));
	// }
}

template <typename InputType, typename OutputType>
TArray<OutputType> TModelHelper<InputType, OutputType>::RunModelSync(TArray<InputType> Input) {
	if (this->InputData.Num()
		!= Input.Num()) {
		throw std::runtime_error(std::string("Input data size mismatch!\n"
			+ std::to_string(this->InputData.Num()) + " != " + std::to_string(Input.Num())));
	}
	this->InputData = Input;
	UE::NNE::EResultStatus const Status = ModelInstance->RunSync(InputBindings, OutputBindings);
	if (Status != UE::NNE::EResultStatus::Ok) {
		throw std::runtime_error(std::string("Model helper sync failed! Code: ")
			+ std::to_string(static_cast<uint8>(Status)));
	}
	return this->OutputData;
}


template <typename InputType, typename OutputType>
bool TModelHelper<InputType, OutputType>::IsValid() const {
	return this->ModelInstance.IsValid();
}

template <typename InputType, typename OutputType>
void TModelHelper<InputType, OutputType>::PrintDebugInfo() {
	UE_LOG(LogTemp, Log, TEXT("MODEL DEBUG INFO:"));
	UE_LOG(LogTemp, Log, TEXT("%s"), IsValid() ? TEXT("Model valid") : TEXT("Model invalid"));
	UE_LOG(LogTemp, Log, TEXT("Input type size: %llu, count: %d, array size got: %d, array size calculated: %d"),
		sizeof(InputType), this->InputData.Num(), this->InputData.NumBytes(), this->InputData.Num() * sizeof(InputType));
	UE_LOG(LogTemp, Log, TEXT("Input binding count: %d, array size got: %llu"),
		this->InputBindings.Num(), this->InputBindings.NumBytes());
	UE_LOG(LogTemp, Log, TEXT("Input binding[0] size: %d"),
		this->InputBindings[0].SizeInBytes);
	UE_LOG(LogTemp, Log, TEXT("--------"));
	UE_LOG(LogTemp, Log, TEXT("Output type size: %llu, count: %d, array size got: %d, array size calculated: %d"),
		sizeof(OutputType), this->OutputData.Num(), this->OutputData.NumBytes(), this->OutputData.Num() * sizeof(OutputType));
	UE_LOG(LogTemp, Log, TEXT("Output binding count: %d, array size got: %llu"),
		this->OutputBindings.Num(), this->OutputBindings.NumBytes());
	UE_LOG(LogTemp, Log, TEXT("Output binding[0] size: %d"),
		this->OutputBindings[0].SizeInBytes);
}


template <typename InputType, typename OutputType>
TArray<UE::NNE::FTensorShape> TModelHelper<InputType, OutputType>::GetTensorShape(TArrayView<const UE::NNE::FTensorDesc> TensorDesc) {
	checkf(TensorDesc.Num() == 1, TEXT("The current example supports only models with a single input tensor"));
	const UE::NNE::FSymbolicTensorShape SymbolicInputTensorShape = TensorDesc[0].GetShape();
	checkf(SymbolicInputTensorShape.IsConcrete(), TEXT("The current example supports only models without variable input tensor dimensions"));
	return { UE::NNE::FTensorShape::MakeFromSymbolic(SymbolicInputTensorShape) };
}

template <typename InputType, typename OutputType>
void TModelHelper<InputType, OutputType>::InitBinding(TArray<InputType>& Array,
													  TArray<UE::NNE::FTensorBindingCPU>& Binding,
													  TArray<UE::NNE::FTensorShape>& TensorShape) {
	Array.SetNumZeroed(TensorShape[0].Volume());
	Binding.SetNumZeroed(1);
	Binding[0].SizeInBytes = Array.Num() * sizeof(InputType);
	Binding[0].Data = Array.GetData();
}
