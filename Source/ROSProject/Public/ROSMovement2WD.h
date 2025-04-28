// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ROSPawnMovement.h"
#include "ROSMovement2WD.generated.h"

/**
 * 
 */
UCLASS()
class ROSPROJECT_API UROSMovement2WD : public UROSPawnMovement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName LeftWheelBone;
	UPROPERTY(EditAnywhere)
	FName RightWheelBone;
	UPROPERTY(EditAnywhere, Category="Odom")
	float LeftWheelRadius = 0.f;
	UPROPERTY(EditAnywhere, Category="Odom")
	float RightWheelRadius = 0.f;
	UPROPERTY(EditAnywhere, Category="Odom")
	float WheelBase = 0.f;
	//UPROPERTY(EditAnywhere, Category="Odom")
	//float OdomInterval = 0.f;
	
public:
	UROSMovement2WD();
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Unit cm/s rads/s
	UFUNCTION(BlueprintCallable)
	virtual void SetVelocity(const FVector& LinearVelocity, const FVector& AngularVelocity) override;
	UFUNCTION(BlueprintCallable)
	virtual void Brake() override;
	UFUNCTION(BlueprintCallable)
	virtual void ReleaseBrake() override;

	FTransform GetOdometryTransform()
	{
		return FTransform(FRotator(0, OdometryRotation.Yaw * 180 / PI,0), OdometryLocation);
	}

	virtual void AddInputVector(FVector WorldVector, bool bForce) override;

	FORCEINLINE FVector GetLinearVelocity()
	{
		return LinearVelocity;
	}
	FORCEINLINE FVector GetAngularVelocity()
	{
		return AngularVelocity;
	}

	FORCEINLINE float GetForwardVelocity() const
	{
		return ForwardVelocity;
	}
	FORCEINLINE float GetRotateVelocity() const
	{
		return RotateVelocity;
	}

private:
	USkeletalMeshComponent* ROSPawnSkeleton;
	FConstraintInstance* LeftConstraint;
	FConstraintInstance* RightConstraint;
	void SetNamedWheelVelocity(const FName& Name, const FVector& Velocity);
	//unit: rps
	void SetAllWheelVelocity(const float LeftVelocity, const float RightVelocity);

	//Odom
	float LeftLastSwing = 0;
	float RightLastSwing = 0;
	static float CalcWheelVelocity(float DeltaTime, float LastSwing, float CurrentSwing)
	{
		if(CurrentSwing - LastSwing > PI)
		{
			LastSwing += 2*PI;
		}
		if(CurrentSwing - LastSwing < -PI)
		{
			CurrentSwing += 2*PI;
		}
		//reverse for positive velocity
		return (LastSwing - CurrentSwing) / DeltaTime;
	}
	float CalcPhysicsWheelVelocity(FName WheelName)
	{
		if(ROSPawnSkeleton)
		{
			return ROSPawnSkeleton->GetPhysicsAngularVelocityInRadians(WheelName).Y;
		}
		return 0;
	}
	void UpdateOdometry(float DeltaTime);
	//cm
	FVector OdometryLocation;
	//rad
	FRotator OdometryRotation;
	
	//velocity
	float ForwardVelocity;
	float RotateVelocity;
	FVector LinearVelocity;
	FVector AngularVelocity;
};
