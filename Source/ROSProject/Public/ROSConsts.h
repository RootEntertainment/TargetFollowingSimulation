// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

namespace EROSSpace
{
	UENUM()
	enum EROSSpace
	{
		WorldSpace,
		OdomSpace,
		BaseLink,
	};
}

//反转Y轴
static FTransform ToRosTransform(FTransform Transform)
{
	FVector Location = Transform.GetLocation();
	Location.Y = -Location.Y;
	Location /= 100.f;//cm->m
	FQuat Quat = Transform.GetRotation();
	Quat.X = -Quat.X;
	Quat.Z = -Quat.Z;
	return FTransform(Quat, Location);
}
static FVector ToRosVector(FVector Vector)
{
	Vector/=100.f;
	Vector.Y = -Vector.Y;
	return Vector;
}