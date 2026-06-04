#include "Gameplay/Pickups/HealPickup.h"

#include "Components/InventoryComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Gameplay/Player/CYYCharacterFather.h"

AHealPickup::AHealPickup()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetSphereRadius(120.0f);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = PickupSphere;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(PickupSphere);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHealPickup::BeginPlay()
{
	Super::BeginPlay();

	if (PickupSphere)
	{
		PickupSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AHealPickup::OnPickupSphereBeginOverlap);
	}
}

void AHealPickup::OnPickupSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bPicked || !HasAuthority())
	{
		return;
	}

	ACYYCharacterFather* PlayerCharacter = Cast<ACYYCharacterFather>(OtherActor);
	if (!PlayerCharacter)
	{
		return;
	}

	UInventoryComponent* Inventory = PlayerCharacter->GetInventoryComponent();
	if (!Inventory)
	{
		return;
	}

	bPicked = true;
	Inventory->AddMedkit(FMath::Max(1, StackAmount));
	Destroy();
}

