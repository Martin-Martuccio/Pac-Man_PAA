#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VariableStorageHandler.h"
#include "InterfacciaField.h"
#include "InterfacciaFantasmi.h"
#include "GameViewCamera.h"
#include "PacMan.h"
#include "Blinky.h"
#include "Inky.h"
#include "Pinky.h"
#include "Clyde.h"
#include "Food.h"
#include "Wall.h"
#include "GateCasaFantasmi.h"
#include "FieldGenerator.generated.h"


UCLASS()
class PROGETTO_PACMAN_API AFieldGenerator : public AActor, public IInterfacciaField
{
	GENERATED_BODY()
	
public:	
	
	AFieldGenerator();
	//Mappa del campo di gioco
	TArray<FString> Map;
	//Dimensioni della mappa
	FVector2D MapSize;
	// Array di puntatori agli oggetti Food
	TArray<FFoodPointer> FoodArray;

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void Tick(float DeltaTime) override;

	// Riferimento al gestore di variabili di gioco (class VariableStorageHandler)
	UPROPERTY(EditDefaultsOnly)
		AVariableStorageHandler* DatabaseRef;

	//Le variabili TSubclassOf vengono utilizzate per definire riferimenti
	//alle classi derivate da una classe di base specificata.

	// Mi permettono di istanziare oggetti derivanti da queste classi nel gioco
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AGameViewCamera> CameraClass;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<APacMan> PacManClass;

	UPROPERTY(EditDefaultsOnly)
		TArray<TSubclassOf<AActor>> GhostClass;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AFood> FoodClass;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> Empty_TileClass;

	UPROPERTY(EditDefaultsOnly)
		TArray<TSubclassOf<AWall>> WallClass;

	UPROPERTY(EditDefaultsOnly)
		TArray<TSubclassOf<AWall>> DoubleWallClass;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AGateCasaFantasmi> DoorClass;
	// Funzione per generare il campo di gioco in base alla mappa, alle dimensioni
	TArray<AActor*> Generate(TArray<FString> map, FVector2D size);
	// Funzione per rigenerare il campo di gioco in base alla mappa, alle dimensioni, ai personaggi e alla presenza di cibo specificati
	// La creo così da utiizzarla ogni volta che pacman perde una vita
	void ReGenerate(TArray<FString> map, FVector2D size, TArray<AActor*> characters, bool restoreFood);
	// Funzione che mi permette di verificare se una Tile è Walkable o no 
	bool callWalkable(int32 x, int32 y, int32 mode);
	// Funzione che mi determina se una posizione sulla mappa corrisponde alla casa dei fantasmi o no
	bool callGhostHouse(int32 x, int32 y);

};


