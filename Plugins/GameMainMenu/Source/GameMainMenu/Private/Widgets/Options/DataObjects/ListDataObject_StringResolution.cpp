// HangHang All Rights Reserved
#include "Widgets/Options/DataObjects/ListDataObject_StringResolution.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "FrontendSettings/FrontendResolution.h"
#include "Kismet/KismetSystemLibrary.h"

void UListDataObject_StringResolution::InitResolutionValues()
{
	TArray<FIntPoint> Resolutions;
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
	const UFrontendGameUserSettings* Settings = UFrontendGameUserSettings::Get();
	InitResolutionValues(Resolutions, Settings ? Settings->GetDesktopResolution() : FIntPoint::ZeroValue);
}

void UListDataObject_StringResolution::InitResolutionValues(const TArray<FIntPoint>& Resolutions, FIntPoint FallbackResolution)
{
	TArray<FIntPoint> ValidResolutions;
	for (const FIntPoint& Size : Resolutions)
		if (FrontendResolution::IsUsable(Size)) ValidResolutions.AddUnique(Size);
	if (ValidResolutions.IsEmpty())
		ValidResolutions.Add(FrontendResolution::IsUsable(FallbackResolution) ? FallbackResolution : FIntPoint(1920, 1080));
	ValidResolutions.Sort([](const FIntPoint& A, const FIntPoint& B)
	{
		const int64 AreaA = static_cast<int64>(A.X) * A.Y;
		const int64 AreaB = static_cast<int64>(B.X) * B.Y;
		return AreaA == AreaB ? A.X < B.X : AreaA < AreaB;
	});
	StringArray.Reset();
	TextArray.Reset();
	for (const FIntPoint& Size : ValidResolutions)
		AddDynamicString(ResolutionToValueString(Size), ResolutionToDisplayText(Size));
	MaxAllowedResolution = ResolutionToValueString(ValidResolutions.Last());
	SetDefaultValueFromString(MaxAllowedResolution);
}

void UListDataObject_StringResolution::OnDataObjectInitialized()
{
	if (StringArray.IsEmpty()) InitResolutionValues();
	FIntPoint Default = FIntPoint::ZeroValue;
	if (!HasDefaultValue() || !FrontendResolution::Parse(GetDefaultStringValue(), Default))
	{
		if (!FrontendResolution::Parse(MaxAllowedResolution, Default)) Default = FIntPoint(1920, 1080);
		SetDefaultValueFromString(ResolutionToValueString(Default));
	}
	const FString DefaultKey = ResolutionToValueString(Default);
	if (!StringArray.Contains(DefaultKey)) AddDynamicString(DefaultKey, ResolutionToDisplayText(Default));
	Super::OnDataObjectInitialized();
	FIntPoint Current = FIntPoint::ZeroValue;
	if (!FrontendResolution::Parse(CurrentStringValue, Current)) Current = Default;
	// Editor windows can have valid sizes absent from fullscreen enumeration.
	// Read the draft binding; never substitute the global live settings here.
	CurrentStringValue = ResolutionToValueString(Current);
	if (!StringArray.Contains(CurrentStringValue)) AddDynamicString(CurrentStringValue, ResolutionToDisplayText(Current));
	TrySetDisplayTextFromStringValue(CurrentStringValue);
}

FString UListDataObject_StringResolution::ResolutionToValueString(const FIntPoint& Size) const
{
	return FString::Printf(TEXT("(X=%d,Y=%d)"), Size.X, Size.Y);
}

FText UListDataObject_StringResolution::ResolutionToDisplayText(const FIntPoint& Size) const
{
	return FText::FromString(FString::Printf(TEXT("%d x %d"), Size.X, Size.Y));
}
