// Fill out your copyright notice in the Description page of Project Settings.


#include "ROSPawn.h"

#include "DrawDebugHelpers.h"
#include "ROSController.h"
#include "ROSIntegrationGameInstance.h"
#include "Engine/InputDelegateBinding.h"
#include "geometry_msgs/TransformStamped.h"
#include "Kismet/GameplayStatics.h"

void FStaticTFTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread,
                                        const FGraphEventRef& MyCompletionGraphEvent)
{
	if(ROSPawn && !ROSPawn->IsPendingKillOrUnreachable())
	{
		ROSPawn->StaticTFTick();
	}
}

// Sets default values
AROSPawn::AROSPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//AutoPossessAI = EAutoPossessAI::Disabled;
	//AIControllerClass = AROSController::StaticClass();

	StaticTFTopic = CreateDefaultSubobject<UTopic>(TEXT("StaticTFTopic"));
	OdomTopic = CreateDefaultSubobject<UTopic>(TEXT("OdomTopic"));

	ROSSkeleton = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ROSSkeletalMeshComponent"));
	SetRootComponent(ROSSkeleton);
}

void AROSPawn::StaticTFTick()
{
	//Update Odom 
	FTransform RelativeTransform = ToRosTransform(GetOdomTransform());
	StaticTfMessage->transforms[0].transform.translation = RelativeTransform.GetLocation();
	StaticTfMessage->transforms[0].transform.rotation = RelativeTransform.GetRotation();
	
	for(auto& Trans : StaticTfMessage->transforms)
	{
		Trans.header.time = FROSTime::Now();
	}
	
	StaticTFTopic->Publish(StaticTfMessage);

	OdomMessage->header.time = FROSTime::Now();
	OdomMessage->pose.pose.position = RelativeTransform.GetLocation();
	OdomMessage->pose.pose.orientation = RelativeTransform.GetRotation();
	OdomMessage->twist.twist.linear = ToRosVector(GetCurrentLinearVelocity(EROSSpace::BaseLink));
	OdomMessage->twist.twist.angular = ToRosVector(GetCurrentAngularVelocity(EROSSpace::BaseLink));
	
	OdomTopic->Publish(OdomMessage);
}

bool AROSPawn::IsMoveInputIgnored() const
{
	return !bEnableInputControl;
}

// Called when the game starts or when spawned
void AROSPawn::BeginPlay()
{
	Super::BeginPlay();
	OriginTransform = GetActorTransform();
	LastTransform = OriginTransform;
	UROSIntegrationGameInstance* ROSGameInst = Cast<UROSIntegrationGameInstance>(GetGameInstance());
	if(ROSGameInst && ROSGameInst->ConnectedToROSBridge[0])
	{
		StaticTFTopic->Init(ROSGameInst->ROSIntegrationCore, "/tf",TEXT("tf2_msgs/TFMessage"));
		StaticTFTopic->Advertise();
		OdomTopic->Init(ROSGameInst->ROSIntegrationCore, OdomTopicName,TEXT("nav_msgs/Odometry"));
		OdomTopic->Advertise();
		InitStaticTFMessage();
		RegisterStaticTFTickFunction(true);
	}
	
	// APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	// AROSPlayerController* ROSPC;
	// if(PC && (ROSPC = Cast<AROSPlayerController>(PC)))
	// {
	// 	ROSPC->SetROSPawn(this);
	// }
	UE_LOG(LogTemp, Warning, TEXT("%p"), GetMovementComponent());
}

void AROSPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RegisterStaticTFTickFunction(false);
	Super::EndPlay(EndPlayReason);
}

void AROSPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ROSPawnMovement = Cast<UROSPawnMovement>(GetMovementComponent());
}

void AROSPawn::InitStaticTFMessage()
{
	StaticTfMessage = MakeShared<ROSMessages::tf2_msgs::TFMessage>();

	ROSMessages::geometry_msgs::TransformStamped TransformStamped;
	TransformStamped.header.frame_id = OdomCoordinate;
	TransformStamped.header.time = FROSTime::Now();
	TransformStamped.header.seq = 0;
	TransformStamped.child_frame_id = BaseCoordinate;
	StaticTfMessage->transforms.Add(TransformStamped);
	
	TArray<UActorComponent*> Components;
	GetComponents(UROSComponent::StaticClass(), Components);
	for(auto Comp : Components)
	{
		UROSComponent* ROSComp = Cast<UROSComponent>(Comp);
		//UE_LOG(LogROS, Warning, TEXT("TF Added %s"), *ROSComp->GetCoordinateID());
		FTransform ROSTrans = ToRosTransform(ROSComp->GetRelativeTransform());
		
		TransformStamped.header.frame_id = BaseCoordinate;
		TransformStamped.header.time = FROSTime::Now();
		TransformStamped.header.seq = 0;
		TransformStamped.child_frame_id = ROSComp->GetCoordinateID();
		TransformStamped.transform.translation = ROSTrans.GetLocation();
		TransformStamped.transform.rotation = ROSTrans.GetRotation();
		StaticTfMessage->transforms.Add(TransformStamped);
	}

	//odometry msg
	OdomMessage = MakeShared<ROSMessages::nav_msgs::Odometry>();
	OdomMessage->header.frame_id = OdomCoordinate;
	OdomMessage->header.time = FROSTime::Now();
	OdomMessage->header.seq = 0;
	OdomMessage->child_frame_id = BaseCoordinate;

	OdomMessage->pose.covariance.SetNum(36);
	OdomMessage->pose.covariance[0] = 0.1;
	OdomMessage->pose.covariance[7] = 0.1;  // y方向的不确定性
	OdomMessage->pose.covariance[14] = 0.1; // z方向的不确定性
	OdomMessage->pose.covariance[21] = 0.1; // 旋转x方向的不确定性
	OdomMessage->pose.covariance[28] = 0.1; // 旋转y方向的不确定性
	OdomMessage->pose.covariance[35] = 0.1; // 旋转z方向的不确定性

	OdomMessage->twist.covariance.SetNum(36);
	OdomMessage->twist.covariance[0] = 0.1;
	OdomMessage->twist.covariance[7] = 0.1;  // y方向的不确定性
	OdomMessage->twist.covariance[14] = 0.1; // z方向的不确定性
	OdomMessage->twist.covariance[21] = 0.1; // 旋转x方向的不确定性
	OdomMessage->twist.covariance[28] = 0.1; // 旋转y方向的不确定性
	OdomMessage->twist.covariance[35] = 0.1; // 旋转z方向的不确定性
}

void AROSPawn::RegisterStaticTFTickFunction(bool bRegister)
{
	if(!IsTemplate())
	{
		if(bRegister)
		{
			StaticTFTickFunction.bCanEverTick = true;
			StaticTFTickFunction.ROSPawn = this;
			StaticTFTickFunction.SetTickFunctionEnable(true);
			StaticTFTickFunction.RegisterTickFunction(GetLevel());
		}
		else
		{
			if(StaticTFTickFunction.IsTickFunctionRegistered())
			{
				StaticTFTickFunction.UnRegisterTickFunction();			
			}
		}
	}
}

// Called every frame
void AROSPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//update velocity
	FVector DeltaLocation = GetActorTransform().GetLocation() - LastTransform.GetLocation();
	FQuat DeltaRotation = GetActorTransform().GetRotation() - LastTransform.GetRotation();
	CurrentLinearVelocity = DeltaLocation / DeltaTime;
	CurrentAngularVelocity = DeltaRotation.Euler() / 180 * PI / DeltaTime;

	if(bShowTrace)
	{
		DrawDebugLine(GetWorld(), LastTransform.GetLocation(), GetActorLocation(), FColor::Green, true);
	}
	
	LastTransform = GetActorTransform();

	if(ROSPawnMovement)
	{
		ROSPawnMovement->Velocity = CurrentLinearVelocity;
		ROSPawnMovement->UpdateComponentVelocity();
	}
	
}

