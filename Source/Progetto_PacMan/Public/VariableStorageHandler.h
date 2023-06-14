#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VariableStorageHandler.generated.h"

/*
	Definisco questa classe per memorizzare tutte le variabili utili che mi servono
	sia per la crezione delle istanze iniziali, che per tenere traccia durante 
	l'esecuzione di tutte quelle informazioni utili per la fruizione del game
*/


UCLASS()
class PROGETTO_PACMAN_API AVariableStorageHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AVariableStorageHandler();

	// Dimensione della mappa
	FVector2D MapSize;
	// Mappa di gioco
	TArray<FString> Map;
	// Impostazioni di gioco
	TArray<int32> Settings;
	// Timer per la modalità dei fantasmi
	TArray<float> GhostModeTimers;
	// Timer per liberare i fantasmi
	TArray<float> GhostFree;

	bool LoadAll();
	// Flag per consentire il movimento di PacMan
	bool canPacManMove;
	// Flag per consentire il movimento dei fantasmi
	bool canGhostsMove;
	// Stato predefinito dei fantasmi
	int32 DefaultGhostState;
	// Flag per indicare se PacMan ha preso un potenziamento (power up)
	bool isPacManPU;

	// Posizione di PacMan
	FVector PacManPosition;
	// Direzione di PacMan
	FVector2D PacManDirection;
	// Vite rimanenti di Pac-Man
	int32 LivesLeft;
	// Cibo rimanente sulla mappa
	int32 LeftOverFood;
	// Flag per indicare se è presente un potenziamento
	bool PU;

	// Posizione del fantasma rosso
	FVector2D RedGhostPosition;
	// Flag per indicare se PacMan ha perso
	bool PacManLost;
	// Numero di fantasmi mangiati da PacMan
	int32 GhostsEaten;

	UFUNCTION(BlueprintCallable)
		int32 GetIndexSprite();

protected:
	
	// Game starts
	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

};