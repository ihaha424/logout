#include "HeldItemComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Objects/InventoryComponent.h"
#include "Log/TPTLog.h"


UHeldItemComponent::UHeldItemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // 네트워킹 설정
    SetIsReplicatedByDefault(true);

    LocalHeldItemComponent = nullptr;
    ReplicatedHeldActor = nullptr;
}

void UHeldItemComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UHeldItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UHeldItemComponent, ReplicatedHeldActor);
}

void UHeldItemComponent::SpawnAndAttachHeldItem(EItemType ItemType)
{
    // 투척 아이템이 아니면 기존 아이템 제거+
    if (ItemType != EItemType::EMP && ItemType != EItemType::NoiseBomb)
    {
        DestroyHeldItem();
        return;
    }

    // 로컬 즉시 표시용 메쉬 생성 (로컬에서 즉시 보이게)
    CreateLocalHeldItemMesh(ItemType);
}

void UHeldItemComponent::DestroyHeldItem()
{
    // 로컬 메쉬 제거 (클라이언트 자신의 즉시 표시용)
    DestroyLocalHeldItemMesh();

    // 서버에 복제된 액터 파괴 요청 (서버가 파괴하고, 멀티캐스트로 클라이언트들도 정리)
    C2S_DestroyReplicatedHeldItem();
}

UStaticMesh* UHeldItemComponent::GetItemStaticMesh(EItemType ItemType) const
{
    if (ItemType == EItemType::None || !ItemAbilityTable)
    {
        return nullptr;
    }

    const FString ContextString = TEXT("UHeldItemComponent::GetItemStaticMesh");
    TArray<FName> RowNames = ItemAbilityTable->GetRowNames();

    for (const FName& RowName : RowNames)
    {
        const FItemDataTable* Row = ItemAbilityTable->FindRow<FItemDataTable>(RowName, ContextString);
        if (!Row) continue;

        if (Row->ItemType == ItemType)
        {
            if (Row->ItemMesh)
            {
                return Row->ItemMesh;
            }
            else
            {
                TPT_LOG(GALog, Warning, TEXT("GetItemStaticMesh: %s 행의 ItemMesh가 null입니다"), *RowName.ToString());
                return nullptr;
            }
        }
    }

    TPT_LOG(GALog, Warning, TEXT("GetItemStaticMesh: ItemType %d에 해당하는 데이터를 찾을 수 없습니다"), static_cast<int32>(ItemType));
    return nullptr;
}

void UHeldItemComponent::CreateLocalHeldItemMesh(EItemType ItemType)
{
    APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());
    if (!Character) return;


    USkeletalMeshComponent* MeshComp = Character->GetMesh();
    if (!MeshComp) return;


    // 기존 로컬 메쉬 제거
    DestroyLocalHeldItemMesh();

    UStaticMesh* ItemMesh = GetItemStaticMesh(ItemType);
    if (!ItemMesh) return;


    const FName HandSocketName = TEXT("RightHandSocket");

    // 로컬 즉시 표시용 StaticMeshComponent 생성
    UStaticMeshComponent* LocalMeshComp = NewObject<UStaticMeshComponent>(Character);
    if (LocalMeshComp)
    {
        LocalMeshComp->SetStaticMesh(ItemMesh);
        LocalMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        LocalMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
        LocalMeshComp->SetSimulatePhysics(false);
        LocalMeshComp->RegisterComponent();
        LocalMeshComp->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandSocketName);
        LocalMeshComp->SetRelativeLocation(FVector::ZeroVector);
        LocalMeshComp->SetRelativeRotation(FRotator::ZeroRotator);
        LocalMeshComp->SetRelativeScale3D(FVector(1.0f));
        LocalMeshComp->SetIsReplicated(true);

        LocalHeldItemComponent = LocalMeshComp;

        //TPT_LOG(GALog, Log, TEXT("CreateLocalHeldItemMesh: 로컬 아이템 메쉬 생성 완료"));
    }
}

void UHeldItemComponent::DestroyLocalHeldItemMesh()
{
    // 로컬 즉시 표시용 컴포넌트 제거
    if (LocalHeldItemComponent)
    {
        if (LocalHeldItemComponent->IsValidLowLevel())
        {
            LocalHeldItemComponent->DestroyComponent();
        }
        LocalHeldItemComponent = nullptr;
        //TPT_LOG(GALog, Log, TEXT("DestroyLocalHeldItemMesh: 로컬 아이템 메쉬 제거 완료"));
    }

    // 특정 소켓에 부착된 StaticMesh 컴포넌트들 찾아서 제거 (추가 안전장치)
    APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());
    if (!Character) return;

    USkeletalMeshComponent* MeshComp = Character->GetMesh();
    if (!MeshComp)  return;


    const FName HandSocketName = TEXT("RightHandSocket");

    TArray<USceneComponent*> ComponentsToRemove;

    for (USceneComponent* ChildComponent : MeshComp->GetAttachChildren())
    {
        if (!ChildComponent)
        {
            continue;
        }

        // 특정 소켓에 부착된 컴포넌트인지 확인
        if (ChildComponent->GetAttachSocketName() == HandSocketName)
        {
            UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(ChildComponent);
            if (StaticMeshComp)
            {
                ComponentsToRemove.Add(StaticMeshComp);
            }
        }
    }

    for (USceneComponent* ComponentToRemove : ComponentsToRemove)
    {
        if (ComponentToRemove && IsValid(ComponentToRemove))
        {
            AActor* ComponentOwner = ComponentToRemove->GetOwner();
            if (ComponentOwner && ComponentOwner != Character)
            {
                ComponentOwner->Destroy();
            }
            else
            {
                ComponentToRemove->DestroyComponent();
            }
        }
    }
}

void UHeldItemComponent::C2S_DestroyReplicatedHeldItem_Implementation()
{
    // 서버에서 복제 액터 파괴
    if (ReplicatedHeldActor)
    {
        // 소유자 PlayerState를 복사해 저장 (멀티캐스트에 전달하기 위해)
        AActor* ActorOwner = ReplicatedHeldActor->GetOwner();
        APlayerState* OwnerPS = nullptr;
        if (ActorOwner)
        {
            APawn* PawnOwner = Cast<APawn>(ActorOwner);
            if (PawnOwner)
            {
                OwnerPS = PawnOwner->GetPlayerState<APlayerState>();
            }
        }

        ReplicatedHeldActor->Destroy();
        ReplicatedHeldActor = nullptr;

        //TPT_LOG(GALog, Log, TEXT("Server_DestroyReplicatedHeldItem: 복제된 아이템 액터 파괴 완료"));

        // 모든 클라이언트에 해당 플레이어의 소켓에 붙은 로컬 메쉬 제거 지시
        const FName HandSocketName = TEXT("RightHandSocket");
        if (OwnerPS)
        {
            S2A_RemoveLocalAttachedMeshes(OwnerPS, HandSocketName);
        }
    }
    else
    {
        // 복제 액터가 없더라도, 로컬 즉시 메쉬가 남아있을 수 있으니 모든 클라이언트 정리 지시
        APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());
        if (Character)
        {
            APlayerState* OwnerPS = Character->GetPlayerState<APlayerState>();
            const FName HandSocketName = TEXT("RightHandSocket");
            if (OwnerPS)
            {
                S2A_RemoveLocalAttachedMeshes(OwnerPS, HandSocketName);
            }
        }
    }
}

void UHeldItemComponent::S2A_RemoveLocalAttachedMeshes_Implementation(APlayerState* OwnerPlayerState, FName SocketName)
{
    if (!OwnerPlayerState)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // 해당 PlayerState에 대응되는 Pawn을 찾아 소켓에 붙은 StaticMesh들을 제거
    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC) continue;

        APawn* P = PC->GetPawn();
        if (!P) continue;

        if (P->GetPlayerState() == OwnerPlayerState)
        {
            APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(P);
            if (!PlayerChar) return;

            USkeletalMeshComponent* CharMesh = PlayerChar->GetMesh();
            if (!CharMesh) return;

            TArray<USceneComponent*> ComponentsToRemove;

            for (USceneComponent* ChildComponent : CharMesh->GetAttachChildren())
            {
                if (!ChildComponent) continue;

                UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(ChildComponent);
                if (!StaticMeshComp) continue;

                if (StaticMeshComp->GetAttachSocketName() == SocketName)
                {
                    ComponentsToRemove.Add(StaticMeshComp);
                }
            }

            for (USceneComponent* ComponentToRemove : ComponentsToRemove)
            {
                if (!ComponentToRemove) continue;

                AActor* ComponentOwner = ComponentToRemove->GetOwner();

                // 만약 이 HeldItemComponent 인스턴스의 로컬 컴포넌트라면 DestroyLocalHeldItemMesh() 호출로 정리
                if (GetOwner() == PlayerChar && LocalHeldItemComponent && ComponentToRemove == LocalHeldItemComponent)
                {
                    DestroyLocalHeldItemMesh();
                }
                else
                {
                    if (ComponentOwner && ComponentOwner != PlayerChar)
                    {
                        ComponentOwner->Destroy();
                    }
                    else
                    {
                        ComponentToRemove->DestroyComponent();
                    }
                }
            }

            break;
        }
    }
}
