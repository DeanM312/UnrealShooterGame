// Fill out your copyright notice in the Description page of Project Settings.


#include "KillTrigger.h"
#include "EnemyCharacter.h"

AKillTrigger::AKillTrigger()
{

	OnActorBeginOverlap.AddDynamic(this, &AKillTrigger::Event);
}


void AKillTrigger::BeginPlay()
{
	Super::BeginPlay();


}


void AKillTrigger::Event(class AActor* overlappedActor, class AActor* otherActor)
{
	if (otherActor)
	{

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "eh");

		AEnemyCharacter* EnemyRef = Cast<AEnemyCharacter>(otherActor);
		if (otherActor == GetWorld()->GetFirstPlayerController()->GetPawn())
		{
			UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
		}
		else if (EnemyRef)
		{
			EnemyRef->Die(FVector(0,0,0),"head");
		}

	}

}