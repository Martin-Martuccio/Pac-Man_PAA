#include "GateCasaFantasmi.h"

AGateCasaFantasmi::AGateCasaFantasmi()
{

	PrimaryActorTick.bCanEverTick = false;

}

// Game starts
void AGateCasaFantasmi::BeginPlay()
{
	Super::BeginPlay();
	//imposto la posizione corretta del gate
	SetActorRotation(FQuat::MakeFromEuler(newRotation), ETeleportType::TeleportPhysics);
	
}

void AGateCasaFantasmi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGateCasaFantasmi::ChangeRotation(FVector r) {
	newRotation = r;
	SetActorRotation(FQuat::MakeFromEuler(newRotation), ETeleportType::TeleportPhysics);
}

