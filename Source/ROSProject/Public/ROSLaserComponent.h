// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ROSComponent.h"
#include "Components/SceneComponent.h"
#include "ROSIntegrationCore.h"
#include "ROSIntegration/Classes/ROSIntegrationGameInstance.h"
#include "ROSIntegration/Classes/RI/Topic.h"
#include "sensor_msgs/LaserScan.h"
#include "std_msgs/String.h"

#include "ROSLaserComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROSPROJECT_API UROSLaserComponent : public UROSComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UROSLaserComponent();

	UPROPERTY(EditAnywhere, Category="Laser", DisplayName="Scan Rate Per Second (hz)")
	int ScanRate = 5;

	UPROPERTY(EditAnywhere, Category="Laser", DisplayName="Rays Per Round");
	int Resolution = 360;

	UPROPERTY(EditAnywhere, Category="Laser", DisplayName="MaxDistance (m)");
	float MaxDistance = 3.f;

	UPROPERTY(EditAnywhere, Category="Laser", DisplayName="Frame(Coordinate) ID");
	FString LaserFrameID = "laser_frame";

	UPROPERTY(EditAnywhere, Category="Laser", DisplayName="Topic Name");
	FString TopicName = TEXT("/unrealros_laser_topic");

	UPROPERTY(EditAnywhere, Category="Laser");
	bool bShowLaser = true;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//ROS
	UPROPERTY()
	UTopic* LaserTopic;
	TSharedPtr<ROSMessages::sensor_msgs::LaserScan> LaserMessage;
	void CreateLaserMessage()
	{
		LaserMessage = MakeShared<ROSMessages::sensor_msgs::LaserScan>();
		LaserMessage->header.frame_id = GetCoordinateID();
		//LaserMessage->header.time = FROSTime::Now();
		LaserMessage->angle_min = 0;
		LaserMessage->angle_max = 2 * PI;
		LaserMessage->angle_increment = 2.f * PI / Resolution; // 360度
		LaserMessage->range_min = 0.1;
		LaserMessage->range_max = MaxDistance;
	}
	//Laser
	float MaxDistanceInternal;
	float ScanInterval;
	float AngleStep;
	TArray<float> LaserBuffer;
	void Scan();
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE virtual FString GetCoordinateID() override;
};
