// HangHang All Rights Reserved


#include "Subsystems/FrontendLoadingScreenSubsystem.h"

#include "PreLoadScreenManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "UObject/Class.h"
#include "FrontendGameplayTags.h"
#include "FrontendSettings/FrontendLoadingScreenSettings.h"
#include "Interface/FrontendLoadingScreenInterface.h"
#include "LogGameMainMenu.h"

bool UFrontendLoadingScreenSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClass;
		GetDerivedClasses(GetClass(), FoundClass);

		return FoundClass.IsEmpty();
	}

	return false;
}

void UFrontendLoadingScreenSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FCoreUObjectDelegates::PreLoadMapWithContext.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	if (LoadingScreenAssetHandle.IsValid())
	{
		LoadingScreenAssetHandle->CancelHandle();
		LoadingScreenAssetHandle.Reset();
	}

	TryRemoveLoadingScreen();

	CachedLoadingScreenClass = nullptr;
	CachedPhaseMapping.Reset();
}

void UFrontendLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &UFrontendLoadingScreenSubsystem::OnMapPreLoad);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UFrontendLoadingScreenSubsystem::OnMapPostLoad);

	PreloadLoadingScreenAssets();
}

void UFrontendLoadingScreenSubsystem::PreloadLoadingScreenAssets()
{
	const UFrontendLoadingScreenSettings* LoadScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();
	if (!LoadScreenSettings) return;

	TArray<FSoftObjectPath> AssetsToLoad;
	if (!LoadScreenSettings->LoadingScreenWidget.IsNull())
	{
		AssetsToLoad.Add(LoadScreenSettings->LoadingScreenWidget.ToSoftObjectPath());
	}
	if (!LoadScreenSettings->PhaseMapping.IsNull())
	{
		AssetsToLoad.Add(LoadScreenSettings->PhaseMapping.ToSoftObjectPath());
	}

	if (AssetsToLoad.IsEmpty()) return;

	if (!UAssetManager::GetIfInitialized()) return;

	LoadingScreenAssetHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
		AssetsToLoad,
		FStreamableDelegate::CreateUObject(this, &UFrontendLoadingScreenSubsystem::OnLoadingScreenAssetsLoaded)
	);
}

void UFrontendLoadingScreenSubsystem::OnLoadingScreenAssetsLoaded()
{
	const UFrontendLoadingScreenSettings* LoadScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();
	if (!LoadScreenSettings) return;

	if (UClass* WidgetClass = LoadScreenSettings->LoadingScreenWidget.Get())
	{
		CachedLoadingScreenClass = WidgetClass;
		bLoadingScreenUnavailable = false;
	}

	if (ULoadingScreenPhaseMapping* PhaseMapping = LoadScreenSettings->PhaseMapping.Get())
	{
		CachedPhaseMapping.Reset(PhaseMapping);
	}
}

UWorld* UFrontendLoadingScreenSubsystem::GetTickableGameObjectWorld() const
{
	if (UGameInstance* OwningGameInstance = GetGameInstance())
	{
		return OwningGameInstance->GetWorld();
	}

	return nullptr;
}

void UFrontendLoadingScreenSubsystem::Tick(float DeltaTime)
{
	const UFrontendLoadingScreenSettings* LoadScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();
	if (LoadScreenSettings && LoadScreenSettings->MaxLoadingScreenTime > 0.0f && LoadingScreenVisibleStartTime >= 0.0)
	{
		const double ElapsedTime = FPlatformTime::Seconds() - LoadingScreenVisibleStartTime;
		if (ElapsedTime > LoadScreenSettings->MaxLoadingScreenTime)
		{
			UE_LOG(LogGameMainMenu, Warning, TEXT("FrontendLoadingScreen: Loading screen stayed up for %.1fs (MaxLoadingScreenTime %.1fs). Force hiding."), ElapsedTime, LoadScreenSettings->MaxLoadingScreenTime);

			bIsCurrentlyLoadingMap = false;
			HideLoadingScreenImmediately(true);
			return;
		}
	}

	TryUpdateLoadingScreen();
}

ETickableTickType UFrontendLoadingScreenSubsystem::GetTickableTickType() const
{
	if (IsTemplate())
	{
		return ETickableTickType::Never;
	}

	return ETickableTickType::Conditional;
}

bool UFrontendLoadingScreenSubsystem::IsTickable() const
{
	return GetGameInstance() && GetGameInstance()->GetGameViewportClient();
}

TStatId UFrontendLoadingScreenSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFrontendLoadingScreenSubsystem,STATGROUP_Tickables);
}

UFrontendLoadingScreenSubsystem::~UFrontendLoadingScreenSubsystem()
{
}

void UFrontendLoadingScreenSubsystem::OnMapPreLoad(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance != GetGameInstance()) return;

	SetTickableTickType(ETickableTickType::Conditional);

	bIsCurrentlyLoadingMap = true;
	bLoadingScreenSuppressed = false;

	HoldLoadingScreenStartUpTime = -1.0;
	HideRequestedTime = -1.0;
	LastBroadcastLoadingPhase = FGameplayTag();

	TryUpdateLoadingScreen();
}

void UFrontendLoadingScreenSubsystem::OnMapPostLoad(UWorld* LoadedWorld)
{
	if (!LoadedWorld)
	{
		// PostLoadMapWithWorld is broadcast with a null world when a map load is cancelled or fails.
		// Without this the subsystem would keep bIsCurrentlyLoadingMap set forever and stay black.
		// Ignore null broadcasts that do not belong to the load this subsystem started (e.g. another PIE client).
		if (!bIsCurrentlyLoadingMap) return;

		bIsCurrentlyLoadingMap = false;

		UE_LOG(LogGameMainMenu, Warning, TEXT("FrontendLoadingScreen: Map load finished without a world. Hiding the loading screen."));

		HideLoadingScreenImmediately(true);
		return;
	}

	if (LoadedWorld->GetGameInstance() == GetGameInstance())
	{
		bIsCurrentlyLoadingMap = false;

		// A force-hidden (stuck) load must not re-enable ticking just to no-op every frame.
		// OnMapPreLoad clears the suppression for the next load.
		if (bLoadingScreenSuppressed) return;

		// Re-enable ticking: the screen may have been hidden (and ticking stopped) before the
		// viewport existed, and the post-load phases are driven from Tick.
		SetTickableTickType(ETickableTickType::Conditional);

		TryUpdateLoadingScreen();
	}
}

void UFrontendLoadingScreenSubsystem::TryUpdateLoadingScreen()
{
	if (bLoadingScreenSuppressed)
	{
		return;
	}

	if (IsPreLoadScreenActive())
	{
		return;
	}

	if (ShouldShowLoadingScreen())
	{
		HideRequestedTime = -1.0;

		if (LoadingScreenVisibleStartTime < 0.0)
		{
			LoadingScreenVisibleStartTime = FPlatformTime::Seconds();
		}

		UGameViewportClient* ViewportClient = GetGameInstance()->GetGameViewportClient();
		if (ViewportClient)
		{
			const bool bInActualLoadingPhase =
				CurrentLoadingPhase != FrontendGameplayTags::Frontend_Loading_Phase_TextureStreaming;
			ViewportClient->bDisableWorldRendering = bInActualLoadingPhase;
		}

		TryDisplayLoadingScreenIfNone();

		// Only broadcast when the phase actually changes. The loading screen sits on screen for
		// HoldLoadingScreenTime, so broadcasting every tick made the widget re-query its display
		// info (and re-set its text) at frame rate.
		if (CurrentLoadingPhase != LastBroadcastLoadingPhase)
		{
			LastBroadcastLoadingPhase = CurrentLoadingPhase;
			OnLoadingPhaseUpdated.Broadcast(CurrentLoadingPhase);
		}
	}
	else
	{
		HideLoadingScreenImmediately(false);
	}
}

void UFrontendLoadingScreenSubsystem::HideLoadingScreenImmediately(bool bForce)
{
	TryRemoveLoadingScreen();

	// Do not reset HoldLoadingScreenStartUpTime here. While waiting for late player controllers
	// this function is called once per tick, and resetting it would restart the hold timer and
	// make ShouldShowLoadingScreen return true again (hide/show loop). OnMapPreLoad resets it
	// for the next load instead.
	LoadingScreenVisibleStartTime = -1.0;
	LastBroadcastLoadingPhase = FGameplayTag();

	if (bForce)
	{
		// The load may still be running and will broadcast PostLoadMap later; do not let that
		// callback re-display the widget after the tick has been stopped.
		bLoadingScreenSuppressed = true;
	}

	// The client's player controller can replicate in after the map load finished. Keep ticking
	// until it exists so the deactivated notification still reaches it (the frontend menu is
	// created there), but do not wait forever if it never arrives.
	if (!bForce && bLoadingScreenWasShown && !AllLocalPlayersHaveControllers())
	{
		const UFrontendLoadingScreenSettings* LoadScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();
		const double MaxWaitTime = LoadScreenSettings ? LoadScreenSettings->MaxWaitForPlayerControllerTime : 30.0;
		const double CurrentTime = FPlatformTime::Seconds();

		if (HideRequestedTime < 0.0)
		{
			HideRequestedTime = CurrentTime;
		}

		if (MaxWaitTime <= 0.0 || CurrentTime - HideRequestedTime < MaxWaitTime)
		{
			return;
		}

		UE_LOG(LogGameMainMenu, Warning, TEXT("FrontendLoadingScreen: Timed out after %.1fs waiting for local player controllers; deactivated notification may be missed."), CurrentTime - HideRequestedTime);
	}

	HideRequestedTime = -1.0;

	if (bLoadingScreenWasShown)
	{
		NotifyLoadingScreenVisibilityChanged(false);
		bLoadingScreenWasShown = false;
	}

	SetTickableTickType(ETickableTickType::Never);
}

bool UFrontendLoadingScreenSubsystem::IsPreLoadScreenActive() const
{
	if (FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get())
	{
		return PreLoadScreenManager->HasValidActivePreLoadScreen();
	}

	return false;
}

bool UFrontendLoadingScreenSubsystem::ShouldShowLoadingScreen()
{
	const UFrontendLoadingScreenSettings* LoadScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();

	if (GIsEditor && !LoadScreenSettings->bShowLoadingScreenInEditor) return false;

	UGameViewportClient* ViewportClient = GetGameInstance()->GetGameViewportClient();
	if (!ViewportClient) return false;

	if (CheckIsNeedShowLoadingScreen())
	{
		return true;
	}

	const double CurrentTime = FPlatformTime::Seconds();

	if (HoldLoadingScreenStartUpTime < 0.0)
	{
		HoldLoadingScreenStartUpTime = CurrentTime;
	}

	const double ElapsedTime = CurrentTime - HoldLoadingScreenStartUpTime;
	return ElapsedTime < LoadScreenSettings->HoldLoadingScreenTime;
}

bool UFrontendLoadingScreenSubsystem::CheckIsNeedShowLoadingScreen()
{
	if (bIsCurrentlyLoadingMap)
	{
		CurrentLoadingPhase = FrontendGameplayTags::Frontend_Loading_Phase_LevelLoad;
		return true;
	}

	UWorld* OwningWorld = GetGameInstance()->GetWorld();

	if (!OwningWorld)
	{
		CurrentLoadingPhase = FrontendGameplayTags::Frontend_Loading_Phase_WorldInit;
		return true;
	}

	if (!OwningWorld->HasBegunPlay())
	{
		CurrentLoadingPhase = FrontendGameplayTags::Frontend_Loading_Phase_BeginPlay;
		return true;
	}

	if (!OwningWorld->GetFirstPlayerController())
	{
		CurrentLoadingPhase = FrontendGameplayTags::Frontend_Loading_Phase_PlayerController;
		return true;
	}

	//todo other check

	CurrentLoadingPhase = FrontendGameplayTags::Frontend_Loading_Phase_TextureStreaming;
	return false;
}

UClass* UFrontendLoadingScreenSubsystem::ResolveLoadingScreenClass()
{
	if (CachedLoadingScreenClass)
	{
		return CachedLoadingScreenClass.Get();
	}

	if (bLoadingScreenUnavailable)
	{
		return nullptr;
	}

	// Do not fall back to a synchronous load while the preload is still in flight: that is the
	// frame hitch this cache exists to avoid. Retry on the next tick instead.
	if (LoadingScreenAssetHandle.IsValid() && LoadingScreenAssetHandle->IsLoadingInProgress())
	{
		return nullptr;
	}

	const UFrontendLoadingScreenSettings* LoadScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();
	if (!LoadScreenSettings) return nullptr;

	UClass* WidgetClass = LoadScreenSettings->GetSoftLoadScreenWidgetClassCheck().Get();
	if (!WidgetClass)
	{
		UE_LOG(LogGameMainMenu, Error, TEXT("FrontendLoadingScreen: Failed to load the loading screen widget class; the loading screen is disabled."));

		bLoadingScreenUnavailable = true;
		return nullptr;
	}

	CachedLoadingScreenClass = WidgetClass;
	return WidgetClass;
}

void UFrontendLoadingScreenSubsystem::TryDisplayLoadingScreenIfNone()
{
	if (CachedLoadingScreenWidget)
	{
		return;
	}

	UClass* LoadWidgetClass = ResolveLoadingScreenClass();
	if (!LoadWidgetClass)
	{
		return;
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return;

	UUserWidget* CreatedLoadingWidget = UUserWidget::CreateWidgetInstance(*GameInstance, LoadWidgetClass, NAME_None);

	if (!CreatedLoadingWidget)
	{
		UE_LOG(LogGameMainMenu, Error, TEXT("FrontendLoadingScreen: Failed to create the loading screen widget; the loading screen is disabled."));

		bLoadingScreenUnavailable = true;
		return;
	}

	CachedLoadingScreenWidget = CreatedLoadingWidget->TakeWidget();

	UGameViewportClient* ViewportClient = GameInstance->GetGameViewportClient();
	if (!ViewportClient)
	{
		CachedLoadingScreenWidget.Reset();
		return;
	}

	ViewportClient->AddViewportWidgetContent(
		CachedLoadingScreenWidget.ToSharedRef(),1000
	);

	bLoadingScreenWasShown = true;

	// Force the current phase through to the widget that was just constructed. With the
	// change-only broadcast below, the widget has missed every previous broadcast.
	LastBroadcastLoadingPhase = FGameplayTag();

	NotifyLoadingScreenVisibilityChanged(true);
}

void UFrontendLoadingScreenSubsystem::TryRemoveLoadingScreen()
{
	UGameInstance* GameInstance = GetGameInstance();
	UGameViewportClient* ViewportClient = GameInstance ? GameInstance->GetGameViewportClient() : nullptr;

	if (ViewportClient)
	{
		// Always restore world rendering, even if the widget never got created. Otherwise a
		// loading screen that was requested but failed to spawn leaves the game black.
		ViewportClient->bDisableWorldRendering = false;

		if (CachedLoadingScreenWidget)
		{
			ViewportClient->RemoveViewportWidgetContent(
				CachedLoadingScreenWidget.ToSharedRef()
			);
		}
	}

	CachedLoadingScreenWidget.Reset();
}

bool UFrontendLoadingScreenSubsystem::AllLocalPlayersHaveControllers() const
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return false;

	for (ULocalPlayer* LocalPlayer : GameInstance->GetLocalPlayers())
	{
		if (!LocalPlayer || !LocalPlayer->GetPlayerController(GameInstance->GetWorld()))
		{
			return false;
		}
	}

	return true;
}

void UFrontendLoadingScreenSubsystem::NotifyLoadingScreenVisibilityChanged(bool bIsVisible)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return;

	for (ULocalPlayer* LocalPlayer : GameInstance->GetLocalPlayers())
	{
		if (!LocalPlayer)
		{
			continue;
		}

		if (APlayerController* PC = LocalPlayer->GetPlayerController(GameInstance->GetWorld()))
		{
			//PC
			if (PC->Implements<UFrontendLoadingScreenInterface>())
			{
				if (bIsVisible)
				{
					IFrontendLoadingScreenInterface::Execute_OnLoadingScreenActivated(PC);
				}
				else
				{
					IFrontendLoadingScreenInterface::Execute_OnLoadingScreenDeactivated(PC);
				}
			}
			//Pawn
			if (APawn* OwningPawn = PC->GetPawn())
			{
				if (OwningPawn->Implements<UFrontendLoadingScreenInterface>())
				{
					if (bIsVisible)
					{
						IFrontendLoadingScreenInterface::Execute_OnLoadingScreenActivated(OwningPawn);
					}
					else
					{
						IFrontendLoadingScreenInterface::Execute_OnLoadingScreenDeactivated(OwningPawn);
					}
				}
			}
			//todo other object to notify
			
		}
	}
}

FLoadingPhaseDisplayInfo UFrontendLoadingScreenSubsystem::GetLoadingPhaseDisplayInfo(FGameplayTag PhaseTag) const
{
	if (!CachedPhaseMapping)
	{
		const UFrontendLoadingScreenSettings* LoadScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();

		if (LoadScreenSettings)
		{
			if (ULoadingScreenPhaseMapping* PhaseMapping = LoadScreenSettings->PhaseMapping.LoadSynchronous())
			{
				CachedPhaseMapping.Reset(PhaseMapping);
			}
		}
	}

	if (CachedPhaseMapping)
	{
		return CachedPhaseMapping->FindDisplayInfo(PhaseTag);
	}

	return ULoadingScreenPhaseMapping::StaticClass()->GetDefaultObject<ULoadingScreenPhaseMapping>()->FindDisplayInfo(PhaseTag);
}
