// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ROSPawnMovement.h"
#include "ROSFloatingMovement.generated.h"

/**
 * 
 */
UCLASS()
class ROSPROJECT_API UROSFloatingMovement : public UROSPawnMovement
{
	GENERATED_BODY()
public:
	UROSFloatingMovement();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ROSFloating)
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ROSFloating)
	float Deceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ROSFloating)
	float MaxForwardSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ROSFloating)
	float AngularAccelaration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ROSFloating)
	float AngularDecelaration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ROSFloating)
	float MaxAngularSpeed;
	
	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//End UActorComponent Interface
	
	//Begin UMovementComponent Interface
	virtual float GetMaxSpeed() const {return MaxForwardSpeed;}
	//End UMovementComponent Interface

	void SetVelocity(const FVector& LinearVelocity, const FVector& AngularVelocity) override
	{
		
	}

	void Stop()
	{
		
	}
private:

	//from UMovementComponent
	//FVector Velocity;

};
