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
    // 투척 아이템이 아니면 기존 아이템 제거
    if (ItemType != EItemType::EMP && ItemType != EItemType::NoiseBomb && ItemType != EItemType::Key)
    {
        DestroyHeldItem();
        return;
    }

    // 로컬 즉시 표시용 메쉬 생성 (로컬에서 즉시 보이게)
    CreateLocalHeldItemMesh(ItemType);

    // 서버에 복제된 액터 스폰 요청
    if (ItemType != EItemType::None)
    {
        C2S_SpawnAndAttachHeldItem(ItemType);
    }
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
    if (!Character)
    {
        return;
    }

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

void UHeldItemComponent::C2S_SpawnAndAttachHeldItem_Implementation(EItemType ItemType)
{
    APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());
    if (!Character || !Character->HasAuthority()) return;

    UStaticMesh* MeshToUse = GetItemStaticMesh(ItemType);
    if (!MeshToUse) return;

    UWorld* World = Character->GetWorld();
    USkeletalMeshComponent* CharMesh = Character->GetMesh();
	if (!World || !CharMesh) return;


    const FName HandSocketName = TEXT("RightHandSocket");
    FVector SpawnLocation = Character->GetActorLocation();
    FRotator SpawnRotation = Character->GetActorRotation();

    // 소켓 위치 가져오기
    if (CharMesh->DoesSocketExist(HandSocketName))
    {
        const FTransform SocketWorldTransform = CharMesh->GetSocketTransform(HandSocketName, RTS_World);
        SpawnLocation = SocketWorldTransform.GetLocation();
        SpawnRotation = SocketWorldTransform.Rotator();
    }

    // 액터 스폰
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Character;
    SpawnParams.Instigator = Character->GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* SpawnedActor = World->SpawnActor<AActor>(AActor::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
    if (!SpawnedActor) return;


    // 복제 설정
    SpawnedActor->SetReplicates(true);
    SpawnedActor->SetReplicateMovement(true);

    // StaticMeshComponent 생성 및 설정
    UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(SpawnedActor);
    if (!MeshComp)
    {
        SpawnedActor->Destroy();
        return;
    }

    MeshComp->SetStaticMesh(MeshToUse);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    MeshComp->SetSimulatePhysics(false);
    MeshComp->SetIsReplicated(true);

    SpawnedActor->SetRootComponent(MeshComp);
    MeshComp->RegisterComponent();

    // 캐릭터 손에 부착
    if (CharMesh->DoesSocketExist(HandSocketName))
    {
        MeshComp->AttachToComponent(CharMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandSocketName);
    }
    else
    {
        MeshComp->AttachToComponent(CharMesh, FAttachmentTransformRules::KeepWorldTransform);
    }

    MeshComp->SetRelativeLocation(FVector::ZeroVector);
    MeshComp->SetRelativeRotation(FRotator::ZeroRotator);
    MeshComp->SetRelativeScale3D(FVector(1.0f));

    // 캐시 저장
    ReplicatedHeldActor = SpawnedActor;

    // 소유자 PlayerState 찾기
    APlayerState* OwnerPS = Character->GetPlayerState<APlayerState>();

    // 멀티캐스트로 모든 클라이언트에 부착 지시
    S2A_AttachReplicatedActor(SpawnedActor, OwnerPS, HandSocketName, ItemType);

    // 서버 측 로컬 즉시 비주얼 제거 (서버 인스턴스에 남아있을 수 있으므로)
    if (LocalHeldItemComponent)
    {
        LocalHeldItemComponent->DestroyComponent();
        LocalHeldItemComponent = nullptr;
    }

    //TPT_LOG(GALog, Log, TEXT("Server_SpawnAndAttachHeldItem: 복제 액터 스폰 및 부착 완료"));
}

void UHeldItemComponent::S2A_AttachReplicatedActor_Implementation(AActor* SpawnedActor, APlayerState* OwnerPlayerState, FName SocketName, EItemType ItemType)
{
    if (!SpawnedActor)
    {
        return;
    }

    // 루트 StaticMeshComponent 찾기
    UStaticMeshComponent* RootMeshComp = Cast<UStaticMeshComponent>(SpawnedActor->GetRootComponent());
    if (!RootMeshComp)
    {
        TArray<UStaticMeshComponent*> Comps;
        SpawnedActor->GetComponents<UStaticMeshComponent>(Comps);
        if (Comps.Num() > 0)
        {
            RootMeshComp = Comps[0];
        }
    }

    // 클라이언트에서 메쉬가 없는 경우 DataTable에서 설정
    if (RootMeshComp && !RootMeshComp->GetStaticMesh())
    {
        UStaticMesh* MeshToUse = GetItemStaticMesh(ItemType);
        if (MeshToUse)
        {
            RootMeshComp->SetStaticMesh(MeshToUse);
            RootMeshComp->SetIsReplicated(true);

            if (!RootMeshComp->IsRegistered())
            {
                RootMeshComp->RegisterComponent();
            }
        }
    }

    // 해당 PlayerState의 Pawn 찾기
    UWorld* World = SpawnedActor->GetWorld();
    if (!World)
    {
        return;
    }

    APawn* TargetPawn = nullptr;
    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC) continue;

        APawn* P = PC->GetPawn();
        if (!P) continue;

        if (P->GetPlayerState() == OwnerPlayerState)
        {
            TargetPawn = P;
            break;
        }
    }

    if (!TargetPawn)
    {
        TPT_LOG(GALog, Warning, TEXT("Multicast_AttachReplicatedActor: OwnerPlayerState에 해당하는 Pawn을 찾을 수 없습니다"));
        return;
    }

    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(TargetPawn);
    if (!PlayerChar) return;


    USkeletalMeshComponent* CharMesh = PlayerChar->GetMesh();
    if (!CharMesh) return;


    // 스폰된 액터를 캐릭터 소켓에 부착
    if (CharMesh->DoesSocketExist(SocketName) && RootMeshComp)
    {
        RootMeshComp->AttachToComponent(CharMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
        RootMeshComp->SetRelativeLocation(FVector::ZeroVector);
        RootMeshComp->SetRelativeRotation(FRotator::ZeroRotator);
    }
    else if (RootMeshComp)
    {
        RootMeshComp->AttachToComponent(CharMesh, FAttachmentTransformRules::KeepWorldTransform);
    }

    SpawnedActor->SetReplicateMovement(true);

    // --- 중복 로컬 메쉬 제거: 같은 소켓에 붙어있는 다른 StaticMeshComponent(로컬 즉시 표시용 등)를 제거 ---
    {
        TArray<USceneComponent*> ToRemove;
        for (USceneComponent* ChildComp : CharMesh->GetAttachChildren())
        {
            if (!ChildComp) continue;

            UStaticMeshComponent* StaticComp = Cast<UStaticMeshComponent>(ChildComp);
            if (!StaticComp) continue;

            // 소켓이 일치하면 고려
            if (StaticComp->GetAttachSocketName() == SocketName)
            {
                // 이 컴포넌트가 이미 스폰된 복제 액터의 루트 컴포넌트인지 확인
                if (StaticComp == RootMeshComp) continue; // 복제 액터의 루트는 건너뜀


                // 복제 액터의 컴포넌트가 아니면 제거 대상으로 추가
                ToRemove.Add(StaticComp);
            }
        }

        for (USceneComponent* CompRem : ToRemove)
        {
            if (!CompRem) continue;

            // 만약 그 컴포넌트가 Pawn의 직접 소유(로컬 즉시 메쉬)라면 컴포넌트만 제거
            AActor* CompOwner = CompRem->GetOwner();
            if (CompOwner && CompOwner != PlayerChar)
            {
                CompOwner->Destroy();
            }
            else
            {
                CompRem->DestroyComponent();
            }

            // 만약 이 UHeldItemComponent 인스턴스의 LocalHeldItemComponent 라면 null 처리
            if (LocalHeldItemComponent && CompRem == LocalHeldItemComponent)
            {
                LocalHeldItemComponent = nullptr;
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
