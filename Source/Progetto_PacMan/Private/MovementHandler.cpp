#include "MovementHandler.h"


void IMovementHandler::setLocalMap(TArray<FString> lm, FVector2D lms)
{
	// Imposta la mappa locale e le sue dimensioni
	localMap = lm;
	localMapSize = lms;
}

FVector2D IMovementHandler::RelativePosition(FVector pos)
{
	// Restituisce la posizione relativa alle celle della mappa
	return FVector2D(FMath::DivideAndRoundNearest((int32) pos.X, 100), FMath::DivideAndRoundNearest((int32) pos.Y, 100));
}

void IMovementHandler::UpdatePosition() {} //override nelle classi figlie

void IMovementHandler::DefinePosAround(int32 mode) {

	// Inizializza l'array posAround con valori falsi
	posAround = TArray<bool>();
	for (int32 i = 0; i < 4; i++)
		posAround.Add(false);
	UpdatePosAround(mode);

}

void IMovementHandler::UpdatePosAround(int32 mode) {

	// Aggiorna l'array posAround con i valori di validità delle posizioni adiacenti
	posAround[0] = IInterfacciaField::isValidPosition(Position + FVector2D(1, 0), localMapSize) && IInterfacciaField::isWalkable(localMap[Position.Y][Position.X + 1], mode);
	posAround[1] = IInterfacciaField::isValidPosition(Position + FVector2D(-1, 0), localMapSize) && IInterfacciaField::isWalkable(localMap[Position.Y][Position.X - 1], mode);
	posAround[2] = IInterfacciaField::isValidPosition(Position + FVector2D(0, 1), localMapSize) && IInterfacciaField::isWalkable(localMap[Position.Y + 1][Position.X], mode);
	posAround[3] = IInterfacciaField::isValidPosition(Position + FVector2D(0, -1), localMapSize) && IInterfacciaField::isWalkable(localMap[Position.Y - 1][Position.X], mode);

}
 
void IMovementHandler::setPercentual(float perc) {

	Percentual = perc;

}

void IMovementHandler::TeleportBehavior()
{
	// Gestisce il comportamento del teletrasporto

	// Uscita dalla mappa tramite la coordinata X
	if (currentVelocity.X != 0 && (Position.X == 0 || Position.X == localMapSize.X - 1))
		posAround[currentVelocity.X > 0 ? 0 : 1] = true;

	// Uscita dalla mappa tramite la coordinata Y
	if (currentVelocity.Y != 0 && (Position.Y == 0 || Position.Y == localMapSize.Y - 1))
		posAround[currentVelocity.Y > 0 ? 2 : 3] = true;
	
	// Se si trova fuori dalla mappa tramite la coordinata X
	if (Position.X < 0 || Position.X >= localMapSize.X) {
		Position.X = FMath::Clamp(Position.X + (currentVelocity.X > 0 ? -1 : 1) * localMapSize.X, -1, localMapSize.X);
		//Sposto l'actor
		Cast<AActor>(this)->SetActorLocation(FVector(Position.X * 100, Position.Y * 100, 0), false, nullptr, ETeleportType::TeleportPhysics);
		//aggirno le pos intorno all'actor
		UpdatePosAround(1);
	}

	// Se si trova fuori dalla mappa tramite la coordinata Y
	if (Position.Y < 0 || Position.Y >= localMapSize.Y) {
		Position.Y = FMath::Clamp(Position.Y + (currentVelocity.Y > 0 ? -1 : 1) * localMapSize.Y, -1, localMapSize.Y);
		Cast<AActor>(this)->SetActorLocation(FVector(Position.X * 100, Position.Y * 100, 0), false, nullptr, ETeleportType::TeleportPhysics);
		UpdatePosAround(1);
	}

}

FVector2D IMovementHandler::getDirection()
{
	// Restituisce la direzione del movimento normalizzata
	FVector2D absVelocity = FMath::Abs(currentVelocity);
	return FVector2D(absVelocity.X < 0.2 ? currentVelocity.X : (currentVelocity.X / absVelocity.X), absVelocity.Y < 0.2 ? currentVelocity.Y : (currentVelocity.Y / absVelocity.Y));
}
