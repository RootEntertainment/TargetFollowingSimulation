// Fill out your copyright notice in the Description page of Project Settings.


#include "ROSController.h"

#include "ROSBot2Wheel.h"
#include "ROSIntegrationGameInstance.h"
#include "ROSTime.h"
#include "GameFramework/Character.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "geometry_msgs/PoseWithCovariance.h"
#include "geometry_msgs/Twist.h"
#include "Kismet/GameplayStatics.h"

AROSController::AROSController()
{
	CmdTopic = CreateDefaultSubobject<UTopic>(TEXT("CmdTopic"));
}

void AROSController::BeginPlay()
{
	Super::BeginPlay();

	UROSIntegrationGameInstance* ROSGameInst = Cast<UROSIntegrationGameInstance>(GetWorld()->GetGameInstance());
	if(ROSGameInst->ConnectedToROSBridge[0])
	{
		CmdTopic->Init(ROSGameInst->ROSIntegrationCore, CmdVelTopicName,TEXT("geometry_msgs/Twist"));

		if(GetPawn() != nullptr)
		{
			bIsSubribing = CmdTopic->Subscribe(std::bind(&AROSController::CmdTopicCallBack, this, std::placeholders::_1));
		}
	}
}

void AROSController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(HasActorBegunPlay() && !bIsSubribing)
	{
		bIsSubribing = CmdTopic->Subscribe(std::bind(&AROSController::CmdTopicCallBack, this, std::placeholders::_1));
	}

	TArray<AActor*> CameraActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), CameraActors);
	if(CameraActors.Num()>0)
	{
		SetViewTarget(CameraActors[0]);
	}
}

void AROSController::OnUnPossess()
{
	CmdTopic->Unsubscribe();
	bIsSubribing = false;
	Super::OnUnPossess();
}

void AROSController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("ROSMoveForward", this, &AROSController::MoveROSForward);
	InputComponent->BindAxis("ROSMoveRight", this, &AROSController::MoveROSRight);
	//InputComponent->BindAxis("ROSRotate", this, &AROSController::RotateROS);
	//InputComponent->BindAction("EngineStart", IE_Pressed, this, &AROSController::OnEngineStart);
}

void AROSController::MoveROSForward(float Value)
{
	if(GetPawn())
	{
		GetPawn()->AddMovementInput(GetPawn()->GetActorForwardVector(), Value);
	}
	if(bEnableControlCharacter && ControlCharacter)
	{
		ControlCharacter->AddMovementInput(ControlCharacter->GetActorForwardVector(), Value);
	}
}

void AROSController::MoveROSRight(float Value)
{
	if(GetPawn())
	{
		GetPawn()->AddMovementInput(GetPawn()->GetActorRightVector(), Value);
	}
	if(bEnableControlCharacter && ControlCharacter)
	{
		ControlCharacter->AddMovementInput(ControlCharacter->GetActorRightVector(), Value);
	}
}


void AROSController::RotateROS(float Value)
{
	if(GetPawn())
	{
		//AddYawInput(Value);
		//UFloatingPawnMovement* MoveComp = Cast<UFloatingPawnMovement>(GetPawn()->GetMovementComponent());
		//MoveComp->add
	}
}

void AROSController::OnEngineStart()
{
	if(GetPawn())
	{
		AROSBot2Wheel* Bot = Cast<AROSBot2Wheel>(GetPawn());
		if(Bot) Bot->EngineStart();
	}
}


void AROSController::CmdTopicCallBack(TSharedPtr<FROSBaseMsg> Msg)
{
	if(Msg->_MessageType == "geometry_msgs/Twist")
	{
		TSharedPtr<ROSMessages::geometry_msgs::Twist> CmdMsg = StaticCastSharedPtr<ROSMessages::geometry_msgs::Twist>(Msg);
		UE_LOG(LogTemp, Display, TEXT("AROSController::CmdTopicCallBack %s, %s, %s"), *(CmdMsg->_MessageType), *CmdMsg->angular.ToFVector().ToString(), *CmdMsg->linear.ToFVector().ToString());
		if(GetPawn())
		{
			AROSPawn* Bot = Cast<AROSPawn>(GetPawn());
			FVector LinearVelocity = CmdMsg->linear.ToFVector()*100;
			LinearVelocity.Y = -LinearVelocity.Y;
			FVector AngularVelocity = CmdMsg->angular.ToFVector();
			AngularVelocity.X = -AngularVelocity.X;
			AngularVelocity.Z = -AngularVelocity.Z;
			if(Bot) Bot->SetVelocity(LinearVelocity, AngularVelocity);
		}
	}
}
