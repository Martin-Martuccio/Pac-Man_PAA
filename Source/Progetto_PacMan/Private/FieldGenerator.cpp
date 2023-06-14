#include "FieldGenerator.h"
#include "Math/Quat.h"
#include "Kismet/GameplayStatics.h"

AFieldGenerator::AFieldGenerator()
{

	PrimaryActorTick.bCanEverTick = false;

}

void AFieldGenerator::BeginPlay()
{
	Super::BeginPlay();

	//Riferimento DB
	TArray<AActor*>temp = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVariableStorageHandler::StaticClass(), temp);
	
	// Recupera il riferimento al VariableStorageHandler
	if (temp.IsValidIndex(0))
		DatabaseRef = Cast<AVariableStorageHandler>(temp[0]);
	// handle error
	else {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Ricarica il gioco, errore nel recupero del DB"));
		return;
	}

}

void AFieldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// Generazione del campo di gioco
TArray<AActor*> AFieldGenerator::Generate(TArray<FString> map, FVector2D size) {

	//Definisco la mappa e la dimensione passati come parametri alla funzione
	Map = TArray<FString>(map);
	MapSize = FVector2D(size);

	/*			Il puntatore ai personaggi è istanziato in questo modo:
								(indice : personaggio)
									 0  : Pacman
									 1  : Blinky
									 2  : Inky
									 3  : Pinky
									 4  : Clyde
	*/
	TArray<AActor*> Characters = { nullptr, nullptr, nullptr, nullptr, nullptr };
	TArray<FVector2D> DefaultTargets = { FVector2D(0, 0), FVector2D(0, 0), FVector2D(0, 0), FVector2D(0, 0) };
	TArray<FVector2D> HomeTargets = { FVector2D(0, 0), FVector2D(0, 0) };

	FoodArray = TArray<FFoodPointer>();

	// Posizione della camera
	TArray<AActor*> camera;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), CameraClass, camera);
	FVector cameraLocation = 100 * FVector(MapSize.X / 2, MapSize.Y * 0.47, MapSize.Y / 0.89);
	
	// Imposta la posizione e la rotazione della telecamera se esiste
	if (camera.IsValidIndex(0))
		Cast<AGameViewCamera>(camera[0])->setPositionAndRotation(cameraLocation, FVector(0, -90, -90));
	
	else // la spawno
		Cast<AGameViewCamera>(GetWorld()->SpawnActor(CameraClass, &cameraLocation))->setRotation(FVector(0, -90, -90));

	//doppio ciclo per la creazione della mappa
	for (int32 y = 0; y < MapSize.Y; y++)
		for (int32 x = 0; x < MapSize.X; x++) {

			//dimensinamento della locazione in base alle cordinate spaziali
			FVector Location = 100 * FVector(x, y, 0);
			
			switch (Map[y][x]) {
			
			// in base al tipo i carattere che trovo => spawno un Actor specifico

			case 'o':
			case '@':
			{

				AFood* food = Cast<AFood>(GetWorld()->SpawnActor(FoodClass, &Location));
				food->setPU(Map[y][x] == '@'); //Se power point => PU = TRUE
				// Aggiunge l'istanza dell'attore e le sue coordinate alla FoodArray
				FoodArray.Add(FFoodPointer(food, FVector2D(x, y)));

				break;

			}
			
			// // Caso '-' o '=' (Wall)
			case '-':
			case '=':
			{
				// Variabile che indica se la parete è singola o doppia
				bool singleW = Map[y][x] == '-';
				// Array che memorizza se le pareti adiacenti sono presenti
				TArray<bool> side = { y + 1 < MapSize.Y && (Map[y + 1][x] == '-' || Map[y + 1][x] == '='),
									  y > 0				&& (Map[y - 1][x] == '-' || Map[y - 1][x] == '='),
									  x + 1 < MapSize.X && (Map[y][x + 1] == '-' || Map[y][x + 1] == '='),
									  x > 0				&& (Map[y][x - 1] == '-' || Map[y][x - 1] == '=') };
				// Contatore per contare quante pareti adiacenti sono presenti
				int32 Counter = 0;
				for (int32 k = 0; k < 4; k++)
					Counter += side[k] ? 1 : 0;
				// Gestisce i diversi casi in base al valore di Counter
				switch (Counter) {
				// Caso con una sola parete adiacente
				case 1:
				{

					FVector Rotation;
					
					bool isDoor = false;

					
					if (side[0]) {
					
						if (y > 0 && Map[y - 1][x] == '+')
							isDoor = true;
						Rotation = FVector((!callWalkable(x - 1, y, 0) || callGhostHouse(x - 1, y) ? 0 : 180), 180, 90);
					}
					
					else if (side[1]) {
						
						if (y + 1 < MapSize.Y && Map[y + 1][x] == '+')
							isDoor = true;
						Rotation = FVector((!callWalkable(x + 1, y, 0) || callGhostHouse(x + 1, y) ? 0 : 180), 180, -90);
					}
					
					else if (side[2]) {
					
						if (x > 0 && Map[y][x - 1] == '+')
							isDoor = true;
						Rotation = FVector((!callWalkable(x, y - 1, 0) || callGhostHouse(x, y - 1) ? 0 : 180), 0, 180);
					}
					
					else {
					
						if (x + 1 < MapSize.X && Map[y][x + 1] == '+')
							isDoor = true;
						Rotation = FVector((!callWalkable(x, y + 1, 0) || callGhostHouse(x, y + 1) ? 0 : 180), 0, 0);
					}
					
					AWall* wall = Cast<AWall>(GetWorld()->SpawnActor(isDoor ? DoubleWallClass[5] : DoubleWallClass[0], &Location));
					wall->ChangeRotation(Rotation);

					break;
				}

				case 2:
				case 3:

					if ((side[0] && side[1]) ^ (side[2] && side[3])) {

						AWall* wall;
						int32 Index;

						if (callWalkable(x + 1, y, 0) || callWalkable(x - 1, y, 0) || callWalkable(x, y - 1, 0) || callWalkable(x, y + 1, 0))
							Index = 0;
						
						else
							Index = 3;

						wall = Cast<AWall>(GetWorld()->SpawnActor(singleW && Index == 0 ? WallClass[0] : DoubleWallClass[Index], &Location));

						if (Index == 0)
						
							if (side[0] && side[1])
								wall->ChangeRotation(FVector((side[3] || callWalkable(x + 1, y, 0) ? 0 : 180), 0, 90));
							else
								wall->ChangeRotation(FVector((side[0] || callWalkable(x, y - 1, 0) ? 0 : 180), 0, 0));
						else {
						
							if (y + 1 < MapSize.Y && Map[y + 1][x] == '-')
								wall->ChangeRotation(FVector(callWalkable(x + 1, y + 1, 0) ? 180 : 0, 0, 90));
							
							else if (y > 0 && Map[y - 1][x] == '-')
								wall->ChangeRotation(FVector(callWalkable(x - 1, y - 1, 0) ? 180 : 0, 0, -90));
							
							else if (x > 0 && Map[y][x - 1] == '-')
								wall->ChangeRotation(FVector(callWalkable(x - 1, y - 1, 0) ? 180 : 0, 180, 0));
							
							else
								wall->ChangeRotation(FVector(callWalkable(x + 1, y - 1, 0) ? 180 : 0, 0, 0));
						}

					}
					
					else {

						int32 Index;
						FVector Rotation;

						if (side[1])
							
							if (side[3]) {
								Index = callWalkable(x + 1, y, 0) || callWalkable(x, y - 1, 0) ? 1 : 2;
								Rotation = FVector(0, 0, 180);
							}
						
							else {
								Index = callWalkable(x - 1, y, 0) || callWalkable(x, y - 1, 0) ? 1 : 2;
								Rotation = FVector(0, 0, -90);
							}
						
						else if (side[3]) {
							Index = callWalkable(x + 1, y, 0) || callWalkable(x, y + 1, 0) ? 1 : 2;
							Rotation = FVector(0, 0, 90);
						}
						
						else {
							Index = callWalkable(x - 1, y, 0) || callWalkable(x, y + 1, 0) ? 1 : 2;
							Rotation = FVector(0, 0, 0);
						}

						if (callGhostHouse(x + 1, y + 1) || callGhostHouse(x - 1, y + 1) || callGhostHouse(x + 1, y - 1) || callGhostHouse(x - 1, y - 1))
							Index = 4;

						AWall* wall = Cast<AWall>(GetWorld()->SpawnActor(singleW ? WallClass[1] : DoubleWallClass[Index], &Location));
						wall->ChangeRotation(Rotation);

					}

					break;

				case 4:

					AWall * wall = Cast<AWall>(GetWorld()->SpawnActor(singleW ? WallClass[1] : DoubleWallClass[1], &Location));

					if (callWalkable(x - 1, y + 1, 0))
						wall->ChangeRotation(FVector(0, 0, 90));
					else if (callWalkable(x - 1, y - 1, 0))
						wall->ChangeRotation(FVector(0, 0, 180));
					else if (callWalkable(x + 1, y - 1, 0))
						wall->ChangeRotation(FVector(0, 0, -90));

					break;

				}

				break;
			}
			
			// Teleport
			case 'T':
				// Controlla se la posizione a dx non è valida
				if (!isValidPosition(x + 1, y, MapSize)) {
					FVector tempLocation = Location + FVector(100, 0, 1);
					// Crea un'istanza dell'attore Empty_Tile con posizione temporanea e scala modificata
					GetWorld()->SpawnActor(Empty_TileClass, &tempLocation)->SetActorRelativeScale3D(FVector(1, 1.5, 1));
				}
				// Controlla se la posizione a sx non è valida
				else if (!isValidPosition(x - 1, y, MapSize)) {
					FVector tempLocation = Location + FVector(-100, 0, 1);
					// Crea un'istanza Empty_Tile
					GetWorld()->SpawnActor(Empty_TileClass, &tempLocation)->SetActorRelativeScale3D(FVector(1, 1.5, 1));
				}
				//sopra
				else if (!isValidPosition(x, y + 1, MapSize)) {
					FVector tempLocation = Location + FVector(0, 100, 1);
					// Crea un'istanza Empty_Tile
					GetWorld()->SpawnActor(Empty_TileClass, &tempLocation)->SetActorRelativeScale3D(FVector(1.5, 1, 1));
				}
				//sotto
				else if (!isValidPosition(x, y - 1, MapSize)) {
					FVector tempLocation = Location + FVector(0, -100, 1);
					// Crea un'istanza Empty_Tile
					GetWorld()->SpawnActor(Empty_TileClass, &tempLocation)->SetActorRelativeScale3D(FVector(1.5, 1, 1));
				}

				break;
			
			// GATE
			case '+':
			{
				// Crea un'istanza dell'attore AGateCasaFantasmi nella posizione specificata
				AGateCasaFantasmi* door = Cast<AGateCasaFantasmi>(GetWorld()->SpawnActor(DoorClass, &Location));
				FVector Rotation = FVector(0, 0, 0);

				// Controlla se la posizione a destra o a sinistra è una ghost house
				if (callGhostHouse(x + 1, y) || callGhostHouse(x - 1, y)) {
					Rotation.Z = 90;
					// Controlla se la posizione a destra è una ghost house e se la posizione a sinistra o una cella precedente sono percorribili o una ghost house
					if (callGhostHouse(x + 1, y) && (callWalkable(x - 1, y, 0) || callGhostHouse(x - 1, y) && x > 0 && Map[y][x - 1] != 'G'))
						Rotation.X = 180;
				}
				// Controlla se la posizione sopra è una ghost house e se la posizione sotto o una cella successiva sono percorribili o una ghost house
				else if (callGhostHouse(x, y - 1) && (callWalkable(x, y + 1, 0) || callGhostHouse(x, y + 1) && y + 1 < MapSize.Y && Map[y + 1][x] != 'G'))
					Rotation.X = 180;


				//Imposto la rotazione
				door->ChangeRotation(Rotation);

				break;
			}
			
			// PacMan
			case 'M':
			{

				FVector temp = Location;
				
				//Controllo se le celle adiacneti sono percorribili
				if (callWalkable(x + 1, y, 1))
					temp += FVector(50, 0, 0);
				
				else if (callWalkable(x - 1, y, 1))
					temp += FVector(-50, 0, 0);
				
				else if (callWalkable(x, y + 1, 1))
					temp += FVector(0, 50, 0);
				
				else if (callWalkable(x, y - 1, 1))
					temp += FVector(0, -50, 0);

				// Crea un'istanza di PacMan nella posizione specificata
				Characters[0] = Cast<APacMan>(GetWorld()->SpawnActor(PacManClass, &temp));
				
				//Imposto l'istanza nel db, specificando l'istanza generata
				Cast<APacMan>(Characters[0])->DatabasePtr = DatabaseRef;
				//Imposto la mappa locale per l'istanza
				Cast<APacMan>(Characters[0])->setLocalMap(Map, MapSize);

				break;

			}
			
			// Blinky
			case 'B':
			{
				// Crea un'istanza di ABlinky nella posizione specificata
				FVector temp = Location + FVector(50, 0, 0);
				Characters[1] = Cast<ABlinky>(GetWorld()->SpawnActor(GhostClass[0], &temp));
				//Imposto nel database tutte le variabili che si riferiscono a Blinky (Characters[1]) e le inizializzo
				Cast<ABlinky>(Characters[1])->DatabasePtr = DatabaseRef;						// puntatore
				Cast<ABlinky>(Characters[1])->currentState = DatabaseRef->DefaultGhostState;	//stato
				Cast<ABlinky>(Characters[1])->SpawnTarget = FVector2D(x, y);					//target spawn
				Cast<ABlinky>(Characters[1])->setLocalMap(Map, MapSize);						// mappa locale
				Cast<ABlinky>(Characters[1])->DefinePosAround(1);								//pos intorno

				break;

			}
			
			// Pinky
			case 'P':
			{
				// Crea un'istanza di APinky nella posizione specificata
				FVector temp = Location + FVector(50, 0, 0);
				Characters[2] = Cast<APinky>(GetWorld()->SpawnActor(GhostClass[1], &temp));
				//Imposto nel database tutte le variabili che si riferiscono a Pinky (Characters[2]) e le inizializzo
				Cast<APinky>(Characters[2])->DatabasePtr = DatabaseRef;
				Cast<APinky>(Characters[2])->currentState = 3;
				Cast<APinky>(Characters[2])->SpawnTarget = FVector2D(x, y);
				Cast<APinky>(Characters[2])->setLocalMap(Map, MapSize);
				Cast<APinky>(Characters[2])->DefinePosAround(2);

				break;

			}

			// Inky
			case 'I':
			{
				// Crea un'istanza di AInky nella posizione specificata
				FVector temp = Location + FVector(50, 0, 0);
				Characters[3] = Cast<AInky>(GetWorld()->SpawnActor(GhostClass[2], &temp));
				//Imposto nel database tutte le variabili che si riferiscono a Inky (Characters[3]) e le inizializzo
				Cast<AInky>(Characters[3])->DatabasePtr = DatabaseRef;
				Cast<AInky>(Characters[3])->currentState = 3;
				Cast<AInky>(Characters[3])->SpawnTarget = FVector2D(x, y);
				Cast<AInky>(Characters[3])->setLocalMap(Map, MapSize);
				Cast<AInky>(Characters[3])->DefinePosAround(2);

				break;

			}

			// Clyde
			case 'C':
			{
				// Crea un'istanza di AClyde nella posizione specificata
				FVector temp = Location + FVector(50, 0, 0);
				Characters[4] = Cast<AClyde>(GetWorld()->SpawnActor(GhostClass[3], &temp));
				//Imposto nel database tutte le variabili che si riferiscono a Clyde (Characters[4]) e le inizializzo
				Cast<AClyde>(Characters[4])->DatabasePtr = DatabaseRef;
				Cast<AClyde>(Characters[4])->currentState = 3;
				Cast<AClyde>(Characters[4])->SpawnTarget = FVector2D(x, y);
				Cast<AClyde>(Characters[4])->setLocalMap(Map, MapSize);
				Cast<AClyde>(Characters[4])->DefinePosAround(2);

				break;

			}
			
			// Target di deafult per i fantasmi
			case '1': case '2': case '3': case '4':

				DefaultTargets[Map[y][x] - '1'] = FVector2D(x, y);

				break;
			
			// Casa
			case 'H':

				HomeTargets[0] = FVector2D(x, y);

				break;
			
			// Secondo Target
			case 'N':

				HomeTargets[1] = FVector2D(x, y);

				break;


			default:
				
				break;

			};

		}
	// Se pacman istanziato
	if (Characters[0] != nullptr)
		//Imposto l'array di foods
		Cast<APacMan>(Characters[0])->setFoodArray(FoodArray);
	// Aggiorno il numero di foos rimanenti
	DatabaseRef->LeftOverFood = FoodArray.Num();
	//Definisco i target
	for (int32 i = 1; i < Characters.Num(); i++) {
		Cast<IInterfacciaFantasmi>(Characters[i])->DefaultTarget = DefaultTargets[i - 1];
		Cast<IInterfacciaFantasmi>(Characters[i])->HomeTarget = HomeTargets[0];
		Cast<IInterfacciaFantasmi>(Characters[i])->NoHomeTarget = HomeTargets[1];
	}

	return Characters;
			
}

bool AFieldGenerator::callWalkable(int32 x, int32 y, int32 mode)
{
	return isValidPosition(x, y, MapSize) && isWalkable(Map[y][x], mode);
}

bool AFieldGenerator::callGhostHouse(int32 x, int32 y)
{
	return isValidPosition(x, y, MapSize) && isGhostHouse(Map[y][x]);
}
// Funzione utilizzate per la rigenerazione della mappa utilizzato dei parametri (map,size, pawn e i foods)
// Utilizzato per esempio quando pacman perde un vita
void AFieldGenerator::ReGenerate(TArray<FString> map, FVector2D size, TArray<AActor*> characters, bool restoreFood)
{

	Map = TArray<FString>(map);
	MapSize = FVector2D(size);
	FoodArray = TArray<FFoodPointer>();

	for (int32 y = 0; y < MapSize.Y; y++)
		for (int32 x = 0; x < MapSize.X; x++) {

			FVector Location = 100 * FVector(x, y, 0);
			
			switch (Map[y][x]) {
			
			// Food o power food
			case 'o':
			case '@':
			{

				if (restoreFood) {

					AFood* food = Cast<AFood>(GetWorld()->SpawnActor(FoodClass, &Location));
					food->setPU(Map[y][x] == '@');

					FoodArray.Add(FFoodPointer(food, FVector2D(x, y)));

				}

				break;

			}
			
			// Pacman
			case 'M':
			{

				FVector temp = Location;
				if (callWalkable(x + 1, y, 1))
					temp += FVector(50, 0, 0);
				else if (callWalkable(x - 1, y, 1))
					temp += FVector(-50, 0, 0);
				else if (callWalkable(x, y + 1, 1))
					temp += FVector(0, 50, 0);
				else if (callWalkable(x, y - 1, 1))
					temp += FVector(0, -50, 0);

				if (characters[0] != nullptr) {
					Cast<APacMan>(characters[0])->SetActorLocation(temp, false, nullptr, ETeleportType::TeleportPhysics);
					Cast<APacMan>(characters[0])->StartedGame = true;
				}

				break;

			}
			
			// Blinky
			case 'B':

				if (characters[1] == nullptr)
					break;

				Cast<ABlinky>(characters[1])->SetActorLocation(Location + FVector(50, 0, 0), false, nullptr, ETeleportType::TeleportPhysics);
				Cast<ABlinky>(characters[1])->UpdateState(DatabaseRef->DefaultGhostState);
				Cast<ABlinky>(characters[1])->UpdatePosAround(1);

				break;
			
			// Pinky
			case 'P':

				if (characters[2] == nullptr)
					break;

				Cast<APinky>(characters[2])->SetActorLocation(Location + FVector(50, 0, 0), false, nullptr, ETeleportType::TeleportPhysics);
				Cast<APinky>(characters[2])->UpdateState(3);
				Cast<APinky>(characters[2])->UpdatePosAround(2);

				break;
			
			// Inky
			case 'I':

				if (characters[3] == nullptr)
					break;

				Cast<AInky>(characters[3])->SetActorLocation(Location + FVector(50, 0, 0), false, nullptr, ETeleportType::TeleportPhysics);
				Cast<AInky>(characters[3])->UpdateState(3);
				Cast<AInky>(characters[3])->UpdatePosAround(2);

				break;
			
			// Clyde
			case 'C':

				if (characters[4] == nullptr)
					break;

				Cast<AClyde>(characters[4])->SetActorLocation(Location + FVector(50, 0, 0), false, nullptr, ETeleportType::TeleportPhysics);
				Cast<AClyde>(characters[4])->UpdateState(3);
				Cast<AClyde>(characters[4])->UpdatePosAround(2);

				break;


			default:
				
				break;

			}
		
		}
	// Se dobbiamo ripristinare il cibo 
	if (restoreFood) {
		if (characters[0] != nullptr)
			Cast<APacMan>(characters[0])->setFoodArray(FoodArray);
		DatabaseRef->LeftOverFood = FoodArray.Num();
	}

}