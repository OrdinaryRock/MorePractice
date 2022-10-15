// Fill out your copyright notice in the Description page of Project Settings.

#include "MPCharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"
#include "../Interact/InteractInterface.h"

// Sets default values
AMPCharacterBase::AMPCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");

	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &AMPCharacterBase::OnOverlapBegin);
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);

	BaseTurnRate = 45.0f;
	BaseLookUpAtRate = 45.0f;
	TraceDistance = 2000.0f;

	ImpulseForce = 500.0f;
	
	bApplyRadialForce = true;
	RadialImpactRadius = 200.0f;
	RadialImpactForce = 2000.0f;
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
	if (OldFocusedActor)
	{
		IInteractInterface* Interface = Cast<IInteractInterface>(OldFocusedActor);
		if (Interface)
		{
			Interface->Execute_OnInteract(OldFocusedActor, this);
		}
	}
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
	//DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 5.0f);

	if (bHit)
	{
		//DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(10, 10, 10), FColor::Emerald, false, 5.0f);
		AActor* NewInteractable = Hit.GetActor();
		
		
		if (NewInteractable)
		{
			if (NewInteractable != OldFocusedActor)
			{
				if (OldFocusedActor)
				{
					IInteractInterface* Interface = Cast<IInteractInterface>(OldFocusedActor);
					if (Interface)
					{
						Interface->Execute_EndFocus(OldFocusedActor);
					}
				}

				IInteractInterface* Interface = Cast<IInteractInterface>(NewInteractable);
				if (Interface)
				{
					Interface->Execute_StartFocus(NewInteractable);
				}

				OldFocusedActor = NewInteractable;
			}
		}
	}
	else
	{
		if (OldFocusedActor)
		{
			IInteractInterface* Interface = Cast<IInteractInterface>(OldFocusedActor);
			if (Interface)
			{
				Interface->Execute_EndFocus(OldFocusedActor);
				OldFocusedActor = nullptr;

			}
		}
	}
}

void AMPCharacterBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteractInterface* Interface = Cast<IInteractInterface>(OtherActor);
	if (Interface)
	{
		Interface->Execute_OnInteract(OtherActor, this);
	}
}

void AMPCharacterBase::FireForward()
{
	FVector Loc;
	FRotator Rot;
	FHitResult Hit;

	GetController()->GetPlayerViewPoint(Loc, Rot);

	FVector Start = Loc;
	FVector End = Start + (Rot.Vector() * TraceDistance);

	FCollisionQueryParams TraceParams;

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	// Checks if we hit something and that something is movable and that something is simulating physics rn
	if (bHit)
	{
		
		if (Hit.GetActor()->IsRootComponentMovable() && Hit.GetActor()->GetRootComponent()->IsSimulatingPhysics())
		{
			UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Hit.GetActor()->GetRootComponent());
			if (MeshComp)
			{
				FVector CameraForward = Camera->GetForwardVector();
				MeshComp->AddImpulse(CameraForward * ImpulseForce * MeshComp->GetMass());
			}
		}

		if (bApplyRadialForce)
		{
			FCollisionShape SphereColShape = FCollisionShape::MakeSphere(RadialImpactRadius);
			bool bSweepHit = GetWorld()->SweepMultiByChannel(HitActors, Hit.Location, Hit.Location + FVector(0, 0, 0.001f),
				FQuat::Identity, ECC_WorldStatic, SphereColShape);
			DrawDebugSphere(GetWorld(), Hit.Location, RadialImpactRadius, 50, FColor::Orange, false, 2.0f);
			if (bSweepHit)
			{
				for (auto& Casualty : HitActors)
				{
					UStaticMeshComponent* CasualtyMesh = Cast<UStaticMeshComponent>(Casualty.GetActor()->GetRootComponent());
					if (CasualtyMesh)
					{
						CasualtyMesh->AddRadialImpulse(Hit.Location, RadialImpactRadius, RadialImpactForce * 1000, ERadialImpulseFalloff::RIF_Constant, false);
					}
				}
			}
		}
		
	}
}

void AMPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AMPCharacterBase::TraceForward();
}

// Called to bind functionality to input
void AMPCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMPCharacterBase::InteractPress);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMPCharacterBase::FireForward);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMPCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMPCharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMPCharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMPCharacterBase::LookUpAtRate);


}