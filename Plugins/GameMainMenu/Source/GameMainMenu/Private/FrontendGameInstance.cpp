// HangHang All Rights Reserved

#include "FrontendGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Widgets/Multiplayer/MenuSessionData.h"

namespace
{
	constexpr const TCHAR* MenuSessionSlotName = TEXT("MenuSession");
	constexpr int32 MenuSessionSlotIndex = 0;
}

UMenuSessionData* UFrontendGameInstance::GetOrCreateMenuSessionData()
{
	return LoadMenuSessionData();
}

UMenuSessionData* UFrontendGameInstance::LoadMenuSessionData()
{
	if (!CachedMenuSessionData)
	{
		if (UGameplayStatics::DoesSaveGameExist(MenuSessionSlotName, MenuSessionSlotIndex))
			CachedMenuSessionData = Cast<UMenuSessionData>(UGameplayStatics::LoadGameFromSlot(MenuSessionSlotName, MenuSessionSlotIndex));
		if (!CachedMenuSessionData)
			CachedMenuSessionData = Cast<UMenuSessionData>(UGameplayStatics::CreateSaveGameObject(UMenuSessionData::StaticClass()));
	}
	return CachedMenuSessionData;
}

void UFrontendGameInstance::SaveMenuSessionData()
{
	TrySaveMenuSessionData();
}

bool UFrontendGameInstance::TrySaveMenuSessionData()
{
	const bool bSaved = CachedMenuSessionData && UGameplayStatics::SaveGameToSlot(CachedMenuSessionData, MenuSessionSlotName, MenuSessionSlotIndex);
	if (!bSaved) UE_LOG(LogTemp, Warning, TEXT("Failed to save MenuSession settings."));
	return bSaved;
}
