#include "KeyCardItem.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Gameplay/Player/CYYCharacterFather.h"
#include "Gameplay/Player/CYYPlayerController.h"
#include "Components/UIManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Gameplay/ShelterGameInstance.h"

AKeyCardItem::AKeyCardItem()
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	CardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardMesh"));
	CardMesh->SetupAttachment(RootComp);

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(RootComp);
	PickupSphere->SetSphereRadius(150.0f);

	InteractHint = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractHint"));
	InteractHint->SetupAttachment(RootComp);
}

void AKeyCardItem::BeginPlay()
{
	Super::BeginPlay();
	InteractHint->SetVisibility(false);
}

void AKeyCardItem::ShowInteractWidget_Implementation()
{
	InteractHint->SetVisibility(true);
}

void AKeyCardItem::HideInteractWidget_Implementation()
{
	InteractHint->SetVisibility(false);
}

void AKeyCardItem::OnInteract_Implementation(ACYYCharacterFather* Interactor)
{
	UShelterGameInstance* GI = Cast<UShelterGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->bHasKeyCard = true;
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (ACYYPlayerController* CYYPC = Cast<ACYYPlayerController>(PC))
		{
			if (UUIManagerComponent* UIMgr = CYYPC->GetUIManagerComponent())
			{
				UIMgr->ShowPickupBanner(TEXT("获得了钥匙"));
			}
		}
	}

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}

	Destroy();
}
