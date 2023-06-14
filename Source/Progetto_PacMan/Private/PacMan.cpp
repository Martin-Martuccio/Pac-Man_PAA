#include "PacMan.h"
#include "GameFramework/Actor.h"


APacMan::APacMan()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	// Percentuale di velocità di PacMan rispetto la velocità di base
	Percentual = 80;
	// TRUE ==> il gioco è iniziato
	StartedGame = true;

}

// Game starts
void APacMan::BeginPlay()
{
	Super::BeginPlay();
	//Imposta il giocatore controllato da questo attore come Player0
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Riferimenti alle StaticMeshComponent (Corpo e le varie parti)
	BodyComponent = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName("Body"));
	Parts = { Cast<UStaticMeshComponent>(GetDefaultSubobjectByName("Part1")), Cast<UStaticMeshComponent>(GetDefaultSubobjectByName("Part2")) };
	
	// Gestione delle rotazioni per le animazioni
	currentRotation = 0;
	rotationAmplitude = 12;
	// Definisce le posizioni attorno a PacMan
	DefinePosAround(1);
	
}

void APacMan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!DatabasePtr->canPacManMove) {
		// Se PacMan non può muoversi, la sua velocità è 0
		currentVelocity = FVector2D(0, 0);
		return;
	}

	else if (StartedGame) {
		StartedGame = false;
		// Imposta la velocità iniziale
		currentVelocity.X = Percentual * Velocity100Perc;
		// Resetta la rotazione del corpo di PacMan
		BodyComponent->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 0)), false, nullptr, ETeleportType::TeleportPhysics);
	}

	// Calcolo della nuova posizione di PacMan
	FVector2D newPosition = FVector2D(FMath::DivideAndRoundNearest((int32)GetActorLocation().X, 100), FMath::DivideAndRoundNearest((int32)GetActorLocation().Y, 100));

	// Update della posizione ad ogni Tick
	if (newPosition != Position) {
		Position = newPosition;
		UpdatePosition(); 
	}

	// Se la velocità è diversa da 0 e non può muoversi nella direzione corrente,
	// la velocità viene impostata a 0 (Check eseguito sia su asse X che su asse Y)

	if (currentVelocity.X != 0 && !posAround[currentVelocity.X > 0 ? 0 : 1])
		currentVelocity.X = 0;
	
	if (currentVelocity.Y != 0 && !posAround[currentVelocity.Y > 0 ? 2 : 3])
		currentVelocity.Y = 0;

	// Spostamenti lungo l'asse X
	if (FMath::Abs(currentVelocity.X) < 0.2) {
		
		if (Position.X < 0) {
			posAround[0] = true;
			Move_XAxis(1);
		}
		
		else if (Position.X >= localMapSize.X) {
			posAround[1] = true;
			Move_XAxis(-1);
		}
		
		// Imposto il pawn lungo l'asse X
		SetActorLocation(FVector(Position.X * 100, GetActorLocation().Y, 0), false, nullptr, ETeleportType::TeleportPhysics);
	}
	// Spostamenti lungo l'asse Y
	if (FMath::Abs(currentVelocity.Y) < 0.2) {
	
		if (Position.Y < 0) {
			posAround[2] = true;
			Move_YAxis(1);
		}
		
		else if (Position.Y >= localMapSize.Y) {
			posAround[3] = true;
			Move_YAxis(-1);
		}
		
		// Imposto il pawn lungo l'asse X
		SetActorLocation(FVector(GetActorLocation().X, Position.Y * 100, 0), false, nullptr, ETeleportType::TeleportPhysics);
	}

	if (!currentVelocity.IsZero()) // Se il pawn è in movimento 
	{
		// Calcolo della nuova posizione tenendo in considerazione la velocità
		FVector NewLocation = GetActorLocation() + (FVector(currentVelocity.X, currentVelocity.Y, 0) * DeltaTime);
		SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

		//Animazione => rotazione del pawn nel momento in cui cambia direzione
		currentRotation += rotationAmplitude;
		
		if (currentRotation >= 60 || currentRotation <= 0)
			rotationAmplitude *= -1;
		
		FVector rotation = FVector(0, 0, FMath::Clamp(currentRotation, 0, 60));
		Parts[0]->SetRelativeRotation(FQuat::MakeFromEuler(rotation), false, nullptr, ETeleportType::TeleportPhysics);
		Parts[1]->SetRelativeRotation(FQuat::MakeFromEuler(-rotation), false, nullptr, ETeleportType::TeleportPhysics);
		
	}
	// Aggiorna la posizione e la direzione nel Variable Storage 
	DatabasePtr->PacManPosition = GetActorLocation();
	DatabasePtr->PacManDirection = getDirection();

}

// Controllo del movimento del pawn tramite UE function (input => (W-A-S-D) )
void APacMan::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("MoveX", this, &APacMan::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &APacMan::Move_YAxis);

}
// MOVIMENTO LUNGO ASSE ORIZZONTALE
void APacMan::Move_XAxis(float AxisValue)
{
	if (FMath::Abs(GetActorLocation().Y - 100 * Position.Y) > 30 || !DatabasePtr->canPacManMove)
		return;

	if (AxisValue > 0 && posAround[0]) {
		// Imposto la velocità X 
		currentVelocity.X = Percentual * Velocity100Perc;
		// Velocità verticale = 0
		currentVelocity.Y = 0;
		// Resetto la rotazione (DX)
		BodyComponent->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 0)), false, nullptr, ETeleportType::TeleportPhysics);
	}

	else if (AxisValue < 0 && posAround[1]) {
		// Imposto la velocità X (verso opposto rispetto a prima)
		currentVelocity.X = -Percentual * Velocity100Perc;
		// Velocità verticale = 0
		currentVelocity.Y = 0;
		// Rotazione del corpo di 180° (SX)
		BodyComponent->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 180)), false, nullptr, ETeleportType::TeleportPhysics);
	}

}
// MOVIMENTO LUNGO ASSE VERTICALE
void APacMan::Move_YAxis(float AxisValue)
{
	if (FMath::Abs(GetActorLocation().X - 100 * Position.X) > 30 || !DatabasePtr->canPacManMove)

		return;

	if (AxisValue > 0 && posAround[2]) {
		// Imposto la velocità Y 
		currentVelocity.Y = Percentual * Velocity100Perc;
		// Velocità orizzontale = 0
		currentVelocity.X = 0;
		// Rotazione del corpo di 90° (SU)
		BodyComponent->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 90)), false, nullptr, ETeleportType::TeleportPhysics);
	}

	else if (AxisValue < 0 && posAround[3]) {
		// Imposto la velocità Y (verso opposto rispetto a prima)
		currentVelocity.Y = -Percentual * Velocity100Perc;
		// Velocità orizzontale = 0
		currentVelocity.X = 0;
		// Rotazione del corpo di -90° (GIU)
		BodyComponent->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, -90)), false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void APacMan::setFoodArray(TArray<FFoodPointer> foodArray) {

	FoodArray = TArray<FFoodPointer>(foodArray);

}

AFood* APacMan::SearchAndEatFood(FVector2D pos) {
	// Cerca il cibo nell'array FoodArray con la posizione specificata
	for (int32 i = 0; i < FoodArray.Num(); i++)

		if (FoodArray[i].pos == pos) {
			// Memorizza il riferimento in una variabile temp
			AFood* temp = FoodArray[i].ref;
			// Rimuovo l'elemento dall'Array
			FoodArray.RemoveAt(i);
			//Update delle variabili nello storage
			DatabasePtr->LeftOverFood--;
			Percentual = DatabasePtr->Settings[DatabasePtr->isPacManPU ? 12 : 4];

			return temp; // ritorno il rierimento al cibo trovato
		}

	return nullptr; // se non trovo nula ==> ritorno nullptr
}

void APacMan::UpdatePosition() {
	// Aggiorna la lista delle posizioni intorno a PacMan
	UpdatePosAround(1); 
	// Aggiorna il valore di Percentual ( IF pacman eat Power-UP => increase velocità)
	Percentual = DatabasePtr->Settings[DatabasePtr->isPacManPU ? 11 : 3];
	// Check su localMap
	if (localMap.Num() == 0)
		return;
	// Ottiene il carattere nella posizione corrente di PacMan nella mappa locale
	switch (IInterfacciaField::isValidPosition(Position, localMapSize) ? localMap[Position.Y][Position.X] : 'T') {
	// Caso : Cibo
	case 'o':
	{
		// Cerca e mangia il cibo nella posizione corrente
		AFood* food = SearchAndEatFood(Position);
		if (food != nullptr)
			food->PacManEat();

		break;

	}
	// Caso: Power-Up
	case '@':
	{

		AFood* food = SearchAndEatFood(Position);
		
		if (food != nullptr) {
			// Imposta il flag PU nello storage a true
			DatabasePtr->PU = true;
			food->PacManEat();
		}

		break;

	}
	// Caso: Teleport
	case 'T':

		TeleportBehavior();

		break;

	default:

		break;

	}
}