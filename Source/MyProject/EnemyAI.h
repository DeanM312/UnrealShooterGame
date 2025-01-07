
#pragma once

#include "Core.h"
#include "AIController.h"
#include "EnemyAI.generated.h"

UCLASS()
class MYPROJECT_API AEnemyAI : public AAIController
{
	GENERATED_BODY()


	UFUNCTION()
	void RepeatingFunction();


	UFUNCTION()
		void TakeTheShot(float offset);



	UFUNCTION()
		void Peak();

	
	

public:

	AEnemyAI();



	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;


	FTimerHandle MemberTimerHandle;
	FTimerHandle ShotsHandle;
	FTimerHandle PeakHandle;
	FTimerDelegate Delegate;

	FVector currentfocus;
	FVector newfocus;
	FVector startpos;
	FVector ostartpos;
	FVector cpos;
	FVector playerpos;
	int hits = 0;
	int shot = 0;

	USoundBase* FireSound;

	USoundBase* AttackLine;
	TArray<USoundBase*> RandomLines;
	USoundBase* GeneralLine;

	float dist = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	FRotator currentrotation;

	float aimaccel = 0.0f;

	TArray<AActor*> friends;

	int tick;

	bool reacted;
	bool elite;
};
