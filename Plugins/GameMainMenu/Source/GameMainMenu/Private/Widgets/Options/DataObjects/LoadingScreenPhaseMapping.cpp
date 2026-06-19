// HangHang All Rights Reserved

#include "Widgets/Options/DataObjects/LoadingScreenPhaseMapping.h"
#include "Internationalization/StringTableRegistry.h"

FLoadingPhaseDisplayInfo ULoadingScreenPhaseMapping::FindDisplayInfo(FGameplayTag PhaseTag) const
{
	for (const FLoadingPhaseDisplayInfo& Entry : PhaseEntries)
	{
		if (Entry.PhaseTag == PhaseTag)
		{
			return Entry;
		}
	}

	FLoadingPhaseDisplayInfo Fallback;
	Fallback.PhaseTag = PhaseTag;
	Fallback.DisplayText = FText::FromStringTable(TEXT("/GameMainMenu/BP/StringTable/ST_GameMessages"), TEXT("Loading"));

	return Fallback;
}
