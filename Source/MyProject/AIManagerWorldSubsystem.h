// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIManagerWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UAIManagerWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void Register(AActor* actor);

	void Unregister(AActor* actor);

	TArray<AActor*> friends;
};
