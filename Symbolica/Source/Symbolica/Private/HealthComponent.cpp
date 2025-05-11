#include "HealthComponent.h"


UHealthComponent::UHealthComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}


void UHealthComponent::Damage(float Damage, FGameplayTag DamageType, AActor* DamageCauser) {
	this->OnDamageGot.Broadcast(Damage, DamageType, DamageCauser);
	if (!this->IsAlive) {
		return;
	}

	// Apply damage
	this->CurrentHealth -= Damage * this->GetDamageMultiplier(DamageType);

	// Check death
	if (this->CurrentHealth <= 0) {
		this->IsAlive = false;
		this->CurrentHealth = 0;
		this->OnZeroHealth.Broadcast();
		if (this->AutoDestroyOnZeroHealth) {
			this->GetOwner()->Destroy();
		}
	}
}

float UHealthComponent::GetDamageMultiplier(FGameplayTag DamageType) const {
	FGameplayTag Tag = DamageType;
	while (Tag != FGameplayTag::EmptyTag) {
		const float* Multiplier = this->DamageMultipliers.Find(Tag);
		if (Multiplier) {
			return *Multiplier;
		}
		Tag = Tag.RequestDirectParent();
	}
	return 1.0f;
}

void UHealthComponent::DebugPrint(float CurrentHealth) {
	UE_LOG(LogTemp, Log, TEXT("%s: %f/%f"), *this->GetOwner()->GetName(), this->CurrentHealth, this->MaxHealth)
}


void UHealthComponent::BeginPlay() {
	Super::BeginPlay();
	if (this->StartAtFullHealth) {
		this->CurrentHealth = this->MaxHealth;
		this->OnHealthUpdated.Broadcast(this->CurrentHealth);
	}
	if (this->Debug) {
		this->OnHealthUpdated.AddDynamic(this, &UHealthComponent::DebugPrint);
	}
}


void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

