// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USphereComponent;
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName= "Initial State"),
	EWS_Equipped UMETA(DisplayName= "Equipped"),
	EWS_Dropped UMETA(DisplayName= "Dropped"),
	EWS_EquippedSecondary UMETA(DisplayName="EquippedSecondary"),

	EWS_MAX UMETA(DisplayName="DefaultMAX")
};

UCLASS()
class MP_COURSE_API AWeapon : public AActor
{
	GENERATED_BODY()

public:	

	AWeapon();
	
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	

	/* Public Getters and Setters */
	
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere ;}
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh ;}
	FORCEINLINE UTexture2D* GetCrosshairsCenter() const {return CrosshairsCenter ;}
	FORCEINLINE UTexture2D* GetCrosshairsLeft() const {return CrosshairsLeft ;}
	FORCEINLINE UTexture2D* GetCrosshairsRight() const {return CrosshairsRight ;}
	FORCEINLINE UTexture2D* GetCrosshairsTop() const {return CrosshairsTop ;}
	FORCEINLINE UTexture2D* GetCrosshairsBottom() const {return CrosshairsBottom ;}
	FORCEINLINE float GetZoomedFOV() const {return ZoomedFOV ;}
	FORCEINLINE float GetZoomedInterpSpeed() const {return ZoomInterpSpeed ;}
	FORCEINLINE float GetFireDelay() const {return FireDelay ;}
	FORCEINLINE	bool GetAutomaticWeapon() const {return bAutomaticWeapon ;}
	FORCEINLINE EWeaponType GetWeaponType() const{return WeaponType ;}
	FORCEINLINE int32 GetAmmo() const {return Ammo ;}
	FORCEINLINE int32 GetMagazineCapacity() const {return MagCapacity ;}
	FORCEINLINE class USoundCue* GetEquipSound() const {return EquipSound ;}
	
	FORCEINLINE	void SetWeaponType(EWeaponType Type){ WeaponType = Type; }

	/* Public Custom Functions */ 

	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);

	void Dropped();
	void SetWeaponState(EWeaponState State);
	void OnEquipped();
	void OnDropped();
	void OnEquippedSecondary();
	void SetHUDWeaponAmmo();
	void SetHUDWeaponAmmo(int32 AmmoAmount);
	bool IsEmpty();
	bool IsFull();
	void AddAmmo(int32 AmmoToAdd);

	void EnableCustomDepth(bool bEnable);

	UPROPERTY()
	bool bDestroyWeapon = false;
	
protected:
	
	virtual void BeginPlay() override;
	virtual void OnWeaponStateSet();

	UFUNCTION()
	virtual void OnAreaSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnAreaSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Category= "Weapon Properties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;



private:

	UPROPERTY(VisibleAnywhere, Category  ="Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;
	
	// USED TO DETECT OVERLAPS WITH CHARACTERS
	UPROPERTY(VisibleAnywhere, Category  ="Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere)
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category  ="Weapon Properties")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category  ="Weapon Properties")
	class USoundCue* EquipSound{};



	// Crosshairs Textures :

	UPROPERTY(EditAnywhere, Category=Crosshairs)
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category=Crosshairs)
	class UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category=Crosshairs)
	class UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category=Crosshairs)
	class UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category=Crosshairs)
	class UTexture2D* CrosshairsBottom;

	// Zoomed field of view while aiming
	UPROPERTY(EditAnywhere)
	float ZoomedFOV{30.f};
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed{20.f};

	UPROPERTY(EditAnywhere)
	float FireDelay =.15f;
	
	UPROPERTY(EditAnywhere)
	bool bAutomaticWeapon = true;

	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Ammo)
	int32 Ammo;

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;
	
	UFUNCTION()
	void OnRep_Ammo();

	UFUNCTION()
	void SpendRound();

	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter{};
	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	EWeaponType WeaponType{};


	

};
