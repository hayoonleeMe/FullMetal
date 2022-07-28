// Copyright Epic Games, Inc. All Rights Reserved.

#include "FullMetalGameMode.h"
#include "Vanguard.h"
#include "UObject/ConstructorHelpers.h"

AFullMetalGameMode::AFullMetalGameMode()
{
	// set default pawn class to our Blueprinted character
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}*/

	static ConstructorHelpers::FClassFinder<ACharacter> BP_Vanguard(TEXT("Blueprint'/Game/Blueprints/BP_Vanguard.BP_Vanguard_C'"));
	if (BP_Vanguard.Succeeded())
	{
		DefaultPawnClass = BP_Vanguard.Class;
	}
}
