#pragma once

#ifndef CUSTOM_DEPTH_PURPLE
#define CUSTOM_DEPTH_PURPLE 250
#endif
#ifndef CUSTOM_DEPTH_BLUE
#define CUSTOM_DEPTH_BLUE 251
#endif
#ifndef CUSTOM_DEPTH_TAN
#define CUSTOM_DEPTH_TAN 252
#endif

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
