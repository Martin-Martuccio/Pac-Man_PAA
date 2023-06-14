#include "Blinky.h"

ABlinky::ABlinky()
{

	PrimaryActorTick.bCanEverTick = true;

}

void ABlinky::BeginPlay()
{
	Super::BeginPlay();

	//Riferimento alle skin
	UStaticMeshComponent* _Skin1 = Cast<UStaticMeshComponent>(this->GetDefaultSubobjectByName("Skin1"));
	UStaticMeshComponent* _Skin2 = Cast<UStaticMeshComponent>(this->GetDefaultSubobjectByName("Skin2"));
	_defaultColor = { UMaterialInstanceDynamic::Create(_Skin1->GetMaterial(0), _Skin1), UMaterialInstanceDynamic::Create(_Skin2->GetMaterial(0), _Skin2) };

	BeginPlayFunction();

}

void ABlinky::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickFunction(DeltaTime);

	// Aggiorna la posizione di Blinky nel variable storage
	DatabasePtr->RedGhostPosition = Position;

	// Imposta actualPercentual attuale in base al Settings[i]
	if (DatabasePtr->LeftOverFood <= DatabasePtr->Settings[9])
		actualPercentual = DatabasePtr->Settings[10];

	else if (DatabasePtr->LeftOverFood <= DatabasePtr->Settings[7])
		actualPercentual = DatabasePtr->Settings[8];

	else actualPercentual = DatabasePtr->Settings[5];

}

TArray<UMaterialInstanceDynamic*> ABlinky::getDefaultColor()
{
	return _defaultColor;
}
