#pragma once

#include "NNERuntimeCPU.h"
#include "NNEModelData.h"
#include "NNE.h"

template<typename InputType, typename OutputType>
class TModelHelper {
	TObjectPtr<UNNEModelData> ModelData;
	TWeakInterfacePtr<INNERuntimeCPU> Runtime;
	TSharedPtr<UE::NNE::IModelInstanceCPU> ModelInstance;

	TArray<UE::NNE::FTensorBindingCPU> InputBindings;
	TArray<UE::NNE::FTensorBindingCPU> OutputBindings;
	TArray<InputType> InputData;
	TArray<OutputType> OutputData;

	static TArray<UE::NNE::FTensorShape> GetTensorShape(TArrayView<const UE::NNE::FTensorDesc> TensorDesc);
	static void InitBinding(TArray<InputType>& Array, TArray<UE::NNE::FTensorBindingCPU>& Binding,
							TArray<UE::NNE::FTensorShape>& TensorShape);
	
public:
	TModelHelper(TObjectPtr<UNNEModelData> ModelData);
	TArray<OutputType> RunModelSync(TArray<InputType> Input);
	void PrintDebugInfo();
	bool IsValid() const;
};
