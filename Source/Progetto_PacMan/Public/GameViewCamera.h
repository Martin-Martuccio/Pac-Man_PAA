#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Actor.h"
#include "GameViewCamera.generated.h"

UCLASS()
class PROGETTO_PACMAN_API AGameViewCamera : public ACameraActor
{
	GENERATED_BODY()

public:

	//funzioni per il setting della mia camera
	void setPosition(FVector pos);
	void setRotation(FVector rot);
	void setPositionAndRotation(FVector pos, FVector rot);

};
