#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TerminalScreenWidget.generated.h"

class UTextBlock;

UCLASS()
class DEMO_CYY_API UTerminalScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Refresh(bool bPower, bool bCard);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PowerLine;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CardLine;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DoorLine;
};
