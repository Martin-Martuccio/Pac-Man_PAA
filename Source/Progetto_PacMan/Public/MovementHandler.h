#pragma once

#include "CoreMinimal.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Interface.h"
#include "InterfacciaField.h"
#include "VariableStorageHandler.h"
#include "MovementHandler.generated.h"

UINTERFACE(MinimalAPI)
class UMovementHandler : public UInterface
{
	GENERATED_BODY()
};


class PROGETTO_PACMAN_API IMovementHandler
{
	GENERATED_BODY()

public:

	// Riferimento VariableStorageHandler
	AVariableStorageHandler* DatabasePtr;

	FVector2D RelativePosition(FVector pos);

	void setLocalMap(TArray<FString> lm, FVector2D lms);

	// La velocità di 75.75 pixel/s , corrispondenti rispettivamente a 9.469 tile/s
	const float Velocity100Perc = 9.469; 

	// Mi permette di scalare la velocità
	float Percentual;

	// Mappa locale (passate come riferimento e non come valore)
	FVector2D localMapSize;
	TArray<FString> localMap;

	FVector2D Position;

	// Array di supporto mi identifica le posizioni adiacenti se sono Walkable e Valid
	TArray<bool> posAround;

	void DefinePosAround(int32 mode);
	void UpdatePosAround(int32 mode);

	// Funzione eseguita ogni volta che la posizione relativa alle celle cambia
	virtual void UpdatePosition();

	FVector2D currentVelocity;

	// Funzione per impostare Percentual => Scalare la velocità
	void setPercentual(float perc);

	// Handler del comportamento in prossimità del Teleport
	void TeleportBehavior();

	FVector2D getDirection();

};
