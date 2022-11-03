
#include "BuffComponent.h"

#include "BlasterCharacter.h"
#include "CombatComponent.h"
#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp(DeltaTime);
	ShieldRampUp(DeltaTime);
}

void UBuffComponent::Heal(int32 HealAmount, float HealTime)
{
	bHealing = true;
	HealingRate = HealAmount / HealTime ;

	AmountToHeal += HealAmount ;
}

void UBuffComponent::ReplenishShield(float ShieldAmount, float ReplenishTime)
{
	bReplenishingShield = true;
	ReplenishRate = ShieldAmount / ReplenishTime ;

	AmountToReplenish += ShieldAmount ;
}

void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime)
{
	if(Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(SpeedBuffTimer, this, &UBuffComponent::ResetSpeed, BuffTime);

	if(Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed ;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed ;
		
	}

	MulticastSpeedBuff(BuffBaseSpeed, BuffCrouchSpeed);
}

void UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchedSpeed = CrouchSpeed;
}

void UBuffComponent::BuffAccuracy(float BaseAccuracy, float BuffAccuracy, float BuffTime)
{
	if(Character)
	{
		UCombatComponent* CombatComponent = Character->GetCombatComponent();
        	if(CombatComponent && CombatComponent->GetEquippedWeapon())
        	{
        		InitialAccuracy = BaseAccuracy;
        		CombatComponent->GetEquippedWeapon()->SetScatterSphereRadius(
        		InitialAccuracy - BuffAccuracy);

        		Character->GetWorldTimerManager().SetTimer(AccuracyBuffTimer, this, &UBuffComponent::ResetAccuracy, BuffTime);
				
				MulticastAccuracyBuff(BuffAccuracy);
        	}
	}
	

}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if(!bHealing || Character == nullptr || Character->GetIsElimmed())
	{
		return;
	}

	const float HealThisFrame = HealingRate * DeltaTime ;
	
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0, Character->GetMaxHealth()));
	Character->UpdateHudHealth();
	AmountToHeal -= HealThisFrame;

	if(AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing=false;
		AmountToHeal = 0.f;
	}
		
}

void UBuffComponent::ShieldRampUp(float DeltaTime)
{
	if(!bReplenishingShield || Character == nullptr || Character->GetIsElimmed())
	{
		return;
	}

	const float ReplenishThisFrame = ReplenishRate * DeltaTime ;
	
	Character->SetShields(FMath::Clamp(Character->GetShields() + ReplenishThisFrame, 0, Character->GetMaxShields()));
	Character->UpdateHudShields();
	AmountToReplenish -= ReplenishThisFrame;

	if(AmountToReplenish <= 0.f || Character->GetShields() >= Character->GetMaxShields())
	{
		bReplenishingShield=false;
		AmountToReplenish = 0.f;
	}
}

void UBuffComponent::MulticastAccuracyBuff_Implementation(float InBuffAccuracy)
{
	UCombatComponent* CombatComponent = Character->GetCombatComponent();
	if(CombatComponent && CombatComponent->GetEquippedWeapon())
	{
		CombatComponent->GetEquippedWeapon()->SetScatterSphereRadius(
			   InitialAccuracy - InBuffAccuracy);
	}

}

void UBuffComponent::MulticastSpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed ;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed ;
	
}

void UBuffComponent::ResetSpeed()
{
	if(Character == nullptr || Character->GetCharacterMovement() == nullptr) return;

	Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed ;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchedSpeed ;

	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchedSpeed);
		
}

void UBuffComponent::ResetAccuracy()
{
	if(Character == nullptr || Character->GetCombatComponent() == nullptr || Character->GetCombatComponent()->GetEquippedWeapon() == nullptr) return;
	
	Character->GetCombatComponent()->GetEquippedWeapon()->SetScatterSphereRadius(InitialAccuracy);
	MulticastAccuracyBuff(0);
}

