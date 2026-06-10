// HangHang All Rights Reserved


#include "Widgets/Options/DataObjects/ListDataObject_StringResolution.h"

#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"

void UListDataObject_StringResolution::InitResolutionValues()
{
	TArray<FIntPoint> SupportedResolutions;
	
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(SupportedResolutions);

	SupportedResolutions.Sort(
		[](const FIntPoint& A, const FIntPoint& B)->bool
		{
			return A.SizeSquared() < B.SizeSquared();
		});
	for (auto SupportedResolution : SupportedResolutions)
	{
		AddDynamicString(ResolutionToValueString(SupportedResolution),ResolutionToDisplayText(SupportedResolution));
	}

	MaxAllowedResolution = ResolutionToValueString(SupportedResolutions.Last());

	SetDefaultValueFromString(MaxAllowedResolution);
}

void UListDataObject_StringResolution::OnDataObjectInitialized()
{
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentTextValue = ResolutionToDisplayText(UFrontendGameUserSettings::Get()->GetScreenResolution());
	}
}

FString UListDataObject_StringResolution::ResolutionToValueString(const FIntPoint& InResolution) const
{
	//Dynamic Getter : (X=1920,Y=1080)
	return FString::Printf(TEXT("(X=%d,Y=%d)"),InResolution.X,InResolution.Y);
}

FText UListDataObject_StringResolution::ResolutionToDisplayText(const FIntPoint& InResolution) const
{
	const FString DisplayString = FString::Printf(TEXT("%d x %d"),InResolution.X,InResolution.Y);

	return FText::FromString(DisplayString);
}
