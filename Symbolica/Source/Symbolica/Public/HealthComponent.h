// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamageGot, float, Damage, FGameplayTag, DamageType, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthUpdated, float, CurrentHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnZeroHealth);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SYMBOLICA_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Debug = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CurrentHealth = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AutoDestroyOnZeroHealth = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool StartAtFullHealth = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsAlive = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, float> DamageMultipliers = TMap<FGameplayTag, float>();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDamageGot OnDamageGot;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthUpdated OnHealthUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnZeroHealth OnZeroHealth;
	
	UHealthComponent();
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Damage(float Damage, FGameplayTag DamageType, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetDamageMultiplier(FGameplayTag DamageType) const;

	UFUNCTION()
	void DebugPrint(float CurrentHealth);

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
