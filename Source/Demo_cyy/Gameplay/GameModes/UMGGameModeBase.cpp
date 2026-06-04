#include "Gameplay/GameModes/UMGGameModeBase.h"
#include "Gameplay/Player/CYYPlayerController.h"

AUMGGameModeBase::AUMGGameModeBase()
{
	PlayerControllerClass = ACYYPlayerController::StaticClass();
}

void AUMGGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

