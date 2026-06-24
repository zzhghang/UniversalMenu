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

	if (!CreditsData)
	{
		return;
	}

	CurrentScrollOffset = 0.f;

	if (CreditsScrollBox)
	{
		CreditsScrollBox->SetScrollOffset(0.f);
	}

	PopulateCreditsEntries();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(StartDelayHandle, this, &UW_CreditsScreen::StartScrolling, CreditsData->StartDelay, false);
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

void UW_CreditsScreen::AddSpacerLines(int32 Count)
{
	for (int32 i = 0; i < Count; ++i)
	{
		UCommonTextBlock* Spacer = NewObject<UCommonTextBlock>(CreditsContentBox);
		Spacer->SetText(FText::FromString(TEXT(" ")));
		if (CreditsTextStyle)
		{
			Spacer->SetStyle(CreditsTextStyle);
		}
		CreditsContentBox->AddChildToVerticalBox(Spacer);
	}
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
		VBoxSlot->SetPadding(FMargin(0.f, 0.f, 0.f, CreditsData->EntrySpacing));
		VBoxSlot->SetHorizontalAlignment(HAlign_Center);
	}

	if (UELogoImage)
	{
		UVerticalBoxSlot* LogoSlog = CreditsContentBox->AddChildToVerticalBox(UELogoImage);
		LogoSlog->SetPadding(FMargin(0.f, CreditsData->LogoSpacing, 0.f, 0.f));
		LogoSlog->SetHorizontalAlignment(HAlign_Center);
	}
}

void UW_CreditsScreen::StartScrolling()
{
	if (UELogoImage && CreditsScrollBox && CreditsData)
	{
		const FGeometry& LogoGeo = UELogoImage->GetCachedGeometry();
		const FGeometry& ScrollGeo = CreditsScrollBox->GetCachedGeometry();
		const float LogoH = LogoGeo.GetLocalSize().Y;
		const float ViewportH = ScrollGeo.GetLocalSize().Y;

		float SpacerPx = 0.f;
		if (LogoH > 0.f && ViewportH > 0.f)
		{
			SpacerPx = (ViewportH - LogoH) * 0.5f;
		}
		else if (ViewportH > 0.f)
		{
			SpacerPx = ViewportH * 0.5f;
		}

		if (SpacerPx > 0.f)
		{
			const int32 SpacerCount = FMath::Max(0, FMath::CeilToInt(SpacerPx / CreditsData->LineHeight));
			AddSpacerLines(SpacerCount);
		}
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ScrollTickHandle, this, &UW_CreditsScreen::UpdateScroll, 0.016f, true);
	}
}

void UW_CreditsScreen::UpdateScroll()
{
	if (!CreditsScrollBox || !CreditsData)
	{
		return;
	}

	const float DeltaTime = FMath::Max(GetWorld()->GetDeltaSeconds(), 0.001f);
	CurrentScrollOffset += CreditsData->ScrollSpeed * DeltaTime;
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
