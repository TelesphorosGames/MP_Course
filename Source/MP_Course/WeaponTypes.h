#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName="Assault Rilfe"),
	EWT_RocketLauncher UMETA(DisplayName="Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName="Pistol"),
	EWT_SubMachineGun UMETA(DisplayName="Sub-Machine Gun"),
	EWT_Shotgun UMETA(DisplayName="Shotgun"),
	EWT_SniperRifle UMETA(DisplayName="SniperRifle"),
	EWT_GrenadeLauncher UMETA(DisplayName="GrenadeLauncher"),

	EWT_MAX UMETA(DisplayName = "DefaultMax")
};
