#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "VariableStorageHandler.h"
#include "FieldGenerator.h"
#include "MyGameMode.generated.h"

// La GameMode gestisce la logica di gioco e le variabili di stato

UCLASS()
class PROGETTO_PACMAN_API AMyGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//Riferimento al mio DataBase contente tutte le variabili utili del gioco
	UPROPERTY()
		AVariableStorageHandler* Database;

	//Riferimento al generatore del campo di gioco
	UPROPERTY()
		AFieldGenerator* Map;

	//Flag per l'inizio del gioco
	bool GameNotStarted;

	// Punteggio del gioco
	int32 Score;
	int32 HighScore;
	
	// Timer per il tempo di gioco
	FTimerHandle GlobalTimerHandle;
	int32 GameTime;

	// Array di attori che rappresentano i personaggi nel gioco
	UPROPERTY()
		TArray<AActor*> Characters;

	// Timer per la modalità dei fantasmini
	FTimerHandle TimerHandle_GhostMode;
	FTimerHandle TimerHandle_GhostFrightened;
	
	// Flag per indicare se è avvenuto un cambio di modalità dei fantasmi
	bool hasChangedMode;

	// Contatori di stato per il gioco
	int32 StateCounter;
	int32 localPoint;	
	int32 localTotalFood;
	int32 localLeftOverFood;
	int32 localGhostsEaten;

	void UseFile(bool ReadMode);

	// Funzione per ottenere il punteggio attuale
	UFUNCTION(BlueprintCallable)
		int32 getScore();

	// Funzione richiamabile da blueprint per ottenere il punteggio più alto
	UFUNCTION(BlueprintCallable)
		int32 getHighScore();

	// Funzione per ottenere le vite rimaste di PacMan
	UFUNCTION(BlueprintCallable)
		int32 getPacManLives();
	
};

