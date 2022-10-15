// Fill out your copyright notice in the Description page of Project Settings.


#include "MP_HealthComponent.h"

// Sets default values for this component's properties
UMP_HealthComponent::UMP_HealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	DefaultHealth = 100;
	Health = DefaultHealth;
}


// Called when the game starts
void UMP_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UMP_HealthComponent::TakeDamage);
	}
	
}

void UMP_HealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// lol miss me with that 0 damage sht
	if (Damage <= 0) return;

	// Subtract the damage from our health, but make sure our health doesn't go below zero
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
}
