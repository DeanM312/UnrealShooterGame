
#include "EnemyAI.h"
#include <Runtime\Engine\Public\DrawDebugHelpers.h>
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "MyProjectCharacter.h"
#include "EnemyCharacter.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyAI::AEnemyAI()
{
	/*
	ConstructorHelpers::FObjectFinder<USoundBase> FIRE(TEXT("/Game/Sounds/M4A1"));
	FireSound = FIRE.Object;

	ConstructorHelpers::FObjectFinder<USoundBase> ATTACK(TEXT("/Game/Sounds/attacking"));
	AttackLine = ATTACK.Object;

	ConstructorHelpers::FObjectFinder<USoundBase> RANDOM(TEXT("/Game/Sounds/random"));
	RandomLine = RANDOM.Object;
	
	ConstructorHelpers::FObjectFinder<USoundBase> GENERAL(TEXT("/Game/Sounds/Lines/generalnew"));
	GeneralLine = GENERAL.Object;
	*/

	ConstructorHelpers::FObjectFinder<USoundBase> FIRE(TEXT("/Game/Sounds/M4A1"));
	FireSound = FIRE.Object;

	ConstructorHelpers::FObjectFinder<USoundBase> ATTACK(TEXT("/Game/Sounds/Lines/attackingnew"));
	AttackLine = ATTACK.Object;


	ConstructorHelpers::FObjectFinder<USoundBase> GENERAL(TEXT("/Game/Sounds/Lines/generalnew"));
	GeneralLine = GENERAL.Object;

	ConstructorHelpers::FObjectFinder<USoundBase> RANDOM(TEXT("/Game/Sounds/Lines/randomnew"));
	ConstructorHelpers::FObjectFinder<USoundBase> RANDOM2(TEXT("/Game/Sounds/Lines/randomnew2"));
	ConstructorHelpers::FObjectFinder<USoundBase> RANDOM3(TEXT("/Game/Sounds/Lines/randomnew3"));
	RandomLines.Add(RANDOM.Object);
	RandomLines.Add(RANDOM2.Object);
	RandomLines.Add(RANDOM3.Object);
	RandomLines.Add(GENERAL.Object);



	ostartpos = FVector(0, 0, 0);

	currentrotation = FRotator(0, 0, 0);

	aimaccel = 0.f;

	playerpos = FVector(99999, 99999, 99999);

	tick = 0;

	reacted = false;


}

void AEnemyAI::BeginPlay() {

	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AEnemyAI::RepeatingFunction, 3, true, 0);

	startpos = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyAI::StaticClass(), friends);

	elite = Cast<AEnemyCharacter>(GetPawn())->elite;
}

void AEnemyAI::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	cpos = GetPawn()->GetActorLocation() + FVector(0.0f, 0.0f, 70.0f);
	FRotator targetrotation = (playerpos - cpos).GetSafeNormal().Rotation();


	float aimspeed = 100.f;
	


	if (abs(currentrotation.Pitch - targetrotation.Pitch) < 2) {
		if (abs(currentrotation.Yaw - targetrotation.Yaw) < 2) {
			aimspeed = 10.f;
		}
	}



	tick++;
	if (tick > 2)
	{
		tick = 0;
	}

	if (dist < 4000)
	{
	
		//SIGHTTTTT
		if (GetWorld()->GetFirstPlayerController()->GetPawn())
		{
			FHitResult Hit;
			FVector ppos = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation() + FVector(0, 0, 30 * tick);
			FCollisionQueryParams TraceParams;


			if (GetWorld()->LineTraceSingleByChannel(Hit, cpos, ppos + FVector(0, 0, 40), ECC_Destructible, TraceParams)) {

				if (Hit.BoneName != "None")
				{
					//aimaccel = aimaccel + 3 * DeltaTime;


					for (int i = 0; i < friends.Num(); i++)
					{
						if (friends[i]->IsValidLowLevel())
						{
							AEnemyAI* Friend = Cast<AEnemyAI>(friends[i]);

							if (Friend)
							{
								Friend->playerpos = ppos;
							}
						}
					}
			




					//if (GEngine)
						//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, playerpos.ToString());
				}
				else
				{
					reacted = false;
				}

				//DrawDebugLine(GetWorld(), cpos, ppos, FColor::Orange, false, 2.0f);
			}



		}
		
	

		currentrotation = FMath::RInterpTo(currentrotation, targetrotation, DeltaTime, aimspeed + aimaccel);



		SetFocalPoint(currentrotation.Vector() * 10000);

		float headshotoff = (playerpos + FVector(0, 0, 40) - cpos).GetSafeNormal().Rotation().Pitch - currentrotation.Pitch;

		//playerpos.Z = playerpos.Z + 70;


		//if (GEngine)
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, playerpos.ToString());



		//GetPawn()->GetController()->SetControlRotation(currentrotation);


		//if (GEngine)
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, currentrotation.ToString());


		//DrawDebugLine(GetWorld(), cpos, currentfocus, FColor::Orange, false, 2.0f);





		if (abs(currentrotation.Pitch - targetrotation.Pitch) < 2) {
			if (abs(currentrotation.Yaw - targetrotation.Yaw) < 2) {




				//if (GEngine)
					//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(aimaccel));




				UWorld* const World = GetWorld();
				if (World != NULL)
				{

					FHitResult Hit;



					FVector Start = cpos;






					FCollisionQueryParams TraceParams;
					for (int i = 0; i < 2; i++)
					{

						FVector End = Start + (targetrotation + FRotator(headshotoff * i, 0, 0)).Vector() * 100000;









						//DrawDebugLine(GetWorld(), cpos, End, FColor::Orange, false, 2.0f);


						//if (GEngine)
							//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, currentrotation.ToString());


						if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Destructible, TraceParams)) {




							if ((Hit.BoneName != "None") && (shot == 0)) {


								

								Delegate.BindUFunction(this, "TakeTheShot", headshotoff * i);



								float rantime = 0;

								if (reacted == true)
								{
									rantime = 0.3f;
									if (elite)
									{
										rantime = 0.05f;
									}

								}
								else
								{
									rantime = FMath::RandRange(0.25f, 0.3f);
									
								}

								GetWorldTimerManager().SetTimer(ShotsHandle, Delegate, rantime, false);






								GetWorldTimerManager().SetTimer(PeakHandle, this, &AEnemyAI::Peak, FMath::RandRange(0.25f, 0.3f), false, rantime);

								shot = 1;

								if (AttackLine != NULL && FMath::RandRange(0, 50) == 0) {
									UGameplayStatics::PlaySoundAtLocation(this, AttackLine, GetPawn()->GetActorLocation());
								}



								break;









							}






						}
					}



				}


			}


		}
	}



	
}



void AEnemyAI::RepeatingFunction() {




	//dist = GetPawn()->GetDistanceTo(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		dist = FVector::Dist(ostartpos, GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation());
	}

	//if (GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(dist));

	if (dist < 4000)
	{
		

		int action = FMath::RandRange(0, 3);

		if (action >= 2) {

			FVector ranloc = UNavigationSystemV1::GetRandomReachablePointInRadius(GetWorld(), cpos, 1000);


			MoveToLocation(ranloc);

			if (RandomLines.Num() > 0 && FMath::RandRange(0,50) == 0) {
				UGameplayStatics::PlaySoundAtLocation(this, RandomLines[FMath::RandRange(0, RandomLines.Num()-1)], GetPawn()->GetActorLocation());
			}


		}
		else if (action == 1) {
			Peak();
		}

		else {
			if (GeneralLine != NULL && FMath::RandRange(0, 50) == 0) {
				UGameplayStatics::PlaySoundAtLocation(this, GeneralLine, GetPawn()->GetActorLocation());
			}

			if (GetWorld()->GetFirstPlayerController()->GetPawn())
			{

				MoveToLocation(playerpos);
			}

		}
	}
	else
	{
		MoveToLocation(ostartpos);
	}


} 


void AEnemyAI::TakeTheShot(float offset) {


	//if (GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(offset));


	shot = 0;
	reacted = true;

	if (FireSound != NULL) {
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetPawn()->GetActorLocation());
	}
	


	//MoveToLocation(GetPawn()->GetActorLocation());
	

	UWorld* const World = GetWorld();
	if (World != NULL)
	{

		FHitResult Hit;



		FVector Start = cpos;
		FVector End = Start + (currentrotation + FRotator(offset, 0, 0)).Vector() * 100000;



		FCollisionQueryParams TraceParams;
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Destructible, TraceParams)) {



			//DrawDebugLine(GetWorld(), Start, test, FColor::Orange, false, 2.0f);
			//DrawDebugSolidBox(GetWorld(), Hit.ImpactPoint, FVector(2), FColor::Blue, true);

			AEnemyCharacter* SelfRef = Cast<AEnemyCharacter>(GetPawn());
			SelfRef->Fire(Hit.Location);

			if (Hit.BoneName != "None") {

				aimaccel = 0.f;
				AMyProjectCharacter* EnemyRef = Cast<AMyProjectCharacter>(Hit.GetActor());

				if (EnemyRef)
				{
					EnemyRef->hp = EnemyRef->hp - 35;
					EnemyRef->GetCharacterMovement()->AddImpulse(currentrotation.GetNormalized().Vector() * 40000);
					

					

					

				}


				



			}







		}



	}

}



void AEnemyAI::Peak() {

	


	FVector enemypos = GetPawn()->GetActorLocation();
	float xdif = enemypos.X - playerpos.X;
	float ydif = enemypos.Y - playerpos.Y;

	int dir = FMath::RandRange(0, 1);

	float x2;
	float y2;

	if (dir == 1) {
		x2 = enemypos.X + ydif / 8;
		y2 = enemypos.Y - xdif / 8;
	}

	else {
		x2 = enemypos.X - ydif / 8;
		y2 = enemypos.Y + xdif / 8;

	}

	


	//DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), FVector(x2, y2, GetPawn()->GetActorLocation().Z), FColor::Orange, false, 20.0f);
	

	MoveToLocation(FVector(x2, y2, GetPawn()->GetActorLocation().Z));

}


