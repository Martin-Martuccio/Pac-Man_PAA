#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Math/Quat.h"
#include "Math/Vector2D.h"
#include "MovementHandler.h"
#include "InterfacciaFantasmi.generated.h"

/*						INTERFACCIA FANTASMI
	Creo una INTERFACCIA per i fantasmi così che ogni fantasma possa 
	ereditare le variabili e le funzioni e fare un override delle funzioni in 
	base le singole specifiche del fantasma stesso
*/

UINTERFACE(MinimalAPI)
class UInterfacciaFantasmi : public UInterface
{
	GENERATED_BODY()
};

class PROGETTO_PACMAN_API IInterfacciaFantasmi
{
	GENERATED_BODY()

public:

	//-----------------------VARIABILI PER GESTIRE IL MOVIMENTO------------------

	// Posizione casa dei fantasmi
	FVector2D HomeTarget;

	// Destinazione
	FVector2D currentTarget;

	// Posizione di spawn
		FVector2D SpawnTarget;

	// Destinazione predefinita
		FVector2D DefaultTarget;

	// Target al di fuori della casa dei fantasmi
		FVector2D NoHomeTarget;

	// Funzione per aggiornare la velocità (e la posizione)
		void UpdateVelocity(bool notBackward);

	// Calcolare la distanza dalla destinazione
		float DistanceFunction(FVector2D pos);

	// Aggiorna la destinazione
		virtual void UpdateTarget();

	// Viene chiamata quando si aggiorna la destinazione in modalità Chase (di default, segue PacMan)
		virtual void UpdateTargetChase();

	//-------------------------Variabili per la gestione della macchina a stati----------------------

	// Stato corrente del fantasma (0:Chase, 1:Scatter, 2:Frightened, 3:StopNellaCasa, 4:RitornoInCasa, 5:UscitaCasa)
	int32 currentState;

	// Aggiorna lo stato del fantasma 
		virtual void UpdateState(int32 state);

	// Ottenere la posizione di PacMan
	FVector2D getPacManPos();

	// Scelta della direzione (la destinazione più vicina restituisce un risultato più piccolo), restituisce la direzione
	// Nota: validDirection definita se (1, 0), (-1, 0), (0, 1) e (0, -1) sono direzioni valide
	FVector2D ChooseFunction(TArray<bool> validDirection);

	// ---------------------------------------------------ANIMAZIONE-----------------------------------------------------
	void MovementAnimation();
	virtual TArray<UMaterialInstanceDynamic*> getDefaultColor();
	void refreshColor(); //utile per la gestione della grafica nei diversi stati

protected:
	void BeginPlayFunction();
	void TickFunction(float DeltaTime);

	// FLAG per tenere traccia se il fantasma può mangiare PacMan
	bool alreadyEaten;

	// Variabile per BLINKY
	int32 actualPercentual;

	// Riferimenti utili per il codice
	AActor* thisAA;
	IMovementHandler* thisIMV;

	// ---------------------------------------------------ANIMAZIONE-------------------------------------------------------
	TArray<UMaterialInstanceDynamic*> DefaultColor;
	TArray<UMaterialInstanceDynamic*> BlueColor;
	UStaticMeshComponent* Skin1;
	UStaticMeshComponent* Skin2;
	UStaticMeshComponent* Eyes;
	UStaticMeshComponent* Mouth;
	UStaticMeshComponent* LeftEyeBall;
	UStaticMeshComponent* RightEyeBall;
	bool ActiveModel;
};
