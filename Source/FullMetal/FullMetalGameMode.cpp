// Copyright Epic Games, Inc. All Rights Reserved.

#include "FullMetalGameMode.h"
#include "FullMetalCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFullMetalGameMode::AFullMetalGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
