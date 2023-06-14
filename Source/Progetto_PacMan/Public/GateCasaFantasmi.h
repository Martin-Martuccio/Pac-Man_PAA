#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GateCasaFantasmi.generated.h"

UCLASS()
class PROGETTO_PACMAN_API AGateCasaFantasmi : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AGateCasaFantasmi();
	
	void ChangeRotation(FVector r);

protected:
	
	virtual void BeginPlay() override;
	// Variabile per la nuova rotazione
	FVector newRotation = FVector(0, 0, 0);

public:	
	
	virtual void Tick(float DeltaTime) override;

};
