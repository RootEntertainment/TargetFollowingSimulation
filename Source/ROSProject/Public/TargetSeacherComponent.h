// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ROSComponent.h"
#include "RI/Topic.h"
#include "ROSIntegrationGameInstance.h"
#include "geometry_msgs/PoseStamped.h"

#include "TargetSeacherComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROSPROJECT_API UTargetSeacherComponent : public UROSComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	AActor* TargetActor;
	UPROPERTY(EditAnywhere)
	FString TopicName = "/goal";
	UPROPERTY(EditAnywhere)
	FString FrameID = "unrealros_goal";
	UPROPERTY(EditAnywhere)
	float SearchRate = 10;
	UPROPERTY(EditAnywhere)
	float FollowDistance = 100.f;
public:
	UTargetSeacherComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual FString GetCoordinateID() override;
private:
	UPROPERTY()
	UTopic* GoalTopic;
	FTransform CurrentPose;
	FTransform CurrentTarget;
	TSharedPtr<ROSMessages::geometry_msgs::PoseStamped> PoseMsg;
};
