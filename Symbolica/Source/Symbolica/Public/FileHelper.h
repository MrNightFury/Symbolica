// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FileHelper.generated.h"

/**
 * 
 */
UCLASS()
class SYMBOLICA_API UFileHelper : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
	
	static IPlatformFile& FileManager;
	static FString BaseFilePath;

public:
	UFUNCTION(BlueprintCallable, Category = "FileHelper")
	static void EnsureBasePath();

	UFUNCTION(BlueprintCallable, Category = "FileHelper")
	static void WriteToFile(FString Filename, FString Content);
	
};
