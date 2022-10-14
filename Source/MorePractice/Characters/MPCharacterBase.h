// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MPCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;


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

protected:

	void MoveForward(float Amount);
	void MoveRight(float Amount);
	void TurnAtRate(float Amount);
	void LookUpAtRate(float Amount);
	void InteractPress();

	UFUNCTION(BlueprintNativeEvent)
	void TraceForward();
	void TraceForward_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera");
	float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera");
	float BaseLookUpAtRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction");
	float TraceDistance;

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
