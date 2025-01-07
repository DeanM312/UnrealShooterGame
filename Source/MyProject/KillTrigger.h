// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "KillTrigger.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AKillTrigger : public ATriggerBox
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay();

public:
	AKillTrigger();

	UFUNCTION()
		void Event(class AActor* overlappedActor, class AActor* otherActor);




	
};
