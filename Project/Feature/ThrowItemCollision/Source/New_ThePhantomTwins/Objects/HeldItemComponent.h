// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Objects/ItemData.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "HeldItemComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NEW_THEPHANTOMTWINS_API UHeldItemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHeldItemComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category = "HeldItem")
    void SpawnAndAttachHeldItem(EItemType ItemType);

    UFUNCTION(BlueprintCallable, Category = "HeldItem")
    void DestroyHeldItem();


private:
    // DataTable에서 StaticMesh 가져오는 함수
    UStaticMesh* GetItemStaticMesh(EItemType ItemType) const;

    // 로컬 즉시 표시용 메쉬 생성
    void CreateLocalHeldItemMesh(EItemType ItemType);

    // 로컬 즉시 표시용 메쉬 제거
    void DestroyLocalHeldItemMesh();

    // 서버 RPC: replicated actor 파괴 요청
    UFUNCTION(Server, Reliable)
    void C2S_DestroyReplicatedHeldItem();
    void C2S_DestroyReplicatedHeldItem_Implementation();

    // Multicast RPC: 모든 클라이언트에서 해당 PlayerState의 Pawn 소켓에 붙어있는 (로컬) StaticMesh들을 제거
    UFUNCTION(NetMulticast, Reliable)
    void S2A_RemoveLocalAttachedMeshes(APlayerState* OwnerPlayerState, FName SocketName);
    void S2A_RemoveLocalAttachedMeshes_Implementation(APlayerState* OwnerPlayerState, FName SocketName);


public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TObjectPtr<UDataTable> ItemAbilityTable;


protected:
    // 로컬 즉시 표시용 컴포넌트 (클라이언트 전용)
    UPROPERTY()
    TObjectPtr<UStaticMeshComponent> LocalHeldItemComponent;

    // 서버에서 스폰한 복제된 액터 캐시
    UPROPERTY(Replicated)
    TObjectPtr<AActor> ReplicatedHeldActor;

};
