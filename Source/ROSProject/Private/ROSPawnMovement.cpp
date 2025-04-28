// Fill out your copyright notice in the Description page of Project Settings.


#include "ROSPawnMovement.h"

#include "AI/Navigation/AvoidanceManager.h"

UROSPawnMovement::UROSPawnMovement() : DesiredLinearVelocity(), DesiredAngularVelocity()
{
}

void UROSPawnMovement::BeginPlay()
{
	Super::BeginPlay();
	AvoidanceUID = 0;
	if (GetOwner() != nullptr)
	{
		UAvoidanceManager* AvoidanceManager = GetWorld()->GetAvoidanceManager();
		if (AvoidanceManager)
		{
			AvoidanceManager->RegisterMovementComponent(this, AvoidanceWeight);
			UE_LOG(LogTemp, Warning, TEXT("AvoidanceManager->RegisterMovementComponent"));
		}
	}
}

void UROSPawnMovement::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UAvoidanceManager* AvoidanceManager = GetWorld()->GetAvoidanceManager();
	if (AvoidanceManager)
	{
		AvoidanceManager->UpdateRVO(this);
	}
}


void UROSPawnMovement::SetRVOAvoidanceUID(int32 UID)
{
	AvoidanceUID = UID;
}

int32 UROSPawnMovement::GetRVOAvoidanceUID()
{
	return AvoidanceUID;
}

void UROSPawnMovement::SetRVOAvoidanceWeight(float Weight)
{
	AvoidanceWeight = Weight;
}

float UROSPawnMovement::GetRVOAvoidanceWeight()
{
	return AvoidanceWeight;
}

FVector UROSPawnMovement::GetRVOAvoidanceOrigin()
{
	return GetActorFeetLocation();
	//return UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
}

float UROSPawnMovement::GetRVOAvoidanceRadius()
{
	return RVOAvoidanceRadius;
}

float UROSPawnMovement::GetRVOAvoidanceHeight()
{
	return RVOAvoidanceHeight;
}

float UROSPawnMovement::GetRVOAvoidanceConsiderationRadius()
{
	return RVOAvoidanceRadius;
}

FVector UROSPawnMovement::GetVelocityForRVOConsideration()
{
	//UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *Velocity.ToString());
	return Velocity;
}

void UROSPawnMovement::SetAvoidanceGroupMask(int32 GroupFlags)
{
	AvoidanceGroup.SetFlagsDirectly(GroupFlags);
}

int32 UROSPawnMovement::GetAvoidanceGroupMask()
{
	return AvoidanceGroup.Packed;
}

void UROSPawnMovement::SetGroupsToAvoidMask(int32 GroupFlags)
{
	GroupsToAvoid.SetFlagsDirectly(GroupFlags);
}

int32 UROSPawnMovement::GetGroupsToAvoidMask()
{
	return GroupsToAvoid.Packed;
}

void UROSPawnMovement::SetGroupsToIgnoreMask(int32 GroupFlags)
{
	GroupsToIgnore.SetFlagsDirectly(GroupFlags);
}

int32 UROSPawnMovement::GetGroupsToIgnoreMask()
{
	return GroupsToIgnore.Packed;
}
