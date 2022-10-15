// Fill out your copyright notice in the Description page of Project Settings.


#include "MP_ForceActor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AMP_ForceActor::AMP_ForceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Cube");
	StaticMesh->SetupAttachment(RootComponent);


	ForceStrength = 10000.0f;

}

// Called when the game starts or when spawned
void AMP_ForceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMP_ForceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Up = StaticMesh->GetUpVector();
	StaticMesh->AddForce(Up * ForceStrength * StaticMesh->GetMass());
}

