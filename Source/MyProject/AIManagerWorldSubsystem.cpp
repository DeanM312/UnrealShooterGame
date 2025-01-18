

#include "AIManagerWorldSubsystem.h"
#include <Runtime\Engine\Classes\Engine\Engine.h>

void UAIManagerWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

}

void UAIManagerWorldSubsystem::Deinitialize()
{

}

void UAIManagerWorldSubsystem::Register(AActor* actor)
{
	friends.Add(actor);
	UE_LOG(LogTemp, Warning, TEXT("I AM ALIVE"));
}

void UAIManagerWorldSubsystem::Unregister(AActor* actor)
{
	friends.Remove(actor);
	UE_LOG(LogTemp, Warning, TEXT("I AM DEAD"));
}