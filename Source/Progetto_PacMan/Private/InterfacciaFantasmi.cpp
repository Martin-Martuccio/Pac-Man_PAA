
#include "InterfacciaFantasmi.h"

void IInterfacciaFantasmi::BeginPlayFunction()
{	
	// Ottenere il riferimento all'attore corrente
	thisAA = Cast<AActor>(this);
	// Ottenere il riferimento all'interfaccia di gestione del movimento
	thisIMV = Cast<IMovementHandler>(this);
	
	// Ottenere i riferimenti ai componenti dello scheletro dell'attore
	Skin1 = Cast<UStaticMeshComponent>(thisAA->GetDefaultSubobjectByName("Skin1"));
	Skin2 = Cast<UStaticMeshComponent>(thisAA->GetDefaultSubobjectByName("Skin2"));
	Mouth = Cast<UStaticMeshComponent>(thisAA->GetDefaultSubobjectByName("Mouth"));
	Eyes = Cast<UStaticMeshComponent>(thisAA->GetDefaultSubobjectByName("Eyes"));
	LeftEyeBall = Cast<UStaticMeshComponent>(thisAA->GetDefaultSubobjectByName("LeftEyeBall"));
	RightEyeBall = Cast<UStaticMeshComponent>(thisAA->GetDefaultSubobjectByName("RightEyeBall"));

	// Aggiornare il colore dei fantasmi
	refreshColor();

	// Impostare un timer per chiamare la funzione MovementAnimation ogni 0.2 secondi
	FTimerHandle TimerHandle;
	thisAA->GetWorldTimerManager().SetTimer(TimerHandle, [&]() {MovementAnimation();}, 0.2f, true);

	//DEFAULT value
	actualPercentual = -1;
	alreadyEaten = false;
	ActiveModel = false;
}

void IInterfacciaFantasmi::TickFunction(float DeltaTime)
{
	//Aggiorno la skin dei miei fantasmi
	for (int32 i = 0; i < 2; i++)
		if (!DefaultColor[i]->IsValidLowLevel() || !BlueColor[i]->IsValidLowLevel())
			refreshColor(); 

	// Verifico se i fantasmi possono muoversi
	if (!thisIMV->DatabasePtr->canGhostsMove) {
		UpdateState(currentState);
		thisIMV->currentVelocity = FVector2D(0, 0);
		return;
	}
	// Calcolo la nuova posizione del fantasma
	FVector2D newPosition = FVector2D(FMath::DivideAndRoundNearest((int32)thisAA->GetActorLocation().X, 100), FMath::DivideAndRoundNearest((int32)thisAA->GetActorLocation().Y, 100));

	//Aggiorno la posizione corrente e difinisco il new target
	if (newPosition != thisIMV->Position) {
		thisIMV->Position = newPosition;
		UpdateTarget();
	}

	// Gestisco la posizione dell'attore
	if (currentState != 5 && Cast<IInterfacciaField>(this)->isValidPosition(thisIMV->Position, thisIMV->localMapSize) && !Cast<IInterfacciaField>(this)->isGhostHouse(thisIMV->localMap[thisIMV->Position.Y][thisIMV->Position.X])) {
		// Regola la posizione sull'asse X se la velocità attuale è bassa
		if (FMath::Abs(thisIMV->currentVelocity.X) < 0.2)
			thisAA->SetActorLocation(FVector(thisIMV->Position.X * 100, thisAA->GetActorLocation().Y, 0), false, nullptr, ETeleportType::TeleportPhysics);
		// Regola la posizione sull'asse Y se la velocità attuale è bassa
		if (FMath::Abs(thisIMV->currentVelocity.Y) < 0.2)
			thisAA->SetActorLocation(FVector(thisAA->GetActorLocation().X, thisIMV->Position.Y * 100, 0), false, nullptr, ETeleportType::TeleportPhysics);
	}

	// Muovo il fantasma se la velocità attuale non è zero (SI STA MUOVENDO)
	if (!thisIMV->currentVelocity.IsZero())
	{	
		//Calcolo della nuova posizione in base alla velocità de fantasma
		FVector NewLocation = thisAA->GetActorLocation() + (FVector(thisIMV->currentVelocity.X, thisIMV->currentVelocity.Y, 0) * DeltaTime);
		//Sposto il fantasma
		thisAA->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	else if (thisIMV->DatabasePtr->canGhostsMove) //Se la velocità è zero, ma posso muovermi => aggiorno il target
		UpdateTarget();

	// Verifica se il fantasma sta toccando Pacman
		if (!thisIMV->DatabasePtr->PacManLost && FMath::Abs(thisAA->GetActorLocation().X - thisIMV->DatabasePtr->PacManPosition.X) + FMath::Abs(thisAA->GetActorLocation().Y - thisIMV->DatabasePtr->PacManPosition.Y) < 100) 
	{
		if (thisIMV->DatabasePtr->isPacManPU && currentState == 2 && !alreadyEaten) {
			thisIMV->DatabasePtr->GhostsEaten++;
			UpdateState(4); // ==> FANTASMA RITORNA NELLA CASA
			UpdateTarget(); // AGGIORNO il target
		}
		else if (currentState < 2)    
			thisIMV->DatabasePtr->PacManLost = true;  //PacMan catturato da un fantasma
	}
	
		if (!thisIMV->DatabasePtr->isPacManPU) // TRUE solo se pacman non è in power up mode
		alreadyEaten = false;

}

void IInterfacciaFantasmi::UpdateVelocity(bool notBackward)
{
	// Verifica se la posizione corrente è valida o richiede il comportamento del teletrasporto
	if (Cast<IInterfacciaField>(this)->isValidPosition(thisIMV->Position, thisIMV->localMapSize) && thisIMV->localMap[thisIMV->Position.Y][thisIMV->Position.X] == 'T' ||
		!Cast<IInterfacciaField>(this)->isValidPosition(thisIMV->Position, thisIMV->localMapSize))
		thisIMV->TeleportBehavior(); //TELEPORT (RALLENTAMENTO velocità per i fantasmi)

	// Ottiene la direzione
	if (notBackward) {
		// Se la velocità corrente lungo l'asse X non è zero, imposta a false la posAround (posizione attorno a PacMan)
		if (thisIMV->currentVelocity.X != 0)
			thisIMV->posAround[thisIMV->currentVelocity.X > 0 ? 1 : 0] = false;
		// Se la velocità corrente lungo l'asse Y non è zero, imposta a false la posAround
		if (thisIMV->currentVelocity.Y != 0)
			thisIMV->posAround[thisIMV->currentVelocity.Y > 0 ? 3 : 2] = false;
	}
	// Calcola la nuova velocità utilizzando la funzione ChooseFunction
	thisIMV->currentVelocity = ChooseFunction(thisIMV->posAround) * thisIMV->Percentual * thisIMV->Velocity100Perc;

}

// Restituisce la posizione del Pacman
FVector2D IInterfacciaFantasmi::getPacManPos()
{
	return FVector2D(thisIMV->DatabasePtr->PacManPosition.X, thisIMV->DatabasePtr->PacManPosition.Y) / 100;
}

// Calcola la distanza tra la posizione 'pos' e il target corrente
float IInterfacciaFantasmi::DistanceFunction(FVector2D pos)
{
	return FVector2D::Distance(pos, currentTarget);
}

FVector2D IInterfacciaFantasmi::ChooseFunction(TArray<bool> validDirection)
{

	TArray<float> distances = { 0, 0, 0, 0 };
	TArray<FVector2D> result = { FVector2D(1, 0), FVector2D(-1, 0), FVector2D(0, 1), FVector2D(0, -1) };

	// Calcolo le distanze e lo memorizzo dentro un array
	for (int32 i = 0; i < 4; i++)
		distances[i] = DistanceFunction(Cast<IMovementHandler>(this)->Position + result[i]);

	TArray<float> sortedDistances = TArray<float>(distances);
	sortedDistances.Sort();
	// Trova la direzione valida con la distanza minima
	for (int32 i = 0; i < 4; i++)
		for (int32 j = 0; j < 4; j++)
			if (validDirection[j] && sortedDistances[i] == distances[j])
				return result[j];

	return FVector2D(0, 0);

}

void IInterfacciaFantasmi::UpdateState(int32 state)
{
	// stato corrente
	currentState = state;

	// Aggiorno la velocità 
	UpdateVelocity(false);

	//------------------Animazioni e grafiche---------------------------

	if (currentState == 2) {
		Skin1->SetMaterial(0, BlueColor[0]);
		Skin2->SetMaterial(0, BlueColor[1]);
		Mouth->SetVisibility(true);
	}
	else {
		Skin1->SetMaterial(0, DefaultColor[0]);
		Skin2->SetMaterial(0, DefaultColor[1]);
		Mouth->SetVisibility(false);
	}
	if (currentState == 4) {
		Skin1->SetVisibility(false);
		Skin2->SetVisibility(false);
	}
	else {
		Skin1->SetVisibility(true);
		Skin2->SetVisibility(true);
	}

}

void IInterfacciaFantasmi::UpdateTarget()
{
	// Verifica se il fantasma è all'interno della casa o se la posizione corrente è valida
	bool InsideHouse = currentState >= 3 || Cast<IInterfacciaField>(this)->isValidPosition(thisIMV->Position, thisIMV->localMapSize) &&
											Cast<IInterfacciaField>(this)->isGhostHouse(thisIMV->localMap[thisIMV->Position.Y][thisIMV->Position.X]);
	thisIMV->UpdatePosAround(InsideHouse ? 2 : 1);

	TArray<int32> settings = thisIMV->DatabasePtr->Settings;

	//In base allo stato in cui mi trovo dovrò aggiornare il target dei fantasmini 
	/*
	Come da specifica:

	1. CHASE - L'obiettivo di un fantasma in modalità Chase è trovare e
		catturare Pac-Man dandogli la caccia nel labirinto. 

	2. SCATTER - In modalità scatter, i fantasmi rinunciano all'inseguimento per
		alcuni secondi e si dirigono verso i rispettivi angoli assegnati.

	3. FRIGHTENED - I fantasmi entrano in modalità Frightened ogni volta che
		Pac-Man mangia uno dei quattro pallini energizzanti situati negli angoli
		del labirinto. I fantasmi diventano tutti blu scuro  e vagheranno senza meta nel labirinto
		per alcuni secondi.

	*/
	switch (currentState) {

	case 0:
		// Stato: CHASE
		//Per avere maggior affidibilità sullo stato
		if (thisIMV->DatabasePtr->DefaultGhostState != 0) {
			UpdateState(thisIMV->DatabasePtr->DefaultGhostState);
			UpdateTarget();
			return;
		}

		UpdateTargetChase();
		// Verifico se la posizione è valida
		if (Cast<IInterfacciaField>(this)->isValidPosition(thisIMV->Position, thisIMV->localMapSize) && thisIMV->localMap[thisIMV->Position.Y][thisIMV->Position.X] == 'T')
			thisIMV->Percentual = settings[6];
		
		else
			thisIMV->Percentual = actualPercentual == -1 ? settings[5] : actualPercentual;

		break;

	case 1:
		// Stato: SCATTER
		if (thisIMV->DatabasePtr->DefaultGhostState != 1) {
			UpdateState(thisIMV->DatabasePtr->DefaultGhostState);
			UpdateTarget();
			return;
		}

		currentTarget = DefaultTarget;

		if (Cast<IInterfacciaField>(this)->isValidPosition(thisIMV->Position, thisIMV->localMapSize) && thisIMV->localMap[thisIMV->Position.Y][thisIMV->Position.X] == 'T')
			thisIMV->Percentual = settings[6];
		
		else
			thisIMV->Percentual = actualPercentual == -1 ? settings[5] : actualPercentual;

		break;

	case 2: 

		// Stato: FRIGHTENED
		alreadyEaten = false;

		// Imposta un target casuale
		currentTarget = FVector2D(FMath::RandRange(0, (int32)thisIMV->localMapSize.X - 1), FMath::RandRange(0, (int32)thisIMV->localMapSize.Y - 1));

		if (Cast<IInterfacciaField>(this)->isValidPosition(thisIMV->Position, thisIMV->localMapSize))
			thisIMV->Percentual = settings[13];

		break;

	case 3:
		// Stato: SPAWN
		currentTarget = SpawnTarget;

		if (Cast<IInterfacciaField>(this)->isValidPosition(thisIMV->Position, thisIMV->localMapSize))
			thisIMV->Percentual = 0;

		break;

	case 4:
		// Stato: INSIDE_HOME
		// Se il fantasma è all'interno della casa
		if (HomeTarget == thisIMV->Position) {
			alreadyEaten = true;
			UpdateState(5);
			UpdateTarget();
			return;
		}

		currentTarget = HomeTarget;

		if (Cast<IInterfacciaField>(this)->isValidPosition(thisIMV->Position, thisIMV->localMapSize))
			thisIMV->Percentual = settings[16];

		break;

	case 5:
		// Stato: OUTSIDE_HOME
		if (NoHomeTarget == thisIMV->Position) {
		// Se il fantasma ha raggiunto la posizione di uscita dalla casa
			currentState = thisIMV->DatabasePtr->DefaultGhostState;
			UpdateTarget();
			
			return;
		}

		currentTarget = NoHomeTarget;

		if (Cast<IInterfacciaField>(this)->isValidPosition(thisIMV->Position, thisIMV->localMapSize))
			thisIMV->Percentual = settings[6];

		break;

	default:
		// Per evitare possibili errori
		// Se lo stato corrente non è gestito, torna allo stato di default
		UpdateState(thisIMV->DatabasePtr->DefaultGhostState);

		break;

	}

	UpdateVelocity(true);

	// Animations
	Eyes->SetRelativeLocation(FVector(10 * thisIMV->getDirection().X, 10 * thisIMV->getDirection().Y + (currentState == 2 ? -10 : 0), 0), false, nullptr, ETeleportType::TeleportPhysics);
	LeftEyeBall->SetRelativeLocation(25 * FVector(thisIMV->getDirection().X, thisIMV->getDirection().Y, 0), false, nullptr, ETeleportType::TeleportPhysics);
	RightEyeBall->SetRelativeLocation(LeftEyeBall->GetRelativeLocation(), false, nullptr, ETeleportType::TeleportPhysics);

}

//In CHASE state il target è pacman
void IInterfacciaFantasmi::UpdateTargetChase()
{
	currentTarget = getPacManPos();
}
// Funzione per dare movimento e animazione ai fantasmi
void IInterfacciaFantasmi::MovementAnimation()
{
	// Alterno skin per creare l'effetto di movimento
	Skin1->SetRelativeLocation(FVector(0, 0, ActiveModel ? 0 : -2), false, nullptr, ETeleportType::TeleportPhysics);
	Skin2->SetRelativeLocation(FVector(0, 0, ActiveModel ? -2 : 0), false, nullptr, ETeleportType::TeleportPhysics);
	ActiveModel = !ActiveModel;
	
}

TArray<UMaterialInstanceDynamic*> IInterfacciaFantasmi::getDefaultColor()
{
	return DefaultColor;
}

void IInterfacciaFantasmi::refreshColor()
{
	// Aggiorno il colore dei fantasmi
	DefaultColor = getDefaultColor();
	BlueColor = { UMaterialInstanceDynamic::Create(Skin1->GetMaterial(0), Skin1), UMaterialInstanceDynamic::Create(Skin2->GetMaterial(0), Skin2) };
	
	// Imposta il colore blu per i fantasmi
	for (int32 i = 0; i < 2; i++)
		BlueColor[i]->SetVectorParameterValue("Color", FLinearColor(0.0, 0.0, 0.5, 1.0));
}
