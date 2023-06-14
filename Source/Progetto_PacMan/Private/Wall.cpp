#include "Wall.h"

AWall::AWall()
{
	// tick counter non serve
	PrimaryActorTick.bCanEverTick = false;

}

void AWall::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWall::ChangeRotation(FVector rotation) {
	
	// Cambia la rotazione del muro utilizzando la funzione SetActorRotation
	// La rotazione viene specificata come parametro passato 
	// successivamente viene convertita in un'istanza di FQuat utilizzando la funzione MakeFromEuler
	SetActorRotation(FQuat::MakeFromEuler(rotation), ETeleportType::TeleportPhysics);
	
}