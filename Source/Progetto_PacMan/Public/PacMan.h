#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Quat.h"
#include "MovementHandler.h"
#include "Food.h"
#include "PacMan.generated.h"

UCLASS()
class PROGETTO_PACMAN_API APacMan : public APawn, public IMovementHandler
{
	GENERATED_BODY()

public:
	
	APacMan();

protected:
	
	// Game starts
	virtual void BeginPlay() override;

	// ---------GRAFICA------------
	UPROPERTY()
			UStaticMeshComponent* BodyComponent;
		// Componente mesh del corpo del PacMan (principale)

		UPROPERTY()
			TArray<UStaticMeshComponent*> Parts;
		// Array contenente i componenti mesh delle parti del PacMan (per animazione)

	UPROPERTY()
		TArray<FFoodPointer> FoodArray;
	// Array contenente i puntatori agli oggetti di cibo istanziati

	// ------------Variabili per il movimento di PacMan-------------
	// Rotazione corrente del PacMan
	float currentRotation;
	// Ampiezza di rotazione del PacMan (correzione)
	float rotationAmplitude;

public:	

	virtual void Tick(float DeltaTime) override;
	//Funzione per controllare il pawn da input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//Funzione per aggiornare la posizone di pacman e tutte le variabili utilizzate per il movimento contenute nel Variable Storage
	virtual void UpdatePosition() override;

	// Gestione del movimento di PacMan all'interno del gioco su i due assi
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);

	void setFoodArray(TArray<FFoodPointer> foodArray);

	AFood* SearchAndEatFood(FVector2D pos);

	// Flag per indicare quando il gioco è iniziato
	bool StartedGame;

};
