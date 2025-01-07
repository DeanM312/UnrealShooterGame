// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonTrigger.h"
#include "LockedDoor.h"
#include "EnemySpawn.h"


// Sets default values
AButtonTrigger::AButtonTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AButtonTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AButtonTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AButtonTrigger::Press()
{
	
	

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALockedDoor::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		ALockedDoor* DoorActor = Cast<ALockedDoor>(FoundActors[0]);

		if (DoorActor)
		{
			DoorActor->Open();
		}
	}


	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawn::StaticClass(), FoundActors);
	for (int i = 0; i < FoundActors.Num(); i++)
	{
		AEnemySpawn* DoorActor = Cast<AEnemySpawn>(FoundActors[i]);

		if (DoorActor)
		{
			DoorActor->Spawn();
		}
	}
	
}

