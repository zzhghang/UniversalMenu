// HangHang All Rights Reserved

#include "Widgets/Credits/W_CreditsScreen.h"
#include "Widgets/Credits/CreditsDataAsset.h"
#include "CommonTextBlock.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UW_CreditsScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UW_CreditsScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	CurrentScrollOffset = 0.f;

	if (CreditsScrollBox)
	{
		CreditsScrollBox->SetScrollOffset(0.f);
	}

	PopulateCreditsEntries();

	if (UWorld* World = GetWorld())
	{
		const float Delay = CreditsData ? CreditsData->StartDelay : 1.f;
		World->GetTimerManager().SetTimer(StartDelayHandle, this, &UW_CreditsScreen::StartScrolling, Delay, false);
	}
}

void UW_CreditsScreen::NativeOnDeactivated()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(StartDelayHandle);
		World->GetTimerManager().ClearTimer(ScrollTickHandle);
	}

	Super::NativeOnDeactivated();
}

void UW_CreditsScreen::PopulateCreditsEntries()
{
	if (!CreditsContentBox || !CreditsData)
	{
		return;
	}

	CreditsContentBox->ClearChildren();

	for (const FCreditsEntry& Entry : CreditsData->Entries)
	{
		const FText LineText = FText::Format(FText::FromString(TEXT("{0} : {1}")), Entry.Role, Entry.Name);

		UCommonTextBlock* LineBlock = NewObject<UCommonTextBlock>(CreditsContentBox);
		LineBlock->SetText(LineText);
		LineBlock->SetJustification(ETextJustify::Center);

		if (CreditsTextStyle)
		{
			LineBlock->SetStyle(CreditsTextStyle);
		}

		UVerticalBoxSlot* VBoxSlot = CreditsContentBox->AddChildToVerticalBox(LineBlock);
		VBoxSlot->SetHorizontalAlignment(HAlign_Center);
	}
}

void UW_CreditsScreen::StartScrolling()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ScrollTickHandle, this, &UW_CreditsScreen::UpdateScroll, ScrollTickInterval, true);
	}
}

void UW_CreditsScreen::UpdateScroll()
{
	if (!CreditsScrollBox || !CreditsData)
	{
		return;
	}

	CurrentScrollOffset += CreditsData->ScrollSpeed * ScrollTickInterval;
	CreditsScrollBox->SetScrollOffset(CurrentScrollOffset);

	const float ScrollEnd = CreditsScrollBox->GetScrollOffsetOfEnd();
	if (CurrentScrollOffset >= ScrollEnd)
	{
		OnCreditsFinished();
	}
}

void UW_CreditsScreen::OnCreditsFinished()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ScrollTickHandle);
	}

	if (bAutoReturnOnFinish)
	{
		DeactivateWidget();
	}
}
