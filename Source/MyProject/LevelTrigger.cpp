// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTrigger.h"

ALevelTrigger::ALevelTrigger()
{
	OnActorBeginOverlap.AddDynamic(this, &ALevelTrigger::Event);

	levels.Add("Lounge");
	levels.Add("Spiral");
	levels.Add("Vent");
	levels.Add("Cross");
	levels.Add("Control");
	levels.Add("Raid");
	levels.Add("Breach");
	levels.Add("Shotgun");
	levels.Add("Chaos");

}

void ALevelTrigger::BeginPlay()
{
	Super::BeginPlay();
}

void ALevelTrigger::Event(class AActor* overlappedActor, class AActor* otherActor)
{
	if (otherActor && otherActor == GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		FString current = GetWorld()->GetMapName();

		current.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

		for (int i = 0; i < levels.Num(); i++)
		{
			if (current == levels[i])
			{
				UGameplayStatics::OpenLevel(GetWorld(), FName(levels[i+1]));
				break;

			}
		}

	}
}