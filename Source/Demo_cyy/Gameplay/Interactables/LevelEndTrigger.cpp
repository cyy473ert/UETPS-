#include "LevelEndTrigger.h"

#include "Components/BoxComponent.h"
#include "Gameplay/Player/CYYCharacterFather.h"
#include "Gameplay/GameModes/CYYGameModeBase.h"
#include "Kismet/GameplayStatics.h"

ALevelEndTrigger::ALevelEndTrigger()
{
	EndBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EndBox"));
	SetRootComponent(EndBox);
	EndBox->SetBoxExtent(FVector(200.0f, 200.0f, 200.0f));
}

void ALevelEndTrigger::BeginPlay()
{
	Super::BeginPlay();

	EndBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelEndTrigger::OnOverlap);
}

void ALevelEndTrigger::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (bTriggered) return;
	if (!Cast<ACYYCharacterFather>(OtherActor)) return;

	bTriggered = true;

	ACYYGameModeBase* GM = Cast<ACYYGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		GM->TriggerVictory();
	}
}
