#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataTable.h"
#include "Objects/ItemData.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerState.h"
#include "GA_HoldItem.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_HoldItem : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UGA_HoldItem();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TObjectPtr<UDataTable> ItemAbilityTable;

private:
    // 로컬에서 즉시 보이도록 생성하는 컴포넌트 (클라이언트 전용)
    UPROPERTY()
    TObjectPtr<UStaticMeshComponent> HeldItemComponent;

    // 서버에서 스폰한 replicated actor 캐시
    UPROPERTY()
    AActor* ReplicatedHeldActor = nullptr;

    // DataTable에서 StaticMesh 얻어오는 함수
    UStaticMesh* SetItemStaticMesh(EItemType ItemType);

    // 서버 RPC: replicated actor 를 스폰/초기화 (서버에서 실행)
    UFUNCTION(Server, Reliable)
    void C2S_SpawnAndAttachHeldItem(EItemType ItemType);
    void C2S_SpawnAndAttachHeldItem_Implementation(EItemType ItemType);

    // Multicast RPC: 스폰된 액터를 모든 클라이언트에서 해당 플레이어의 손에 붙이도록 지시
    UFUNCTION(NetMulticast, Reliable)
    void S2A_AttachReplicatedActor(AActor* SpawnedActor, APlayerState* OwnerPlayerState, FName SocketName, EItemType ItemType);
    void S2A_AttachReplicatedActor_Implementation(AActor* SpawnedActor, APlayerState* OwnerPlayerState, FName SocketName, EItemType ItemType);

    // 서버 RPC: replicated actor 파괴 요청
    UFUNCTION(Server, Reliable)
    void C2S_DestroyReplicatedHeldItem();
    void C2S_DestroyReplicatedHeldItem_Implementation();
};
