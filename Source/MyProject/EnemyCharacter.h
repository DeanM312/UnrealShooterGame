// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "EnemyCharacter.generated.h"


UCLASS()
class MYPROJECT_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame

	void Die(FVector force, FName bone);

	void Fire(FVector force);

	virtual void Tick(float DeltaTime) override;
	FVector ostartpos;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	int hp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
		UParticleSystemComponent* BulletTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool elite;

	USoundBase* DeathLine;
};
