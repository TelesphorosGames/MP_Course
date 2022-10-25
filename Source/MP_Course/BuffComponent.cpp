
#include "BuffComponent.h"

#include "BlasterCharacter.h"
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
}

void UBuffComponent::Heal(int32 HealAmount, float HealTime)
{
	bHealing = true;
	HealingRate = HealAmount / HealTime ;

	AmountToHeal += HealAmount ;
}

void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime)
{
	
}

void UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchedSpeed = CrouchSpeed;
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

void UBuffComponent::ResetSpeed()
{
	
}

