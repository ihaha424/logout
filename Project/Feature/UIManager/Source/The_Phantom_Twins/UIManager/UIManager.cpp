
#include "UIManager.h"
#include "Blueprint/UserWidget.h"

DEFINE_LOG_CATEGORY(LogUIManager);

void UUIManager::Initialize(APlayerController* InOwner)
{
	if (!InOwner)
	{
		UE_LOG(LogUIManager, Error, TEXT("Initialize: InOwner is Nullptr."));
	}
	OwnerPC = InOwner;
}

bool UUIManager::RegisterUI(const FString& Key, UUserWidget* Widget, int32 Order)
{
	if (nullptr == Widget)
	{
		UE_LOG(LogUIManager, Error, TEXT("RegisterUI: Widget is nullptr."));
		return false;
	}
	if (Order < 0)
	{
		UE_LOG(LogUIManager, Error, TEXT("RegisterUI: Order is invaild."));
		return false;
	}
	if (UIMap.Contains(Key))
	{
		UE_LOG(LogUIManager, Log, TEXT("RegisterUI: Key is already Register."));
		return false;
	}

	FUIInformation UIInfo;
	UIInfo.Order = Order;
	UIInfo.UI = Widget;
	UIInfo.bIsActivate = false;

	UIMap.Add(Key, UIInfo);

	return true;
}

bool UUIManager::UnregisterUI(const FString& Key, UUserWidget* Widget)
{
	if (nullptr == Widget)
	{
		UE_LOG(LogUIManager, Error, TEXT("UnRegisterUI: Widget is nullptr."));
		return false;
	}
	if (UIMap.Contains(Key))
	{
		UE_LOG(LogUIManager, Log, TEXT("UnRegisterUI: Key is  Non-Register."));
		return false;
	}
	if (UIMap[Key].UI != Widget)
	{
		UE_LOG(LogUIManager, Warning, TEXT("UnRegisterUI: The data in the key and the data in the widget do not match."));
		return false;
	}

	UIMap.Remove(Key);

	return true;
}

bool UUIManager::SetWidget(const FString& Key, bool bActive)
{
	if (!UIMap.Contains(Key))
	{
		UE_LOG(LogUIManager, Warning, TEXT("SetWidget: %s is Non-Register."), *Key);
		return false;
	}

	if (bActive)
	{
		if (UIMap[Key].bIsActivate)
		{
			UE_LOG(LogUIManager, Log, TEXT("SetWidget: %s is already Show."), *Key);
			return false;
		}
		UIMap[Key].UI->AddToViewport(UIMap[Key].Order);
		UIMap[Key].bIsActivate = true;
	}
	else
	{
		if (!UIMap[Key].bIsActivate)
		{
			UE_LOG(LogUIManager, Log, TEXT("SetWidget: %s is already Hide."), *Key);
			return false;
		}
		UIMap[Key].UI->RemoveFromParent();
		UIMap[Key].bIsActivate = false;
	}
	return true;
}
