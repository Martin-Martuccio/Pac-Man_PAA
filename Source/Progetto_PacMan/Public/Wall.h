#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wall.generated.h"

UCLASS()
class PROGETTO_PACMAN_API AWall : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AWall();

protected:
	
	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;
	
	void ChangeRotation(FVector rotation);

};

