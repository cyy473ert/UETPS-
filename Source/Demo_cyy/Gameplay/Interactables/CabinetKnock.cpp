#include "CabinetKnock.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Gameplay/Player/CYYCharacterFather.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

ACabinetKnock::ACabinetKnock()
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	CabinetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CabinetMesh"));
	CabinetMesh->SetupAttachment(RootComp);

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetupAttachment(RootComp);
	TriggerSphere->SetSphereRadius(300.0f);
}

void ACabinetKnock::BeginPlay()
{
	Super::BeginPlay();

	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ACabinetKnock::OnPlayerApproach);
}

void ACabinetKnock::OnPlayerApproach(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (bTriggered) return;
	if (!Cast<ACYYCharacterFather>(OtherActor)) return;

	bTriggered = true;
	KnockStep = 1;
	PlayKnock();
}

void ACabinetKnock::PlayKnock()
{
	switch (KnockStep)
	{
	case 1:
		if (KnockSound1)
			UGameplayStatics::PlaySoundAtLocation(this, KnockSound1, GetActorLocation());
		KnockStep++;
		GetWorldTimerManager().SetTimer(KnockTimer, this, &ACabinetKnock::PlayKnock, 1.5f, false);
		break;
	case 2:
		if (KnockSound2)
			UGameplayStatics::PlaySoundAtLocation(this, KnockSound2, GetActorLocation());
		KnockStep++;
		GetWorldTimerManager().SetTimer(KnockTimer, this, &ACabinetKnock::PlayKnock, 1.5f, false);
		break;
	case 3:
		if (KnockSound3)
			UGameplayStatics::PlaySoundAtLocation(this, KnockSound3, GetActorLocation());
		KnockStep++;
		GetWorldTimerManager().SetTimer(KnockTimer, this, &ACabinetKnock::PlayKnock, 1.0f, false);
		break;
	case 4:
		if (VoiceLine)
			UGameplayStatics::PlaySoundAtLocation(this, VoiceLine, GetActorLocation());
		KnockStep++;
		GetWorldTimerManager().SetTimer(KnockTimer, this, &ACabinetKnock::PlayKnock, 3.0f, false);
		break;
	case 5:
		if (LoudKnockSound)
			UGameplayStatics::PlaySoundAtLocation(this, LoudKnockSound, GetActorLocation());
		KnockStep++;
		GetWorldTimerManager().SetTimer(KnockTimer, this, &ACabinetKnock::PlayKnock, 2.0f, false);
		break;
	case 6:
		if (LoudKnockSound)
			UGameplayStatics::PlaySoundAtLocation(this, LoudKnockSound, GetActorLocation());
		KnockStep++;
		break;
	default:
		break;
	}
}
