#include "EnemySpawn.h"
#include "EnemyCharacter.h"
// Sets default values
AEnemySpawn::AEnemySpawn()
{

}

// Called when the game starts or when spawned
void AEnemySpawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemySpawn::Spawn()
{
	FVector Loc = GetActorLocation() + FVector(0,0,50);
	GetWorld()->SpawnActor(bpRef, &Loc);


}

