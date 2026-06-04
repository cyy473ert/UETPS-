#include "Terminal.h"

#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Gameplay/ShelterGameInstance.h"
#include "UI/TerminalScreenWidget.h"

ATerminal::ATerminal()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	TerminalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TerminalMesh"));
	TerminalMesh->SetupAttachment(RootComp);

	ScreenWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ScreenWidget"));
	ScreenWidget->SetupAttachment(RootComp);
}

void ATerminal::BeginPlay()
{
	Super::BeginPlay();
}

void ATerminal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Accumulator += DeltaTime;
	if (Accumulator < UpdateInterval) return;
	Accumulator = 0.0f;

	UShelterGameInstance* GI = Cast<UShelterGameInstance>(GetGameInstance());
	if (!GI) return;

	if (UUserWidget* WidgetObj = ScreenWidget->GetUserWidgetObject())
	{
		if (UTerminalScreenWidget* Screen = Cast<UTerminalScreenWidget>(WidgetObj))
		{
			Screen->Refresh(GI->bPowerRestored, GI->bHasKeyCard);
		}
	}
}
