// Fill out your copyright notice in the Description page of Project Settings.


#include "ROSFloatingMovement.h"

UROSFloatingMovement::UROSFloatingMovement()
{
	MaxForwardSpeed = 200.f;
	Acceleration = 400.f;
	Deceleration = 800.f;

	AngularAccelaration = 1.f;
	AngularDecelaration = 3.f;
}

void UROSFloatingMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// FVector Delta = UpdatedComponent->GetForwardVector() * DeltaTime * CurForwardSpeed;
	// FQuat Quat(FVector(0, 0, 1), CurAngularSpeed * DeltaTime);
	// MoveUpdatedComponent(Delta, Quat, false);
	//
	// if(CurForwardSpeed < DesiredForwardSpeed)
	// {
	// 	CurAngularSpeed += AngularAccelaration;
	// }
}