// Fill out your copyright notice in the Description page of Project Settings.


#include "ROSBot2Wheel.h"

AROSBot2Wheel::AROSBot2Wheel()
{
	ROSMovement = CreateDefaultSubobject<UROSMovement2WD>(TEXT("ROSMovement2WD"));
}

FVector AROSBot2Wheel::ConsumeMovementInputVector()
{
	FVector Forward = GetActorForwardVector();
	FVector Right = GetActorRightVector();

	float ForwardProjection = FVector::DotProduct(ControlInputVector, Forward) / Forward.Size();
	float RightProjection = FVector::DotProduct(ControlInputVector, Right) / Right.Size();

	ROSMovement->SetVelocity(ForwardProjection * FVector(MaxForwardVelocity, 0, 0), RightProjection * FVector(0, 0, MaxRotateVelocity));

	return Internal_ConsumeMovementInputVector();
}

void AROSBot2Wheel::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//UE_LOG(LogROS, Warning, TEXT("%s"), *ROSSkeleton->GetBoneQuaternion("LeftWheel", EBoneSpaces::ComponentSpace).Rotator().ToString());
	//ROSSkeleton->Constraints[0]->GetAngularSwing2Motion()
	//UE_LOG(LogROS, Warning, TEXT("Odom: %s"), *ROSMovement->GetOdometryTransform().ToString());
	//UE_LOG(LogROS, Warning, TEXT("Origin: %s"), *GetActorTransform().GetRelativeTransform(GetOriginTransform()).ToString());

	//UE_LOG(LogROS, Warning, TEXT("DesiredAngularVelocity: %s"), *ROSMovement->GetDesiredAngularVelocity().ToString());
	//UE_LOG(LogROS, Warning, TEXT("AngularVelocity: %s"), *GetCurrentAngularVelocity().ToString());
	//UE_LOG(LogROS, Warning, TEXT("LinearVelocity: %s"), *GetCurrentLinearVelocity().ToString());
	
	if(ROSMovement && !IsMoveInputIgnored()) ConsumeMovementInputVector();
}

FVector AROSBot2Wheel::GetCurrentAngularVelocity(EROSSpace::EROSSpace Space)
{
	if(!ROSMovement) return Super::GetCurrentAngularVelocity(Space);
	return ROSMovement->GetAngularVelocity();
}

FVector AROSBot2Wheel::GetCurrentLinearVelocity(EROSSpace::EROSSpace Space)
{
	if(!ROSMovement) return Super::GetCurrentLinearVelocity(Space);
	switch(Space)
	{
	case EROSSpace::BaseLink:
		return FVector(ROSMovement->GetForwardVelocity(), 0, 0);
	case EROSSpace::OdomSpace:
		return ROSMovement->GetLinearVelocity();
	case EROSSpace::WorldSpace:
	default:
		return Super::GetCurrentLinearVelocity(Space);
	}
}

FTransform AROSBot2Wheel::GetOdomTransform()
{
	if(ROSMovement) return ROSMovement->GetOdometryTransform();
	else return FTransform();
}
