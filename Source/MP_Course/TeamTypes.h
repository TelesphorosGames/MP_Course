#pragma once

UENUM(BlueprintType)
enum class ETeam : uint8
{
	ET_TeamOne UMETA(DisplayName="TeamOne"),
	ET_TeamTwo UMETA(DisplayName="TeamTwo"),
	ET_TeamThree UMETA(DisplayName="TeamThree"),
	ET_TeamFour UMETA(DisplayName="TeamFour"),
	ET_TeamDefault UMETA(DisplayName="TeamDefault"),

	ET_Max UMETA(DisplayName="DefaultMax")
	
};