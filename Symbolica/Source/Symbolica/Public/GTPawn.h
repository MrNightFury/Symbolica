// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Pawn.h"
#include "GTPawn.generated.h"

UCLASS()
class SYMBOLICA_API AGTPawn : public APawn, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	AGTPawn();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagContainer GameplayTags;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override {
		TagContainer = GameplayTags;
	}

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
