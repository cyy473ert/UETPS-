#include "PowerSwitch.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Gameplay/ShelterGameInstance.h"

APowerSwitch::APowerSwitch()
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
	SwitchMesh->SetupAttachment(RootComp);

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetupAttachment(RootComp);
	TriggerSphere->SetSphereRadius(200.0f);

	InteractHint = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractHint"));
	InteractHint->SetupAttachment(RootComp);
}

void APowerSwitch::BeginPlay()
{
	Super::BeginPlay();

	if (HintWidgetClass)
	{
		InteractHint->SetWidgetClass(HintWidgetClass);
	}
	InteractHint->SetVisibility(false);
}

void APowerSwitch::ShowInteractWidget_Implementation()
{
	InteractHint->SetVisibility(true);
}

void APowerSwitch::HideInteractWidget_Implementation()
{
	InteractHint->SetVisibility(false);
}

void APowerSwitch::OnInteract_Implementation(ACYYCharacterFather* Interactor)
{
	if (bActivated) return;
	bActivated = true;

	UShelterGameInstance* GI = Cast<UShelterGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->bPowerRestored = true;
	}

	InteractHint->SetVisibility(false);

	if (ActivateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ActivateSound, GetActorLocation());
	}
}
