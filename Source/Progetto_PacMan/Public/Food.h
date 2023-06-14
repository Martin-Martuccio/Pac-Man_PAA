#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Food.generated.h"

UCLASS()
class PROGETTO_PACMAN_API AFood : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AFood();
	// Funzione chiamata quando Pac-Man mangia l'oggetto di cibo
	void PacManEat();
	// Imposta se l'oggetto di cibo è un power-up
	void setPU(bool b);

protected:
	
	virtual void BeginPlay() override;

	// Flag per i power up
	bool PU;

	UPROPERTY()
		UStaticMeshComponent* _food;

	FTimerHandle TimerHandle_Blink;

public:	
	
	virtual void Tick(float DeltaTime) override;

};