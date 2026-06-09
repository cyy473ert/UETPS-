#include "Item.h"

#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Gameplay/Player/CYYCharacterFather.h"
#include "Kismet/GameplayStatics.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetSphereRadius(50.0f);
	PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	RootComponent = PickupCollision;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(PickupCollision);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(ItemMesh);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	PickupWidget->SetVisibility(false);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItem::ShowInteractWidget_Implementation()
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(true);
	}
}

void AItem::HideInteractWidget_Implementation()
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
}

UWeapon* AItem::DataAsset_Implementation()
{
	if (WeaponDataAsset)
	{
		return WeaponDataAsset;
	}
	if (WeaponDataAssetClass)
	{
		return WeaponDataAssetClass->GetDefaultObject<UWeapon>();
	}
	UE_LOG(LogTemp, Warning, TEXT("DataAsset_Implementation: both WeaponDataAsset and WeaponDataAssetClass are null. Actor=%s"), *GetName());
	return nullptr;
}

void AItem::AttachtoCharacter(FName SocketName)
{
	ACYYCharacterFather* Character = Cast<ACYYCharacterFather>(GetOwner());
	if (!Character || !Character->Arm)
	{
		return;
	}

	AttachToComponent(
		Character->Arm,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		SocketName
	);
}

void AItem::EquitmentWeapon()
{
	AttachtoCharacter(EquitmentSocketName);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (PickupCollision)
	{
		PickupCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AItem::ShootParticles()
{
	if (!ItemMesh)
	{
		return;
	}

	const USkeletalMeshSocket* ParticlesSocket = ItemMesh->GetSocketByName(TEXT("ShootParticles"));
	if (!ParticlesSocket)
	{
		return;
	}

	const FTransform ParticlesSocketTransform = ParticlesSocket->GetSocketTransform(ItemMesh);
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			MuzzleFlash,
			ParticlesSocketTransform.GetLocation(),
			ParticlesSocketTransform.GetRotation().Rotator()
		);
	}

	ACYYCharacterFather* Character = Cast<ACYYCharacterFather>(GetOwner());
	if (!Character || !Character->GetFollowCamera())
	{
		return;
	}

	FHitResult FireHit;
	const FVector Start = Character->GetFollowCamera()->GetComponentLocation();
	const FVector End = Start + Character->GetFollowCamera()->GetForwardVector() * 50000.0f;
	GetWorld()->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);

	if (FireHit.bBlockingHit)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);
	}
}

void AItem::DecrementAmmo()
{
	/*if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		--Ammo;
	}*/
}
