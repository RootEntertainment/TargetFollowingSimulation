// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ROSMovement2WD.h"
#include "ROSPawn.h"
#include "ROSBot2Wheel.generated.h"

/**
 * 
 */
UCLASS()
class ROSPROJECT_API AROSBot2Wheel : public AROSPawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere)
	UROSMovement2WD* ROSMovement;
	
public:
	AROSBot2Wheel();
	virtual UPawnMovementComponent* GetMovementComponent() const override{ return ROSMovement; }
	virtual FVector ConsumeMovementInputVector() override;
	virtual void Tick(float DeltaSeconds) override;

	FORCEINLINE void EngineStart()
	{
		if(ROSMovement) ROSMovement->SetVelocity(FVector(MaxForwardVelocity, 0, 0), FVector(0, 0, MaxRotateVelocity));
	}
	FORCEINLINE virtual void SetVelocity(const FVector& LinearVelocity, const FVector& AngularVelocity) override
	{
		if(ROSMovement) ROSMovement->SetVelocity(LinearVelocity, AngularVelocity);
	}
	
	UPROPERTY(EditAnywhere)
	float MaxForwardVelocity;
	UPROPERTY(EditAnywhere)
	float MaxRotateVelocity;

	virtual FVector GetCurrentAngularVelocity(EROSSpace::EROSSpace Space) override;
	virtual FVector GetCurrentLinearVelocity(EROSSpace::EROSSpace Space) override;
	
protected:
	virtual FTransform GetOdomTransform() override;
};
