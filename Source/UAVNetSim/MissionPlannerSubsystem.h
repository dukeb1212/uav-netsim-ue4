// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MissionPlannerSubsystem.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FMissionPlan
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString MissionName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FVector> Waypoints;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FRotator> WaypointOrientations;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UTexture2D*> Images;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector StartLocation;
};

UCLASS()
class UAVNETSIM_API UMissionPlannerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// Sets default values for this subsystem's properties
	UMissionPlannerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FMissionPlan> MissionPlans;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 CurrentMissionPlanIndex;
};
