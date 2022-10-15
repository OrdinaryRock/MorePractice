// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "MPCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UCameraShakeBase;
class AInteractableBase;
class UCurveFloat;

UCLASS()
class MOREPRACTICE_API AMPCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMPCharacterBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera");
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera");
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player");
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* CurveFloat;

protected:

	// Functions
	void MoveForward(float Amount);
	void MoveRight(float Amount);
	void TurnAtRate(float Amount);
	void LookUpAtRate(float Amount);
	void InteractPress();

	// Trace
	UFUNCTION(BlueprintNativeEvent)
	void TraceForward();
	void TraceForward_Implementation();

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera");
	float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera");
	float BaseLookUpAtRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction");
	float TraceDistance;

	// Overlap
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult);

	// Impulse
	UPROPERTY(EditAnywhere)
	float ImpulseForce;

	void FireForward();

	UPROPERTY(EditAnywhere)
	bool bApplyRadialForce;
	UPROPERTY(EditAnywhere)
	float RadialImpactRadius;
	UPROPERTY(EditAnywhere)
	float RadialImpactForce;

	TArray<FHitResult> HitActors;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> CamShake;
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<AInteractableBase> ActorToSpawn;

	UFUNCTION()
	void SpawnObject(FVector Loc, FRotator Rot);

public:	

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;


private:
	AActor* OldFocusedActor;

	FVector StartScale;
	FVector TargetScale;
	FTimeline SquashTimeline;

	UFUNCTION()
		void SquashProgress(float Value);

};
