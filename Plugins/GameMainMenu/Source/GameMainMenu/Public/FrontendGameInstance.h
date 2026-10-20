// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "FrontendGameInstance.generated.h"

class UMenuSessionData;

UCLASS(BlueprintType, Blueprintable)
class GAMEMAINMENU_API UFrontendGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public:

	// Begin UGameInstance Interface
	virtual void Init() override;
	// End UGameInstance Interface

	UFUNCTION(BlueprintPure, Category = "MenuSession")
	UMenuSessionData* GetMenuSessionData() const { return CachedMenuSessionData; }

	UFUNCTION(BlueprintCallable, Category = "MenuSession")
	UMenuSessionData* GetOrCreateMenuSessionData();

	UFUNCTION(BlueprintCallable, Category = "MenuSession")
	UMenuSessionData* LoadMenuSessionData();

	UFUNCTION(BlueprintCallable, Category = "MenuSession")
	void SaveMenuSessionData();

	UFUNCTION(BlueprintCallable, Category = "MenuSession")
	bool TrySaveMenuSessionData();

private:

	/** Runs the hardware benchmark once on first launch and applies the detected quality levels. */
	void RunHardwareBenchmarkIfNeeded();

	UPROPERTY(Transient)
	UMenuSessionData* CachedMenuSessionData = nullptr;
};
