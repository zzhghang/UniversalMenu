// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Widgets/Options/DataObjects/LoadingScreenPhaseMapping.h"
#include "FrontendLoadingScreenSubsystem.generated.h"

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
	
	void TryUpdateLoadingScreen();
	
	bool IsPreLoadScreenActive() const;
	
	bool ShouldShowLoadingScreen();
	
	bool CheckIsNeedShowLoadingScreen();
	
	void TryDisplayLoadingScreenIfNone();
	
	void TryRemoveLoadingScreen();

	void NotifyLoadingScreenVisibilityChanged(bool bIsVisible);

	bool bIsCurrentlyLoadingMap = false;

	double HoldLoadingScreenStartUpTime = -1.0;

	FGameplayTag CurrentLoadingPhase;

	TSharedPtr<SWidget> CachedLoadingScreenWidget = nullptr;
};
