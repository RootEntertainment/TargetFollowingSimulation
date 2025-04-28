// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RI/Topic.h"
#include "Camera/CameraActor.h"
#include "ROSController.generated.h"

/**
 * 
 */
UCLASS()
class ROSPROJECT_API AROSController : public APlayerController
{
	GENERATED_BODY()
public:
	AROSController();
	UPROPERTY(EditAnywhere, Category = ROSController)
	FString CmdVelTopicName = "/cmd_vel";

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<ACharacter> ControlCharacter;
	UPROPERTY(EditAnywhere)
	bool bEnableControlCharacter;
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void SetupInputComponent() override;
	
	UPROPERTY()
	UTopic* CmdTopic;
	bool bIsSubribing = false;

	void CmdTopicCallBack(TSharedPtr<FROSBaseMsg> Msg);

	void MoveROSForward(float Value);
	void MoveROSRight(float Value);
	void RotateROS(float Value);
	void OnEngineStart();
};
