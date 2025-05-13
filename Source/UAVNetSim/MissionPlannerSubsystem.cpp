// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionPlannerSubsystem.h"

UMissionPlannerSubsystem::UMissionPlannerSubsystem()
{
	CurrentMissionPlanIndex = -1;
}

void UMissionPlannerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}
