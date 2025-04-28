// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ROSComponent.h"
#include "ROSConsts.h"
#include "ROSPawnMovement.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Pawn.h"
#include "nav_msgs/OccupancyGrid.h"
#include "nav_msgs/Odometry.h"
#include "RI/Topic.h"
#include "tf2_msgs/TFMessage.h"
#include "ROSPawn.generated.h"

class FStaticTFTickFunction : public FTickFunction
{
public:
	AROSPawn* ROSPawn;
	FStaticTFTickFunction() : ROSPawn(nullptr)
	{
		bCanEverTick = true;
	}

private:
	virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;
};

//ROSPawn的抽象基类，没有移动实现，能够回报Odom和BaseLink坐标系
UCLASS()
class ROSPROJECT_API AROSPawn : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* ROSSkeleton;
	
public:
	// Sets default values for this pawn's properties
	AROSPawn();
	
	void StaticTFTick();
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category="ROSPawn", DisplayName="Base Link")
	FString BaseCoordinate = "unrealros_base_link";
	UPROPERTY(EditAnywhere, Category="ROSPawn", DisplayName="Odom")
	FString OdomCoordinate = "unrealros_odom";
	UPROPERTY(EditAnywhere, Category="ROSPawn", DisplayName="Odom Topic Name")
	FString OdomTopicName = "/odom";	
	UPROPERTY(EditAnywhere, Category="ROSPawn", DisplayName="Keyboard Control")
	bool bEnableInputControl = true;
	
	virtual FVector GetCurrentLinearVelocity(EROSSpace::EROSSpace Space)
	{
		switch (Space)
		{
		case EROSSpace::BaseLink:
			return GetTransform().InverseTransformVectorNoScale(CurrentLinearVelocity);
		case EROSSpace::OdomSpace:
			return OriginTransform.InverseTransformVectorNoScale(CurrentLinearVelocity);
		case EROSSpace::WorldSpace:
		default:
			return CurrentLinearVelocity;
		}
	}
	virtual FVector GetCurrentAngularVelocity(EROSSpace::EROSSpace Space)
	{
		switch (Space)
		{
		case EROSSpace::BaseLink:
			return GetTransform().InverseTransformVectorNoScale(CurrentAngularVelocity);
		case EROSSpace::OdomSpace:
			return OriginTransform.InverseTransformVectorNoScale(CurrentAngularVelocity);
		case EROSSpace::WorldSpace:
		default:
			return CurrentAngularVelocity;
		}
	}
	virtual bool IsMoveInputIgnored() const override;
	virtual void SetVelocity(const FVector& LinearVelocity, const FVector& AngularVelocity){}

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PostInitializeComponents() override;
protected:
	FORCEINLINE FTransform GetOriginTransform()
	{
		return OriginTransform;
	}
	FORCEINLINE virtual FTransform GetOdomTransform()
	{
		return GetTransform().GetRelativeTransform(OriginTransform);
	}
	UROSPawnMovement* ROSPawnMovement;
private:
	UPROPERTY()
	UTopic* StaticTFTopic;
	TSharedPtr<ROSMessages::tf2_msgs::TFMessage> StaticTfMessage;
	UPROPERTY()
	UTopic* OdomTopic;
	TSharedPtr<ROSMessages::nav_msgs::Odometry> OdomMessage;
	void InitStaticTFMessage();
	
	FTransform OriginTransform;
	
	FStaticTFTickFunction StaticTFTickFunction;
	void RegisterStaticTFTickFunction(bool bRegister);

	FTransform LastTransform;
	FVector CurrentAngularVelocity;
	FVector CurrentLinearVelocity;
public:	
	// Called every frame
	
	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
