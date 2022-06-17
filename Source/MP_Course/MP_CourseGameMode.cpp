// Copyright Epic Games, Inc. All Rights Reserved.

#include "MP_CourseGameMode.h"
#include "MP_CourseCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMP_CourseGameMode::AMP_CourseGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
