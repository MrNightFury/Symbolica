// Fill out your copyright notice in the Description page of Project Settings.


#include "FileHelper.h"



IPlatformFile& UFileHelper::FileManager = FPlatformFileManager::Get().GetPlatformFile();
FString UFileHelper::BaseFilePath = TEXT("");


void UFileHelper::EnsureBasePath() {
	auto const SavedPath = FPaths::ProjectSavedDir() + TEXT("/Files");
		
	FileManager = FPlatformFileManager::Get().GetPlatformFile();
	if (!FileManager.DirectoryExists(*SavedPath)) {
		FileManager.CreateDirectory(*SavedPath);
	}
	BaseFilePath = SavedPath;
}


void UFileHelper::WriteToFile(const FString Filename, const FString Content) {
	FFileHelper::SaveStringToFile(Content, *(BaseFilePath + "/" + Filename));
}