

#include "StoryFlowExampleActor.h"
#include "StoryFlowManager.h"
#include "Engine/Engine.h"
#include "UObject/NoExportTypes.h"

void AStoryFlowExampleActor::BeginPlay()
{
    Super::BeginPlay();

    UStoryFlowManager* Manager = GetGameInstance()->GetSubsystem<UStoryFlowManager>();

    // 1. RegisterData
    UObject* FlowObj = NewObject<UObject>(this, FName("Flow_A1"));
    Manager->RegisterData("CurrentFlow", FlowObj);

    // 2. SetData
    UObject* NextFlow = NewObject<UObject>(this, FName("Flow_A2"));
    Manager->SetData("CurrentFlow", NextFlow);

    // 3. Subscribe (C++ 콜백 등록)
    FlowSubscriptionId = Manager->Subscribe("CurrentFlow",
        [this](FName Key, const UObject* Value)
        {
            OnFlowChanged(Key, Value);
        });

    // 4. GetData
    UObject* Retrieved = Manager->GetData("CurrentFlow");
    if (Retrieved)
    {
        UE_LOG(LogTemp, Log, TEXT("GetData returned: %s"), *GetNameSafe(Retrieved));
    }

    // 5. GetDataAs<UObject>
    UObject* Typed = Manager->GetDataAs<UObject>("CurrentFlow");
    if (Typed)
    {
        UE_LOG(LogTemp, Log, TEXT("GetDataAs<UObject> returned: %s"), *Typed->GetName());
    }

    // 6. SaveToDisk
    Manager->SaveToDisk();

    // 7. Unsubscribe
    Manager->Unsubscribe("CurrentFlow", FlowSubscriptionId);

    // 8. LoadFromDisk (for demonstration)
    Manager->LoadFromDisk();

    // 9. Get again after load
    UObject* AfterLoad = Manager->GetData("CurrentFlow");
    if (AfterLoad)
    {
        UE_LOG(LogTemp, Log, TEXT("After Load: %s"), *AfterLoad->GetName());
    }
}

void AStoryFlowExampleActor::OnFlowChanged(FName Key, const UObject* Value)
{
    UE_LOG(LogTemp, Warning, TEXT("Flow Changed: %s = %s"), *Key.ToString(), *GetNameSafe(Value));
}