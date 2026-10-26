// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/Options/DataObjects/LoadingScreenPhaseMapping.h"
#include "FrontendLoadingScreenSubsystem.generated.h"

struct FStreamableHandle;

/**
 * 
 */
UCLASS()
class GAMEMAINMENU_API UFrontendLoadingScreenSubsystem : public UGameInstanceSubsystem,public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingPhaseUpdatedDelegate, FGameplayTag, PhaseTag);

	// begin USubsystem interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
	virtual void Deinitialize() override;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// end USubsystem interface

	// begin FTickableGameObject interface
	virtual UWorld* GetTickableGameObjectWorld() const;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual ETickableTickType GetTickableTickType() const override;
	
	virtual bool IsTickable() const override;
	
	virtual TStatId GetStatId() const override;
	// End FTickableGameObject interface

	~UFrontendLoadingScreenSubsystem();
	
public:
	
	UPROPERTY(BlueprintAssignable)
	FOnLoadingPhaseUpdatedDelegate OnLoadingPhaseUpdated;

	UFUNCTION(BlueprintPure)
	FLoadingPhaseDisplayInfo GetLoadingPhaseDisplayInfo(FGameplayTag PhaseTag) const;

private:

	void OnMapPreLoad(const FWorldContext& WorldContext, const FString&  MapName );
	
	void OnMapPostLoad(UWorld* LoadedWorld);

	// Preloads the widget class and phase mapping off the map-load path. Without this the
	// first PreLoadMap synchronously loads WBP_LoadingScreen and stalls the frame that is
	// supposed to show it.
	void PreloadLoadingScreenAssets();

	void OnLoadingScreenAssetsLoaded();
	
	void TryUpdateLoadingScreen();

	// Central hide path: restores world rendering, clears timers, delivers the deactivated
	// notification once, and stops ticking. bForce skips waiting for late local players.
	void HideLoadingScreenImmediately(bool bForce);
	
	bool IsPreLoadScreenActive() const;
	
	bool ShouldShowLoadingScreen();
	
	bool CheckIsNeedShowLoadingScreen();

	UClass* ResolveLoadingScreenClass();
	
	void TryDisplayLoadingScreenIfNone();
	
	void TryRemoveLoadingScreen();

	void NotifyLoadingScreenVisibilityChanged(bool bIsVisible);

	bool AllLocalPlayersHaveControllers() const;

	bool bIsCurrentlyLoadingMap = false;

	double HoldLoadingScreenStartUpTime = -1.0;

	// Time we started showing (or trying to show) the loading screen; -1 when hidden.
	double LoadingScreenVisibleStartTime = -1.0;

	// Time we first entered the "hide but wait for player controllers" state; -1 otherwise.
	double HideRequestedTime = -1.0;

	FGameplayTag CurrentLoadingPhase;

	FGameplayTag LastBroadcastLoadingPhase;

	bool bLoadingScreenWasShown = false;

	bool bLoadingScreenUnavailable = false;

	// Set when a stuck/failed load force-hides the screen. Keeps the late PostLoadMap callback
	// from re-displaying the widget after the subsystem stopped ticking.
	bool bLoadingScreenSuppressed = false;

	TSharedPtr<SWidget> CachedLoadingScreenWidget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UClass> CachedLoadingScreenClass = nullptr;

	mutable TStrongObjectPtr<ULoadingScreenPhaseMapping> CachedPhaseMapping;

	TSharedPtr<FStreamableHandle> LoadingScreenAssetHandle;
};
