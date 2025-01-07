// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectCharacter.h"
#include "MyProjectProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include <Runtime\Engine\Public\DrawDebugHelpers.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "ButtonTrigger.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMyProjectCharacter

AMyProjectCharacter::AMyProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	MouseSensitivity = 0.16f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 10.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);
	


	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	GetCharacterMovement()->AirControl = 1;

	hp = 100;

	PrimaryActorTick.bCanEverTick = true;
	
	flux = 0.0f;
	fluxcapacity = 10.0f;
	venting = false;

	currentweapon = 1;
}

void AMyProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	FP_Gun->SetRelativeLocation(FVector(5.0f,20.0f, 3.0f));
	

	//FirstPersonCameraComponent->DetachFromParent(true);

}

void AMyProjectCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (hp <= 100)
	{
		hp = hp + 10 * DeltaTime;
	}

	if (hp <= 0)
	{
		hp = 100;
		Death();
		
	}

	if (flux >= 0)
	{
		flux -= 0.3 * DeltaTime;

		if (venting)
		{
			flux -= 3 * DeltaTime;
		}
	}
	else
	{
		flux = 0;
		venting = false;
	}

}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyProjectCharacter::OnFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AMyProjectCharacter::TurnAtRate);

	PlayerInputComponent->BindAxis("LookUp", this, &AMyProjectCharacter::LookUpAtRate);
	

	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMyProjectCharacter::Interact);
	PlayerInputComponent->BindAction("Vent", IE_Released, this, &AMyProjectCharacter::Vent);


	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMyProjectCharacter::Crouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMyProjectCharacter::Uncrouch);


	PlayerInputComponent->BindAction("Pistol", IE_Pressed, this, &AMyProjectCharacter::Pistol);
	PlayerInputComponent->BindAction("Shotgun", IE_Pressed, this, &AMyProjectCharacter::Shotgun);


}

void AMyProjectCharacter::OnFire()
{
	float fluxbuffer = 1.0f;
	if (currentweapon == 2)
	{
		fluxbuffer = 3.3f;
	}

	// try and fire a projectile
	if (ProjectileClass != NULL && flux < fluxcapacity - fluxbuffer && !venting)
	{
		if (currentweapon == 1)
		{
			flux += 1.0f;

			// try and play the sound if specified
			if (FireSound != NULL)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			// try and play a firing animation if specified
			if (FireAnimation != NULL)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != NULL)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}







			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				FVector Loc;
				FRotator Rot;
				FHitResult Hit;

				GetController()->GetPlayerViewPoint(Loc, Rot);

				FVector Start = Loc;
				FVector End = Start + (Rot.Vector() * 20000);

				FCollisionQueryParams TraceParams;
				if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams)) {

					//DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 2.0f);
					//DrawDebugSolidBox(GetWorld(), Hit.ImpactPoint, FVector(2), FColor::Blue, true);


					AEnemyCharacter* EnemyRef = Cast<AEnemyCharacter>(Hit.GetActor());

					int type = 1;

					if (type == 0)
					{
						UGameplayStatics::SpawnEmitterAtLocation(World, BulletTrail, Start, Rot, true);

					}
					else
					{
						//UGameplayStatics::SpawnEmitterAtLocation(World, BulletTrail, FP_Gun->GetComponentLocation(), (FP_Gun->GetComponentLocation()-Hit.Location).GetSafeNormal().Rotation(), true);
						//UGameplayStatics::SpawnEmitterAtLocation(World, BulletTrail, FP_Gun->GetComponentLocation() + FVector(0, 0, 5), (Hit.Location - FP_Gun->GetComponentLocation() - FVector(0, 0, 15)).GetSafeNormal().Rotation(), true);
						UGameplayStatics::SpawnEmitterAtLocation(World, BulletTrail, FP_Gun->GetSocketLocation("MuzzleFlash"), FP_Gun->GetComponentRotation() + FRotator(0, 90, 0), true);

					}


					if (EnemyRef) {
						hit = true;

						if (Hit.BoneName == "head") {

							EnemyRef->hp = EnemyRef->hp - 101;


							UGameplayStatics::SpawnEmitterAtLocation(World, HeadshotEffect, Hit.Location, (Hit.Location - FP_Gun->GetComponentLocation() - FVector(0, 0, 15)).GetSafeNormal().Rotation(), true);





							if (HitSound != NULL) {
								UGameplayStatics::PlaySoundAtLocation(this, HitSound, Hit.Location, 1.0f, 1.0f, 0.3f);
							}





						}
						else {
							EnemyRef->hp = EnemyRef->hp - 35;
							EnemyRef->GetCharacterMovement()->AddImpulse((End - Start).GetSafeNormal() * 40000);
						}


						if (EnemyRef->hp <= 0) {
							EnemyRef->Die((End - Start).GetSafeNormal(), Hit.BoneName);
							//Hit.GetActor()->SetActorLocation(EnemyRef->ostartpos);
						}
					}

					//if (GEngine)
						//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Hit.BoneName.ToString());




				}



			}
		}
		else
		{ 
			flux += 3.3f;

			// try and play the sound if specified
			if (FireSound != NULL)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation(),1.0F,0.1F);
			}

			// try and play a firing animation if specified
			if (FireAnimation != NULL)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != NULL)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}


			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				FVector Loc;
				FRotator Rot;
				FHitResult Hit;

				GetController()->GetPlayerViewPoint(Loc, Rot);

				FVector Start = Loc;
				

				FCollisionQueryParams TraceParams;
				UGameplayStatics::SpawnEmitterAtLocation(World, BulletTrail, FP_Gun->GetSocketLocation("MuzzleFlash"), FP_Gun->GetComponentRotation() + FRotator(0, 90, 0), true);



				for (size_t i = 0; i < 8; i++)
				{
					int test = FMath::FRand() * 100;
					//int test = FMath::FRand() * 2 * PI;
					int test2 = 3 * sqrt(FMath::FRand());

					 FVector End = Start + ((Rot + FRotator(cos(test) * test2, sin(test) * test2, 0)).Vector() * 20000);
					 //FVector End = Start + ((Rot + FRotator(FMath::RandRange(-3, 3), FMath::RandRange(-3, 3),0)).Vector() * 20000);
					 //End += FVector(FMath::RandRange(-100, 100)*100, FMath::RandRange(-100, 100)*100, FMath::RandRange(-100, 100)*100);


					if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams)) {

						//DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 2.0f);
						//DrawDebugSolidBox(GetWorld(), Hit.ImpactPoint, FVector(2), FColor::Blue, true);


						AEnemyCharacter* EnemyRef = Cast<AEnemyCharacter>(Hit.GetActor());




						if (EnemyRef) {
							hit = true;

							if (Hit.BoneName == "head") {

								EnemyRef->hp = EnemyRef->hp - 101;


								UGameplayStatics::SpawnEmitterAtLocation(World, HeadshotEffect, Hit.Location, (Hit.Location - FP_Gun->GetComponentLocation() - FVector(0, 0, 15)).GetSafeNormal().Rotation(), true);





								if (HitSound != NULL) {
									UGameplayStatics::PlaySoundAtLocation(this, HitSound, Hit.Location, 1.0f, 1.0f, 0.3f);
								}





							}
							else {
								EnemyRef->hp = EnemyRef->hp - 35;
								EnemyRef->GetCharacterMovement()->AddImpulse((End - Start).GetSafeNormal() * 40000);
							}


							if (EnemyRef->hp <= 0) {
								EnemyRef->Die((End - Start).GetSafeNormal(), Hit.BoneName);
								//Hit.GetActor()->SetActorLocation(EnemyRef->ostartpos);
							}
						}

						//if (GEngine)
							//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Hit.BoneName.ToString());




					}
				}



			}







		}
	}



}


void AMyProjectCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMyProjectCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMyProjectCharacter::TurnAtRate(float Val)
{
	AddControllerYawInput(Val * MouseSensitivity);
}

void AMyProjectCharacter::LookUpAtRate(float Val)
{
	AddControllerPitchInput(Val * MouseSensitivity);
}

float AMyProjectCharacter::GetHealth()
{
	return hp;
	
}


float AMyProjectCharacter::GetFlux()
{
	return flux / fluxcapacity;

}

bool AMyProjectCharacter::PlayFlash() {
	if (hit)
	{
		hit = false;
		return true;
	}

	return false;
}



void AMyProjectCharacter::Interact() {

	if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "poke");


	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FVector Loc;
		FRotator Rot;
		FHitResult Hit;

		GetController()->GetPlayerViewPoint(Loc, Rot);

		FVector Start = Loc;
		FVector End = Start + (Rot.Vector() * 200);

		FCollisionQueryParams TraceParams;
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams)) {
			


			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "eh");

			
			AButtonTrigger* ButtonRef = Cast<AButtonTrigger>(Hit.GetActor());

			if (ButtonRef)
			{
				ButtonRef->Press();
			}

		}
	}

}


void AMyProjectCharacter::Vent() {
	venting = true;

}

void AMyProjectCharacter::Death() {


	
	if (GetMesh()->bHiddenInGame)
	{
		DetachFromControllerPendingDestroy();
	

		//FirstPersonCameraComponent->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
		//GetMesh()->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
		GetMesh()->SetHiddenInGame(false);
		FirstPersonCameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "head");
		FirstPersonCameraComponent->bUsePawnControlRotation = false;
		FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.f, 0.f));
		FirstPersonCameraComponent->SetRelativeRotation(FRotator(0, 90, 270));

		//FirstPersonCameraComponent->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
		//GetMesh()->SetupAttachment(FirstPersonCameraComponent);
		/* Disable all collision on capsule */
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		SetActorEnableCollision(true);

		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;

		GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AMyProjectCharacter::ARGH, 2);


		
	}

}








void AMyProjectCharacter::ARGH() {
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);

}



void AMyProjectCharacter::Crouch() {
	GetCapsuleComponent()->SetCapsuleHalfHeight(64);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 48.f));
	GetMesh()->SetWorldScale3D(FVector(1, 1, 0.85));
	GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->GetMaxSpeed() / 2;
	ACharacter::Crouch();
}

void AMyProjectCharacter::Uncrouch() {
	GetCapsuleComponent()->SetCapsuleHalfHeight(96);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0, 0, 64.f));
	GetMesh()->SetWorldScale3D(FVector(1, 1, 1));
	GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->GetMaxSpeed() * 2;
	SetActorLocation(GetActorLocation() + FVector(0, 0, 10));
	ACharacter::UnCrouch();

}


void AMyProjectCharacter::Pistol() {
	currentweapon = 1;
}

void AMyProjectCharacter::Shotgun() {
	currentweapon = 2;
}

