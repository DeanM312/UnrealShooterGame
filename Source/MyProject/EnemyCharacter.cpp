// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "EnemyAI.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	hp = 100;


	BulletTrail = CreateDefaultSubobject<UParticleSystemComponent>(FName("BulletTrail"));
	BulletTrail->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<USoundBase> FIRE(TEXT("/Game/Sounds/Lines/deathnew"));
	DeathLine = FIRE.Object;

	elite = false;


}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	ostartpos = GetActorLocation();

	AEnemyAI* EnemyController = Cast<AEnemyAI>(GetController());
	EnemyController->ostartpos = ostartpos;

	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyCharacter::Die(FVector force, FName bone)
{
	AEnemyAI* EnemyController = Cast<AEnemyAI>(GetController());
	EnemyController->Die();
	DetachFromControllerPendingDestroy();
	
	/* Disable all collision on capsule */
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);

	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
	GetMesh()->AddImpulseAtLocation(force * 50000, GetActorLocation(), bone);

	SetLifeSpan(10.0f);


	if (DeathLine != NULL && FMath::RandRange(0, 2) == 0) {
		UGameplayStatics::PlaySoundAtLocation(this, DeathLine, GetActorLocation());
	}



}

void AEnemyCharacter::Fire(FVector end)
{
	BulletTrail->Activate(true);
	BulletTrail->SetBeamSourcePoint(0, GetActorLocation() +FVector(0.0f, 0.0f, 40.0f), 0);
	BulletTrail->SetBeamTargetPoint(0, end, 0);
}

