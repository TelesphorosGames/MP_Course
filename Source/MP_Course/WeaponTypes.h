﻿#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName="Assault Rilfe"),
	EWT_RocketLauncher UMETA(DisplayName="Rocket Launcher"),

	EWT_MAX UMETA(DisplayName = "DefaultMax")
};
