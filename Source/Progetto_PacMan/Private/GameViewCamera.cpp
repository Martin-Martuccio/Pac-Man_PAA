#include "GameViewCamera.h"

//posizione
void AGameViewCamera::setPosition(FVector pos) {

	SetActorLocation(pos, false, nullptr, ETeleportType::TeleportPhysics);

}
//rotazione
void AGameViewCamera::setRotation(FVector rot) {

	SetActorRotation(FQuat::MakeFromEuler(rot), ETeleportType::TeleportPhysics);

}
//posizione e rotazione
void AGameViewCamera::setPositionAndRotation(FVector pos, FVector rot) {

	setPosition(pos);
	setRotation(rot);

}