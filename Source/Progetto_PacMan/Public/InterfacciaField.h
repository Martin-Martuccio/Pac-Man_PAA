// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Food.h"
#include "InterfacciaField.generated.h"

USTRUCT() //costruiscon una struct utile per la gestione dei food tramite puntatore
struct FFoodPointer
{
	GENERATED_BODY()

public:

	//costruttore senza parametri
	FFoodPointer() {
		ref = nullptr;
		pos = FVector2D(0, 0);
	}
	//con parametri
	FFoodPointer(AFood* _ref, FVector2D _pos) {
		ref = _ref;
		pos = _pos;
	}

	AFood* ref;
	FVector2D pos;

};


UINTERFACE(MinimalAPI)
class UInterfacciaField : public UInterface
{
	GENERATED_BODY()
};

class PROGETTO_PACMAN_API IInterfacciaField
{
	GENERATED_BODY()

public:

	//Mi definisco delle funzioni che sono utili per la gestione del mio campo di gioco
	//Verifica della validità della posizione
	static bool isValidPosition(FVector2D position, FVector2D MaxLimit);
	static bool isValidPosition(int32 x, int32 y, FVector2D MaxLimit);
	//Verifica della possibilità di camminarci sopra (MODE mi permette di distingure la Walkability in base al character 0:code , 1:pacman , 2: fantasmi)
	static bool isWalkable(char tile, int32 mode);
	//Data una tile posso sapere se apaprtiene alla casa del fantasmi
	static bool isGhostHouse(char tile);

};
