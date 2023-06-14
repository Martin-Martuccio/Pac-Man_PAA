#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovementHandler.h"
#include "InterfacciaFantasmi.h"
#include "Pinky.generated.h"

UCLASS()
class PROGETTO_PACMAN_API APinky : public AActor, public IInterfacciaFantasmi, public IMovementHandler
{
	GENERATED_BODY()
	
public:	
	
	APinky();

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void Tick(float DeltaTime) override;
	virtual void UpdateTargetChase() override;

	UPROPERTY()
		TArray<UMaterialInstanceDynamic*> _defaultColor;

	virtual TArray<UMaterialInstanceDynamic*> getDefaultColor() override;

};
