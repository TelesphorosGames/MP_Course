
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

UCLASS()
class MP_COURSE_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	

	APickupSpawnPoint();
	
	
	virtual void Tick(float DeltaTime) override;

	
protected:

	virtual void BeginPlay() override;

		
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickup>> PickupClasses;

	UPROPERTY()
	APickup* SpawnedPickup;

	
	void SpawnPickup();
	
	UFUNCTION()
	void SpawnPickupTimerFinished();

	UFUNCTION()
	void StartSpawnPickupTimer(AActor* DestroyedActor);	
	

private:

	FTimerHandle SpawnPickupTimer;

	float SpawnPickupTimeMin = 0.f;
	float SpawnPickupTimeMax = 1.f;

	
	

};
