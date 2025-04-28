// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSeacherComponent.h"

#include "DrawDebugHelpers.h"
#include "ROSConsts.h"


UTargetSeacherComponent::UTargetSeacherComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	GoalTopic = CreateDefaultSubobject<UTopic>("GoalTopic");
}

void UTargetSeacherComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickInterval(1.f / SearchRate);
	
	UROSIntegrationGameInstance* ROSGameInst = Cast<UROSIntegrationGameInstance>(GetWorld()->GetGameInstance());
	if(ROSGameInst && ROSGameInst->ConnectedToROSBridge[0])
	{
		GoalTopic->Init(ROSGameInst->ROSIntegrationCore, TopicName,TEXT("geometry_msgs/PoseStamped"));
		GoalTopic->Advertise();
	}
	PoseMsg = MakeShared<ROSMessages::geometry_msgs::PoseStamped>();
	PoseMsg->header.frame_id = FrameID;
	PoseMsg->header.seq = 0;

	
	CurrentTarget = GetOwner()->GetTransform();
}

void UTargetSeacherComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(TargetActor)
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());
		QueryParams.AddIgnoredActor(TargetActor);
		
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, GetComponentLocation(), TargetActor->GetActorLocation(), ECC_Vehicle, QueryParams);  // 发射射线

		if(!bHit)
		{
			CurrentPose = TargetActor->GetTransform();
			FVector Direction = TargetActor->GetActorLocation() - GetComponentLocation();
			if(Direction.Size() < FollowDistance)
			{
				CurrentTarget = GetOwner()->GetTransform();
			}
			else
			{
				Direction.Normalize();
				FVector Location = CurrentPose.GetLocation() - Direction * FollowDistance;
				FQuat Rotation = Direction.ToOrientationQuat();
				CurrentTarget.SetLocation(Location);
				CurrentTarget.SetRotation(Rotation);
			}
			
		}else
		{
			CurrentTarget = CurrentPose;
		}
		
		PoseMsg->header.time = FROSTime::Now();
		FTransform Transform = ToRosTransform(CurrentTarget.GetRelativeTransform(GetComponentTransform()));
		PoseMsg->pose.position = Transform.GetLocation();
		PoseMsg->pose.orientation = Transform.GetRotation();
		
		GoalTopic->Publish(PoseMsg);

		DrawDebugSphere(GetWorld(), CurrentTarget.GetLocation(), 20, 20, FColor::Yellow, false, 1.f / SearchRate);
		DrawDebugSphere(GetWorld(), CurrentPose.GetLocation(), 20, 20, FColor::Green, false, 1.f / SearchRate);
		
	}
}

FString UTargetSeacherComponent::GetCoordinateID()
{
	return FrameID;
}
