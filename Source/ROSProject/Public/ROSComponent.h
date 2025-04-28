// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ROSComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROSPROJECT_API UROSComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	FORCEINLINE virtual FString GetCoordinateID();
};
