// HangHang All Rights Reserved

#include "FrontendGameInstance.h"

#include "FrontendSettings/FrontendGameUserSettings.h"
#include "LogGameMainMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Multiplayer/MenuSessionData.h"

namespace
{
	constexpr const TCHAR* MenuSessionSlotName = TEXT("MenuSession");
	constexpr int32 MenuSessionSlotIndex = 0;
}

void UFrontendGameInstance::Init()
{
	Super::Init();

	RunHardwareBenchmarkIfNeeded();
}

void UFrontendGameInstance::RunHardwareBenchmarkIfNeeded()
{
	// Dedicated servers have no rendering and no local player, so there is nothing to benchmark.
	if (IsDedicatedServerInstance())
	{
		return;
	}

	UFrontendGameUserSettings* UserSettings = UFrontendGameUserSettings::Get();
	if (!UserSettings) return;

	// Only on first launch: ApplyHardwareBenchmarkResults saves the results, so later runs skip this.
	// RunHardwareBenchmark blocks while it measures CPU and GPU, so it runs once during startup
	// instead of on every frontend map load.
	if (UserSettings->GetLastCPUBenchmarkResult() < 0.f || UserSettings->GetLastGPUBenchmarkResult() < 0.f)
	{
		UserSettings->RunHardwareBenchmark();
		UserSettings->ApplyHardwareBenchmarkResults();
	}
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
	if (!bSaved) UE_LOG(LogGameMainMenu, Warning, TEXT("Failed to save MenuSession settings."));
	return bSaved;
}
