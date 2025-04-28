// Fill out your copyright notice in the Description page of Project Settings.


#include "ROSMovement2WD.h"

#include "ROSIntegrationCore.h"


UROSMovement2WD::UROSMovement2WD() : ROSPawnSkeleton(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
	//PrimaryComponentTick.TickInterval = 0.04;
}

void UROSMovement2WD::BeginPlay()
{
	Super::BeginPlay();
	LeftLastSwing = 0;
	RightLastSwing = 0;
	
	ROSPawnSkeleton = Cast<USkeletalMeshComponent>(UpdatedComponent);

	if(ROSPawnSkeleton && ROSPawnSkeleton->GetPhysicsAsset())
	{
		TArray<struct FConstraintInstance*>& Constraints = ROSPawnSkeleton->Constraints;
		for(int32 i=0; i < Constraints.Num(); i++)
		{
			if(Constraints[i]->JointName == LeftWheelBone)
			{
				LeftConstraint = Constraints[i];
				LeftConstraint->SetAngularDriveParams(500, 100000, 0);
			}else if(Constraints[i]->JointName == RightWheelBone)
			{
				RightConstraint = Constraints[i];
				RightConstraint->SetAngularDriveParams(500, 100000, 0);
			}
		}
	}
}

void UROSMovement2WD::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//UE_LOG(LogROS, Warning, TEXT("%f"), (GetOwner()->GetActorLocation() - LastLocation).Size() / DeltaTime);
	
	//Odom
	UpdateOdometry(DeltaTime);

	//Add Damping
	if(ROSPawnSkeleton && GetOwner())
	{
		FVector PhysicsAngularVelocity = ROSPawnSkeleton->GetPhysicsAngularVelocityInRadians("Root");
		FVector BalanceAxis = GetOwner()->GetActorRightVector();
		FVector Projection = BalanceAxis * (PhysicsAngularVelocity | BalanceAxis);
		PhysicsAngularVelocity -= Projection * 0.2;
		ROSPawnSkeleton->SetPhysicsAngularVelocityInRadians(PhysicsAngularVelocity);
	}
}

void UROSMovement2WD::SetVelocity(const FVector& InLinearVelocity, const FVector& InAngularVelocity)
{
	Super::SetVelocity(InLinearVelocity, InAngularVelocity);
	float Diff = InAngularVelocity.Z * WheelBase;
	float Sum = InLinearVelocity.X * 2;
	
	SetAllWheelVelocity((Sum+Diff) / LeftWheelRadius / 2 / (2*PI), (Sum-Diff) / RightWheelRadius / 2 / (2*PI));
	//UE_LOG(LogROS, Warning, TEXT("set lv: %f"), (Sum+Diff) / LeftWheelRadius / 2);
	//UE_LOG(LogROS, Warning, TEXT("set rv: %f"), (Sum-Diff) / RightWheelRadius / 2);
}

void UROSMovement2WD::Brake()
{
	Super::Brake();
	SetAllWheelVelocity(0, 0);
}

void UROSMovement2WD::ReleaseBrake()
{
	Super::ReleaseBrake();
	
}

void UROSMovement2WD::AddInputVector(FVector WorldVector, bool bForce)
{
	Super::AddInputVector(WorldVector, bForce);
}

void UROSMovement2WD::SetNamedWheelVelocity(const FName& Name, const FVector& InVelocity)
{
	if(!ROSPawnSkeleton)
	{
		return;
	}
	UPhysicsAsset* const PhysicsAsset = ROSPawnSkeleton->GetPhysicsAsset();
	if( !PhysicsAsset )
	{
		return;
	}
	TArray<struct FConstraintInstance*>& Constraints = ROSPawnSkeleton->Constraints;
	for(int32 i=0; i < Constraints.Num(); i++)
	{
		if(Constraints[i]->JointName == Name)
		{
			Constraints[i]->SetAngularVelocityDriveTwistAndSwing(false, true);
			Constraints[i]->SetAngularVelocityTarget(InVelocity);
			//Constraints[i]->SetAngularDriveParams()
			UE_LOG(LogROS, Warning, TEXT("Drive Stiffness:%f, Damping: %f, MaxForce: %f."), Constraints[i]->ProfileInstance.AngularDrive.SwingDrive.Stiffness, Constraints[i]->ProfileInstance.AngularDrive.SwingDrive.Damping, Constraints[i]->ProfileInstance.AngularDrive.SwingDrive.MaxForce);
		}
	}
	
}

void UROSMovement2WD::SetAllWheelVelocity(const float LeftVelocity, const float RightVelocity)
{
	// if(!ROSPawnSkeleton)
	// {
	// 	return;
	// }
	// UPhysicsAsset* const PhysicsAsset = ROSPawnSkeleton->GetPhysicsAsset();
	// if( !PhysicsAsset )
	// {
	// 	return;
	// }
	// TArray<struct FConstraintInstance*>& Constraints = ROSPawnSkeleton->Constraints;
	// for(int32 i=0; i < Constraints.Num(); i++)
	// {
	// 	if(Constraints[i]->JointName == LeftWheelBone)
	// 	{
	// 		Constraints[i]->SetAngularVelocityDriveTwistAndSwing(false, true);
	// 		Constraints[i]->SetAngularVelocityTarget(FVector(0, LeftVelocity, 0));
	// 		//Constraints[i]->SetAngularDriveParams()
	// 		UE_LOG(LogROS, Warning, TEXT("Drive Stiffness:%f, Damping: %f, MaxForce: %f."), Constraints[i]->ProfileInstance.AngularDrive.SwingDrive.Stiffness, Constraints[i]->ProfileInstance.AngularDrive.SwingDrive.Damping, Constraints[i]->ProfileInstance.AngularDrive.SwingDrive.MaxForce);
	// 	}
	// 	else if(Constraints[i]->JointName == RightWheelBone)
	// 	{
	// 		Constraints[i]->SetAngularVelocityDriveTwistAndSwing(false, true);
	// 		Constraints[i]->SetAngularVelocityTarget(FVector(0, RightVelocity, 0));
	// 		UE_LOG(LogROS, Warning, TEXT("Drive Stiffness:%f, Damping: %f, MaxForce: %f."), Constraints[i]->ProfileInstance.AngularDrive.SwingDrive.Stiffness, Constraints[i]->ProfileInstance.AngularDrive.SwingDrive.Damping, Constraints[i]->ProfileInstance.AngularDrive.SwingDrive.MaxForce);
	// 	}
	//
	// }
	if(LeftConstraint)
	{
		LeftConstraint->SetAngularVelocityDriveTwistAndSwing(false, true);
		//LeftConstraint->SetAngularDriveParams(500, 10000000, 0);
		LeftConstraint->SetAngularVelocityTarget(FVector(0, LeftVelocity, 0));
		
	}

	if(RightConstraint)
	{
		RightConstraint->SetAngularVelocityDriveTwistAndSwing(false, true);
		//RightConstraint->SetAngularDriveParams(500, 10000000, 0);
		RightConstraint->SetAngularVelocityTarget(FVector(0, RightVelocity, 0));
	}
}

void UROSMovement2WD::UpdateOdometry(float DeltaTime)
{
	if(!LeftConstraint || !RightConstraint) return;
	float LeftCurrentSwing = LeftConstraint->GetCurrentSwing2();
	float LeftVelocity =  CalcWheelVelocity(DeltaTime, LeftLastSwing, LeftCurrentSwing) * LeftWheelRadius;
	//UE_LOG(LogROS, Warning, TEXT("lv: %f"), LeftVelocity / LeftWheelRadius);

	float RightCurrentSwing = RightConstraint->GetCurrentSwing2();
	float RightVelocity = CalcWheelVelocity(DeltaTime, RightLastSwing, RightCurrentSwing) * RightWheelRadius;
	//UE_LOG(LogROS, Warning, TEXT("rv: %f"), RightVelocity / RightWheelRadius);
	
	ForwardVelocity = (LeftVelocity + RightVelocity) / 2;
	RotateVelocity = (LeftVelocity - RightVelocity) / WheelBase;

	LinearVelocity = FVector(ForwardVelocity * FMath::Cos(OdometryRotation.Yaw),ForwardVelocity * FMath::Sin(OdometryRotation.Yaw),0);
	AngularVelocity.Z = RotateVelocity;
	
	OdometryLocation += LinearVelocity * DeltaTime;
	OdometryRotation.Yaw += RotateVelocity * DeltaTime;

	LeftLastSwing = LeftCurrentSwing;
	RightLastSwing = RightCurrentSwing;
}
