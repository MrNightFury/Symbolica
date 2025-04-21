// Fill out your copyright notice in the Description page of Project Settings.


#include "GTPawn.h"


// Sets default values
AGTPawn::AGTPawn() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGTPawn::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void AGTPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AGTPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

