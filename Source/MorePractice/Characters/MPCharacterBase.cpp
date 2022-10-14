// Fill out your copyright notice in the Description page of Project Settings.

#include "MPCharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMPCharacterBase::AMPCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");

	StaticMesh->SetupAttachment(RootComponent);
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);

	BaseTurnRate = 45.0f;
	BaseLookUpAtRate = 45.0f;
	TraceDistance = 2000.0f;

}

void AMPCharacterBase::MoveForward(float Amount)
{
	if (Controller && Amount != 0.0f) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Amount);
	}
}

void AMPCharacterBase::MoveRight(float Amount)
{
	if (Controller && Amount != 0.0f) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Amount);
	}
}

void AMPCharacterBase::TurnAtRate(float Amount)
{
	AddControllerYawInput(Amount * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMPCharacterBase::LookUpAtRate(float Amount)
{
	AddControllerPitchInput(Amount * BaseLookUpAtRate * GetWorld()->GetDeltaSeconds());
}

void AMPCharacterBase::InteractPress()
{
	TraceForward();
}

void AMPCharacterBase::TraceForward_Implementation()
{
	FVector Loc;
	FRotator Rot;
	FHitResult Hit;

	GetController()->GetPlayerViewPoint(Loc, Rot);

	FVector Start = Loc;
	FVector End = Start + (Rot.Vector() * TraceDistance);

	FCollisionQueryParams TraceParams;

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	//Requires special #include "DrawDebugHelpers.h"
	DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 5.0f);

	if (bHit)
	{
		DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(10, 10, 10), FColor::Emerald, false, 5.0f);
	}
}

// Called to bind functionality to input
void AMPCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMPCharacterBase::InteractPress);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMPCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMPCharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMPCharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMPCharacterBase::LookUpAtRate);


}

