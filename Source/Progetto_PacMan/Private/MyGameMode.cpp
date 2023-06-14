#include "MyGameMode.h"

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Impostazione del flag GameNotStarted ==> INZIO del gioco
	GameNotStarted = true;

	// Riferimento alla classe AFieldGenerator
	TArray<AActor*> temp;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFieldGenerator::StaticClass(), temp);
	// Verifica se è stato trovato almeno un oggetto di tipo AFieldGenerator
	if (temp.IsValidIndex(0))
		Map = Cast<AFieldGenerator>(temp[0]);
	
	else {
		UE_LOG(LogTemp, Warning, TEXT("MyGameMode ERROR: manca la mappa di gioco"));
	
		return;
	}

	// Riferimento alla classe AVariableStorageHandler
	temp = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVariableStorageHandler::StaticClass(), temp);
	// Verifica se è stato trovato almeno un oggetto di tipo AVariableStorageHandler
	if (temp.IsValidIndex(0))
		Database = Cast<AVariableStorageHandler>(temp[0]);
	
	else {
		UE_LOG(LogTemp, Warning, TEXT("MyGameMode ERROR: manca VariableStorage"));
	
		return;
	}

	// Caricamento dei dati dal database
	if (!Database->LoadAll()) {
		UE_LOG(LogTemp, Warning, TEXT("MyGameMode ERROR: non riesco a caricare i dati del DB"));
	
		return;
	}

	// Inizializzazione delle variabili di gioco
	Score = 0;
	HighScore = 0;
	GameTime = 0;

	// Per punteggio
	UseFile(true);

	
	Database->DefaultGhostState = 1;
	Database->LivesLeft = 3;

	StateCounter = 0;

	// Generazione del labirinto
	Characters = Map->Generate(Database->Map, Database->MapSize);

	localTotalFood = Database->LeftOverFood;
	localLeftOverFood = Database->LeftOverFood;

	// Avvio del gioco dopo 5 secondi
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [&]() {

		Database->canPacManMove = true;
		Database->canGhostsMove = true;

		if (Database->GhostModeTimers.IsValidIndex(0))
			GetWorldTimerManager().SetTimer(TimerHandle_GhostMode, [&]() { hasChangedMode = true; }, Database->GhostModeTimers[0], false);

		GetWorldTimerManager().SetTimer(GlobalTimerHandle, [&]() { GameTime++; }, 1.0f, true);

		GameNotStarted = false;

	}, 5.0f, false);

}

// Funzione per leggere/scrivere un file di configurazione
void AMyGameMode::UseFile(bool ReadMode) {

	FString file = FPaths::ProjectConfigDir();
	file.Append(TEXT("MyConfig.txt"));

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	// Verifica se il file esiste
	if (FileManager.FileExists(*file))
	{

		if (ReadMode) {

			FString FileContent;

			if (FFileHelper::LoadFileToString(FileContent, *file, FFileHelper::EHashOptions::None)) {

				UE_LOG(LogTemp, Warning, TEXT("Lettura corretta, Contento: %s"), *FileContent);

				for (int32 i = 0; i < FileContent.Len(); i++)
					// Se il file è stato letto correttamente, viene salvato il valore letto nella variabile HighScore
					HighScore = (FileContent[i] - '0') * pow(10, FileContent.Len() - 1 - i);

			}

			else
				UE_LOG(LogTemp, Warning, TEXT("Nessun caricamento dal fil txt"));

		}
		
		else {

			if (FFileHelper::SaveStringToFile(FString::FromInt(HighScore), *file)) {
				UE_LOG(LogTemp, Warning, TEXT("MyGameMode ERROR: Scrittura riuscita!"));
			}
		
			else
				UE_LOG(LogTemp, Warning, TEXT("MyGameMode ERROR: Scrittura non riuscita!"));

		}
		
	}
	
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Gamemode: ERROR FILE non trovato"));
		UE_LOG(LogTemp, Warning, TEXT("Gamemode: locazione del file errata: %s"), *file);
	}

}

void AMyGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GameNotStarted)

		return;
	// Impostazione del puntatore al database per ogni personaggio
	for (int32 i = 0; i < Characters.Num(); i++)
		if (!Cast<IMovementHandler>(Characters[i])->DatabasePtr->IsValidLowLevel())
			Cast<IMovementHandler>(Characters[i])->DatabasePtr = Database;

	// Timer globale per il tempo di gioco
	if (!GetWorldTimerManager().IsTimerActive(GlobalTimerHandle))
		GetWorldTimerManager().SetTimer(GlobalTimerHandle, [&]() { GameTime++; }, 1.0f, true);

	// Se Pacman ha mangiato tutti i foods
	if (Database->LeftOverFood <= 0) {

		// Reimpostazione degli alimenti rimasti
		Database->LeftOverFood = 100000;

		// Rimozione dei timer attivi
		if (GetWorldTimerManager().IsTimerActive(TimerHandle_GhostMode))
			GetWorldTimerManager().ClearTimer(TimerHandle_GhostMode);
		if (GetWorldTimerManager().IsTimerActive(TimerHandle_GhostFrightened))
			GetWorldTimerManager().ClearTimer(TimerHandle_GhostFrightened);
		
		// Reinizializzazione delle variabili
		Database->PU = false;
		Database->isPacManPU = false;
		Database->GhostsEaten = 0;
		localPoint = 0;
		localGhostsEaten = Database->GhostsEaten;

		// Salvataggio del punteggio più alto
		UseFile(false);

		// Blocco del movimento di Pacman e dei fantasmi
		Database->canPacManMove = false;
		Database->canGhostsMove = false;

		FTimerHandle TimerHandle1, TimerHandle2, TimerHandle3;

		// Timer per l'effetto dei personaggi
		GetWorldTimerManager().SetTimer(TimerHandle1, [&]() {

			for (int32 i = 0; i < Characters.Num(); i++)
				Characters[i]->SetActorLocation(Characters[i]->GetActorLocation() + FVector(0, 0, -3), false, nullptr, ETeleportType::TeleportPhysics);

			}, 2.0f, false);

		// Rigenerazione del labirinto e degli elementi presenti
		GetWorldTimerManager().SetTimer(TimerHandle2, [&]() {

			Map->ReGenerate(Database->Map, Database->MapSize, Characters, true);

			localTotalFood = Database->LeftOverFood;
			localLeftOverFood = Database->LeftOverFood;

			}, 3.0f, false);

		// Inizio di un nuovo gioco
		GetWorldTimerManager().SetTimer(TimerHandle3, [&]() {

			//Faccio muovere i pawn
			Database->canPacManMove = true;
			Database->canGhostsMove = true;

			GameTime = 0;

			}, 5.0f, false);

	}// fine (Gestione di pacman che ha vinto il livello)

	// Se è cambiata la modalità dei fantasmi
	if (hasChangedMode) {

		hasChangedMode = false;
		Database->DefaultGhostState = 1 - Database->DefaultGhostState;
		
		// Se ci sono altre modalità e i timer associati
		if (StateCounter < Database->GhostModeTimers.Num() - 1 && Database->GhostModeTimers[StateCounter] > 0) {

			StateCounter++;

			// Avvio del timer per il cambio di modalità
			GetWorldTimerManager().SetTimer(TimerHandle_GhostMode, [&]() { hasChangedMode = true; }, Database->GhostModeTimers[StateCounter], false);

		}

	}

	// Se Pacman è stato mangiato
	if (Database->PacManLost) {

		// Rimozione dei timer attivi presenti nel gioco
		if (GetWorldTimerManager().IsTimerActive(TimerHandle_GhostMode))
			GetWorldTimerManager().ClearTimer(TimerHandle_GhostMode);
		
		if (GetWorldTimerManager().IsTimerActive(TimerHandle_GhostFrightened))
			GetWorldTimerManager().ClearTimer(TimerHandle_GhostFrightened);

		//Salvataggio del punteggio più alto
		UseFile(false);

		Database->PacManLost = false;
		Database->canPacManMove = false;
		Database->canGhostsMove = false;

		FTimerHandle TimerHandle1, TimerHandle2, TimerHandle3;

		// Timer per l'effetto di dissolvenza dei personaggi
		GetWorldTimerManager().SetTimer(TimerHandle1, [&]() {

			for (int32 i = 0; i < Characters.Num(); i++)
				Characters[i]->SetActorLocation(Characters[i]->GetActorLocation() + FVector(0, 0, -3), false, nullptr, ETeleportType::TeleportPhysics);

			}, 2.0f, false);

		// Se non ci sono più vite rimaste ==> GAME OVER
		if (Database->LivesLeft <= 0) {

			GetWorldTimerManager().SetTimer(TimerHandle2, [&]() {

				TArray<AActor*> temp;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameViewCamera::StaticClass(), temp);
				if (temp.IsValidIndex(0))
					Cast<USceneComponent>(temp[0]->GetDefaultSubobjectByName(TEXT("GameOverText")))->SetVisibility(true);

				GameTime = 0;

				}, 5.0f, false);

		}
		else {

			//Pacman ha ancora delle vite rimaste, ma devo farlo 'rinascere'
			Database->PU = false;
			Database->isPacManPU = false;
			Database->GhostsEaten = 0;
			localPoint = 0;
			localGhostsEaten = Database->GhostsEaten;

			// Rigenerazione del labirinto
			GetWorldTimerManager().SetTimer(TimerHandle2, [&]() {

				Database->LivesLeft--;

				Map->ReGenerate(Database->Map, Database->MapSize, Characters, false);

				}, 4.0f, false);

			// Inizio del nuovo gioco
			GetWorldTimerManager().SetTimer(TimerHandle3, [&]() {

				Database->canPacManMove = true;
				Database->canGhostsMove = true;

				GameTime = 0;
				// Avvio del timer per il cambio di modalità dei fantasmi
				GetWorldTimerManager().SetTimer(TimerHandle_GhostMode, [&]() { hasChangedMode = true; }, Database->GhostModeTimers[StateCounter], false);

				}, 6.0f, false);
		}

	}

	// Se Pacman ha mangiato cibo
	if (localLeftOverFood > Database->LeftOverFood) {
		
		//Update delle variabili del punteggio e dei foods rimanenti
		Score += 10 * (localLeftOverFood - Database->LeftOverFood);
		localPoint += localLeftOverFood - Database->LeftOverFood;
		localLeftOverFood = Database->LeftOverFood;
		
		//NEW RECORD!!!!
		if (Score > HighScore)
			HighScore = Score;

		GameTime = 0;

		// Se era attivo l'effetto di potenziamento
		if (Database->PU) {

			Database->PU = false;
			Database->isPacManPU = true;

			// Rimozione del timer attivo per la modalità spaventata dei fantasmi
			if (GetWorldTimerManager().IsTimerActive(TimerHandle_GhostFrightened))
				GetWorldTimerManager().ClearTimer(TimerHandle_GhostFrightened);

			Database->GhostsEaten = 0;
			localGhostsEaten = Database->GhostsEaten;

			// Attivazione della modalità spaventata per i fantasmi
			for (int32 i = 1; i < Characters.Num(); i++)
				if (Cast<IInterfacciaFantasmi>(Characters[i])->currentState < 3)
					Cast<IInterfacciaFantasmi>(Characters[i])->UpdateState(2);

			// Timer per la modalità spaventata dei fantasmi
			GetWorldTimerManager().SetTimer(TimerHandle_GhostFrightened, [&]() {

				Database->PU = false;
				Database->isPacManPU = false;
				Database->GhostsEaten = 0;
				localGhostsEaten = Database->GhostsEaten;

				// Ripristino dello stato normale dei fantasmi
				for (int32 i = 1; i < Characters.Num(); i++)
					
					if (Cast<IInterfacciaFantasmi>(Characters[i])->currentState == 2)
						Cast<IInterfacciaFantasmi>(Characters[i])->UpdateState(Database->DefaultGhostState);

			}, Database->Settings[14], false);

		}

	}

	// I fantasmi Inky, Pinky e Clyde possono uscire dalla casa
	for (int32 i = 2; i < Characters.Num(); i++)
		if (Cast<IInterfacciaFantasmi>(Characters[i])->currentState == 3 && Cast<IInterfacciaFantasmi>(Characters[i - 1])->currentState < 3 && (localPoint >= Database->GhostFree[i - 2] || GameTime >= Database->GhostFree[i + 1]))
			Cast<IInterfacciaFantasmi>(Characters[i])->UpdateState(5);

	// Se Pacman ha mangiato un fantasma
	if (localGhostsEaten < Database->GhostsEaten) {

		//Aumento il punteggio
		Score += 100 * FMath::Pow(2.0f, (float)Database->GhostsEaten);
		localGhostsEaten = Database->GhostsEaten;

		if (Score > HighScore)
			HighScore = Score;

		// Se Pacman ha mangiato tutti i fantasmi (-1 perchè devo considerare anche pacman in Characters.Num)
		if (localGhostsEaten >= Characters.Num() - 1) {

			// Rimozione del timer attivo per la modalità spaventata dei fantasmi
			if (GetWorldTimerManager().IsTimerActive(TimerHandle_GhostFrightened))
				GetWorldTimerManager().ClearTimer(TimerHandle_GhostFrightened);

			Database->PU = false;
			Database->isPacManPU = false;
			Database->GhostsEaten = 0;
			localGhostsEaten = Database->GhostsEaten;

		}

	}

}

int32 AMyGameMode::getScore()
{
	return Score;
}

int32 AMyGameMode::getHighScore()
{
	return HighScore;
}

int32 AMyGameMode::getPacManLives()
{
	return Database->LivesLeft;
}
