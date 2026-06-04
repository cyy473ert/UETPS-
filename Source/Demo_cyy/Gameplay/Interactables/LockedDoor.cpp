#include "LockedDoor.h"

#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Gameplay/ShelterGameInstance.h"

ALockedDoor::ALockedDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComp);
	DoorMesh->SetMobility(EComponentMobility::Movable);

	StatusLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("StatusLight"));
	StatusLight->SetupAttachment(RootComp);
	StatusLight->SetLightColor(FLinearColor::Red);
	StatusLight->SetIntensity(5000.0f);
	StatusLight->SetAttenuationRadius(300.0f);

	InteractHint = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractHint"));
	InteractHint->SetupAttachment(RootComp);
}

void ALockedDoor::BeginPlay()
{
	Super::BeginPlay();

	ClosedRotation = DoorMesh->GetRelativeRotation();

	if (HintWidgetClass)
	{
		InteractHint->SetWidgetClass(HintWidgetClass);
	}
	InteractHint->SetVisibility(false);
}

void ALockedDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bUnlocked)
	{
		UShelterGameInstance* GI = Cast<UShelterGameInstance>(GetGameInstance());
		if (GI && GI->bPowerRestored && GI->bHasKeyCard)
		{
			bUnlocked = true;
			StatusLight->SetLightColor(FLinearColor::Green);
		}
		return;
	}

	if (bOpened)
	{
		FRotator Current = DoorMesh->GetRelativeRotation();
		FRotator Target = ClosedRotation + FRotator(0.0f, OpenAngle, 0.0f);
		FRotator NewRot = FMath::RInterpTo(Current, Target, DeltaTime, OpenSpeed);
		DoorMesh->SetRelativeRotation(NewRot);
	}
}

void ALockedDoor::Open()
{
	if (bOpened) return;
	bOpened = true;

	if (OpenSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
	}
}

void ALockedDoor::ShowInteractWidget_Implementation()
{
	if (bUnlocked)
	{
		InteractHint->SetVisibility(true);
	}
}

void ALockedDoor::HideInteractWidget_Implementation()
{
	InteractHint->SetVisibility(false);
}

void ALockedDoor::OnInteract_Implementation(ACYYCharacterFather* Interactor)
{
	if (bUnlocked && !bOpened)
	{
		Open();
	}
}
