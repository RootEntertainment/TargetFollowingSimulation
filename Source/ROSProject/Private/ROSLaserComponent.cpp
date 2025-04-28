// Fill out your copyright notice in the Description page of Project Settings.


#include "ROSLaserComponent.h"

#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UROSLaserComponent::UROSLaserComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
	LaserTopic = CreateDefaultSubobject<UTopic>(TEXT("LaserTopic"));
}


// Called when the game starts
void UROSLaserComponent::BeginPlay()
{
	Super::BeginPlay();
	MaxDistanceInternal = MaxDistance * 100.f;
	ScanInterval = 1.f / ScanRate;
	AngleStep = 360.0f / Resolution;
	SetComponentTickInterval(ScanInterval);
	CreateLaserMessage();
	
	UROSIntegrationGameInstance* ROSGameInst = Cast<UROSIntegrationGameInstance>(GetWorld()->GetGameInstance());
	if(ROSGameInst && ROSGameInst->ConnectedToROSBridge[0])
	{
		LaserTopic->Init(ROSGameInst->ROSIntegrationCore, TopicName,TEXT("sensor_msgs/LaserScan"));
		LaserTopic->Advertise();
	}
	
	LaserBuffer.SetNum(Resolution);
}

void UROSLaserComponent::OnRegister()
{
	Super::OnRegister();
}

void UROSLaserComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	LaserTopic->Unadvertise();
	Super::EndPlay(EndPlayReason);
}


void UROSLaserComponent::Scan()
{
	FVector StartLocation = GetComponentLocation();  // 射线起始位置
	
	for (int i = 0; i < Resolution; ++i)
	{
		float Angle = i * AngleStep;
		//FVector Direction = FVector(FMath::Cos(FMath::DegreesToRadians(Angle)), FMath::Sin(FMath::DegreesToRadians(Angle)), 0.f);  // 计算xz平面内的方向向量
		FVector Direction = GetOwner()->GetActorForwardVector().RotateAngleAxis(Angle, FVector(0.0f, 0.0f, -1.0f));
		FVector EndLocation = StartLocation + Direction * MaxDistanceInternal;

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());  // 忽略自身

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_PhysicsBody, QueryParams);  // 发射射线

		if (bHit)
		{
			LaserBuffer[i] = (HitResult.ImpactPoint - StartLocation).Size();  // 添加射线命中的距离
			if(bShowLaser) DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Red, false, ScanInterval);
		}
		else
		{
			LaserBuffer[i] = MaxDistanceInternal;  // 添加射线未命中的最大距离
			if(bShowLaser) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, ScanInterval);
		}
	}
}

// Called every frame
void UROSLaserComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Scan();
	LaserMessage->header.time = FROSTime::Now();
	LaserMessage->ranges.SetNum(LaserBuffer.Num());
	for(int i=0; i < LaserBuffer.Num(); i++)
	{
		LaserMessage->ranges[i] = LaserBuffer[i]/100.f;
	}

#if WITH_EDITOR
	UE_LOG(LogROS, Display, TEXT("LaserComponent: ROSTime %lu.%lu"), LaserMessage->header.time._Sec, LaserMessage->header.time._NSec);
#endif
	if(LaserTopic->IsAdvertising()) LaserTopic->Publish(LaserMessage);
}

FString UROSLaserComponent::GetCoordinateID()
{
	return LaserFrameID;
}

