// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerBox.h"
#include "LevelTrigger.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ALevelTrigger : public ATriggerBox
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay();

public:
	ALevelTrigger();

	UFUNCTION()
		void Event(class AActor* overlappedActor, class AActor* otherActor);

	TArray<FString> levels;
};
