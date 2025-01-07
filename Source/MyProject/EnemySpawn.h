
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawn.generated.h"

UCLASS()
class MYPROJECT_API AEnemySpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void Spawn();

	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = Gameplay)
		TSubclassOf<class AEnemyCharacter> bpRef;

};
