#include "Food.h"

AFood::AFood()
{
	// tick counter non mi serve
	PrimaryActorTick.bCanEverTick = false;
	PU = false;

}

void AFood::BeginPlay()
{
	Super::BeginPlay();
	// Ottiene il componente UStaticMeshComponent di nome "Food"
	_food = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName("Food"));

}

void AFood::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// NULLA
}

void AFood::setPU(bool b) {

	PU = b;

	if (PU) {
		
		// Cambia la scala dell'oggetto di cibo
		_food->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
		
		// Attiva l'animazione di 'lampeggio'
		GetWorldTimerManager().SetTimer(TimerHandle_Blink, [&]() { _food->SetVisibility(!_food->IsVisible()); }, 0.15f, true);
	}
	else {
		
		// Cambia la scala dell'oggetto di cibo
		_food->SetRelativeScale3D(FVector(0.3, 0.3, 1.0));

		// Disattiva l'animazione di 'lampeggio'
		if (GetWorldTimerManager().IsTimerActive(TimerHandle_Blink))
			GetWorldTimerManager().ClearTimer(TimerHandle_Blink);
		_food->SetVisibility(true);
	}

}

void AFood::PacManEat()
{
	// Distrugge l'attore dell'oggetto di cibo quando pacman lo mangia
	Destroy();
}
