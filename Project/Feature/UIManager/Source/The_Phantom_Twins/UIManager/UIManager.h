#pragma once

#include "CoreMinimal.h"
#include "UIManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUIManager, Log, All);

UENUM(BlueprintType)
enum class EMessageTargetType : uint8
{
	Multicast		UMETA(DisplayName = "All Clients (Multicast)"),
	LocalClient		UMETA(DisplayName = "Local Client Only"),
	OnlyHost		UMETA(DisplayName = "Host Only"),
	AllExceptSelf	UMETA(DisplayName = "All Except This Client")
	// TargetClient  UMETA(DisplayName = "Specific Target Client") // This is Known Other PC. Our Game is not Necessary...
};

USTRUCT(BlueprintType)
struct FUIInformation
{
	GENERATED_BODY()
public:
	FUIInformation()
		: UI(nullptr)
		, Order(0)
		, bIsActivate(false)
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIManager")
	TObjectPtr<UUserWidget> UI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIManager")
	int32 Order;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIManager")
	bool bIsActivate;
};


UCLASS(Blueprintable)
class THE_PHANTOM_TWINS_API UUIManager : public UObject
{
    GENERATED_BODY()

public:
	void Initialize(APlayerController* InOwner);

    
	bool RegisterUI(const FString& Key, UUserWidget* Widget, int32 Order = 0);
	/**
	 * @brief 
			: The "UserWidget* widget" exists so that you don't delete incorrect data for temporary comparison.
	 * @param Key 
	 * @param Widget 
	 * @return 
	 */
	bool UnregisterUI(const FString& Key, UUserWidget* Widget);

	bool SetWidget(const FString& Key, bool bActive);
	


protected:
    UPROPERTY()
    TMap<FString, FUIInformation> UIMap;

	UPROPERTY()
	APlayerController* OwnerPC;

};
