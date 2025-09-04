

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

    // 소유자 검증
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }
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

    // 로컬 즉시 표시용 메쉬 생성
    CreateLocalHeldItemMesh(ItemType);

    // 서버에 복제된 액터 스폰 요청
    if (ItemType != EItemType::None)
    {
        C2S_SpawnAndAttachHeldItem(ItemType);
    }
}

void UHeldItemComponent::DestroyHeldItem()
{
    // 로컬 메쉬 제거
    DestroyLocalHeldItemMesh();

    // 서버에 복제된 액터 파괴 요청
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
    if (!Character)
    {
        TPT_LOG(GALog, Warning, TEXT("CreateLocalHeldItemMesh: Owner가 PlayerCharacter가 아닙니다."));
        return;
    }

    USkeletalMeshComponent* MeshComp = Character->GetMesh();
    if (!MeshComp)
    {
        TPT_LOG(GALog, Warning, TEXT("CreateLocalHeldItemMesh: Character에 Mesh가 없습니다."));
        return;
    }

    // 기존 로컬 메쉬 제거
    DestroyLocalHeldItemMesh();

    UStaticMesh* ItemMesh = GetItemStaticMesh(ItemType);
    if (!ItemMesh)
    {
        TPT_LOG(GALog, Warning, TEXT("CreateLocalHeldItemMesh: ItemMesh를 찾을 수 없습니다."));
        return;
    }

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

        TPT_LOG(GALog, Log, TEXT("CreateLocalHeldItemMesh: 로컬 아이템 메쉬 생성 완료"));
    }
}

void UHeldItemComponent::DestroyLocalHeldItemMesh()
{
    // 로컬 즉시 표시용 컴포넌트 제거
    if (LocalHeldItemComponent)
    {
        LocalHeldItemComponent->DestroyComponent();
        LocalHeldItemComponent = nullptr;
        TPT_LOG(GALog, Log, TEXT("DestroyLocalHeldItemMesh: 로컬 아이템 메쉬 제거 완료"));
    }

    // 특정 소켓에 부착된 StaticMesh 컴포넌트들 찾아서 제거
    APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    USkeletalMeshComponent* MeshComp = Character->GetMesh();
    if (!MeshComp)
    {
        return;
    }

    const FName HandSocketName = TEXT("RightHandSocket");

    // 소켓에 부착된 모든 자식 컴포넌트들을 확인
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
            // StaticMeshComponent인지 확인
            UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(ChildComponent);
            if (StaticMeshComp)
            {
                ComponentsToRemove.Add(StaticMeshComp);
                TPT_LOG(GALog, Log, TEXT("DestroyLocalHeldItemMesh: %s 소켓에 부착된 StaticMeshComponent 발견: %s"),
                    *HandSocketName.ToString(),
                    *StaticMeshComp->GetName());
            }
        }
    }

    // 찾은 컴포넌트들 제거
    for (USceneComponent* ComponentToRemove : ComponentsToRemove)
    {
        if (ComponentToRemove && IsValid(ComponentToRemove))
        {
            // 컴포넌트가 액터의 일부인지 확인하고 적절히 처리
            AActor* ComponentOwner = ComponentToRemove->GetOwner();
            if (ComponentOwner && ComponentOwner != Character)
            {
                // 다른 액터의 컴포넌트인 경우, 해당 액터를 파괴
                ComponentOwner->Destroy();
            }
            else
            {
                // 캐릭터의 자식 컴포넌트인 경우, 컴포넌트만 제거
                ComponentToRemove->DestroyComponent();
            }
        }
    }

    //TPT_LOG(GALog, Log, TEXT("DestroyLocalHeldItemMesh: %s 소켓 정리 완료 - 제거된 컴포넌트 수: %d"),
    //    *HandSocketName.ToString(),
    //    ComponentsToRemove.Num());
}

void UHeldItemComponent::C2S_SpawnAndAttachHeldItem_Implementation(EItemType ItemType)
{
    APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());
    if (!Character || !Character->HasAuthority())
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: 권한 없거나 Character가 null입니다."));
        return;
    }

    UStaticMesh* MeshToUse = GetItemStaticMesh(ItemType);
    if (!MeshToUse)
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: MeshToUse가 null입니다."));
        return;
    }

    UWorld* World = Character->GetWorld();
    USkeletalMeshComponent* CharMesh = Character->GetMesh();
    if (!World || !CharMesh)
    {
        return;
    }

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
    if (!SpawnedActor)
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: 액터 스폰 실패"));
        return;
    }

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

    // 리슨 서버에서는 로컬 즉시 비주얼 제거 (중복 방지)
    if (LocalHeldItemComponent)
    {
        LocalHeldItemComponent->DestroyComponent();
        LocalHeldItemComponent = nullptr;
    }

    TPT_LOG(GALog, Log, TEXT("Server_SpawnAndAttachHeldItem: 복제 액터 스폰 및 부착 완료"));
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
    if (!PlayerChar)
    {
        return;
    }

    USkeletalMeshComponent* CharMesh = PlayerChar->GetMesh();
    if (!CharMesh)
    {
        return;
    }

    // 스폰된 액터를 캐릭터 소켓에 부착
    if (CharMesh->DoesSocketExist(SocketName))
    {
        RootMeshComp->AttachToComponent(CharMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
        RootMeshComp->SetRelativeLocation(FVector::ZeroVector);
        RootMeshComp->SetRelativeRotation(FRotator::ZeroRotator);
    }
    else
    {
        RootMeshComp->AttachToComponent(CharMesh, FAttachmentTransformRules::KeepWorldTransform);
    }

    SpawnedActor->SetReplicateMovement(true);
}

void UHeldItemComponent::C2S_DestroyReplicatedHeldItem_Implementation()
{
    if (ReplicatedHeldActor)
    {
        ReplicatedHeldActor->Destroy();
        ReplicatedHeldActor = nullptr;

        TPT_LOG(GALog, Log, TEXT("Server_DestroyReplicatedHeldItem: 복제된 아이템 액터 파괴 완료"));
    }
}
