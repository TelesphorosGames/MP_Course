// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName= "Initial State"),
	EWS_Equipped UMETA(DisplayName= "Equipped"),
	EWS_Dropped UMETA(DisplayName= "Dropped"),

	EWS_MAX UMETA(DisplayName="DefaultMAX")
};

UCLASS()
class MP_COURSE_API AWeapon : public AActor
{
	GENERATED_BODY()

	
	
public:	

	AWeapon();
	
	virtual void Tick(float DeltaTime) override;


	/* Public Getters and Setters */
	
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State ;}

	/* Public Custom Functions */ 

	void ShowPickupWidget(bool bShowWidget);


protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnAreaSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnAreaSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:

	UPROPERTY(VisibleAnywhere, Category  ="Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;
	
	// USED TO DETECT OVERLAPS WITH CHARACTERS
	UPROPERTY(VisibleAnywhere, Category  ="Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere)
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category  ="Weapon Properties")
	class UWidgetComponent* PickupWidget;
	

};
