#include "GA_HoldItem.h"
#include "Engine/StaticMesh.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Objects/InventoryComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

UGA_HoldItem::UGA_HoldItem()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_HoldItem);

    HeldItemComponent = nullptr;
    ReplicatedHeldActor = nullptr;}

void UGA_HoldItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
    if (Pawn && !Pawn->IsLocallyControlled())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    float SlotNumber = TriggerEventData ? TriggerEventData->EventMagnitude : -1.f;
    TPT_LOG(GALog, Warning, TEXT("SlotNumber %f"), SlotNumber);

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
    if (!Character)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }



    // 오른손에 붙임
    USkeletalMeshComponent* MeshComp = Character->GetMesh();
    const FName HandSocketName = TEXT("RightHandSocket");

    APlayerController* PlayerController = ActorInfo->PlayerController.Get();
    EItemType choiceItemType = EItemType::None;

    if (PlayerController)
    {
        APlayerState* PS = PlayerController->PlayerState;
        if (PS)
        {
            APS_Player* PlayerPS = Cast<APS_Player>(PS);
            if (PlayerPS)
            {
                UInventoryComponent* InventoryComponent = PlayerPS->InventoryComp;
                if (InventoryComponent)
                {
                    choiceItemType = InventoryComponent->ChoiceItem(static_cast<int32>(SlotNumber - 1));
                }
            }
        }
    }

    // 소음폭탄 이거나 EMP면 Mesh 손에 들기
    if (choiceItemType == EItemType::EMP || choiceItemType == EItemType::NoiseBomb)
    {
        C2S_DestroyReplicatedHeldItem();    // 투척 아이템 아니면 손에 있는 Mesh 지우기

        // 로컬 즉시 표시용 Mesh 가져오기
        UStaticMesh* choiceItemStaticMesh = SetItemStaticMesh(choiceItemType);

        // 로컬 즉시성: 로컬에서만 보이는 StaticMeshComponent 생성
        if (choiceItemStaticMesh && MeshComp)
        {
            if (HeldItemComponent)
            {
                HeldItemComponent->DestroyComponent();
                HeldItemComponent = nullptr;
            }

            UStaticMeshComponent* LocalMeshComp = NewObject<UStaticMeshComponent>(Character);
            if (LocalMeshComp)
            {
                LocalMeshComp->SetStaticMesh(choiceItemStaticMesh);
                LocalMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                LocalMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
                LocalMeshComp->SetSimulatePhysics(false);

                LocalMeshComp->RegisterComponent();

                LocalMeshComp->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandSocketName);
                LocalMeshComp->SetRelativeLocation(FVector::ZeroVector);
                LocalMeshComp->SetRelativeRotation(FRotator::ZeroRotator);
                LocalMeshComp->SetRelativeScale3D(FVector(1.0f));

                HeldItemComponent = LocalMeshComp;
            }

            TPT_LOG(GALog, Log, TEXT("아이템 타입에 해당하는 메쉬를 찾아서 로컬 손 소켓에 부착 완료."));


            // 이 다음에 소음폭탄, EMP 투척 아이템이 날아가는 포물선 인디케이터 출력
        }
        else
        {
            TPT_LOG(GALog, Warning, TEXT("이 아이템 타입에 대한 DataTable 메쉬가 없거나 MeshComp가 null입니다."));
        }

        // 서버에 복제되는 액터 스폰 요청 (서버가 각 클라이언트에서 보이도록)
        if (choiceItemType != EItemType::None)
        {
            C2S_SpawnAndAttachHeldItem(choiceItemType);
        }
    }
    else
    {
        TPT_LOG(GALog, Warning, TEXT("투척 아이템 X => 손에 있는 Mesh 지우기"));

        C2S_DestroyReplicatedHeldItem();    // 투척 아이템 아니면 손에 있는 Mesh 지우기
    }



    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}


// DataTable에서 StaticMesh 얻어오는 함수
UStaticMesh* UGA_HoldItem::SetItemStaticMesh(EItemType ItemType)
{
    if (ItemType == EItemType::None) return nullptr;
    if (!ItemAbilityTable)
    {
        TPT_LOG(GALog, Warning, TEXT("SetItemStaticMesh: ItemAbilityTable이 %s에서 지정되지 않음"), *GetName());
        return nullptr;
    }

    const FString ContextString = TEXT("UGA_HoldItem::SetItemStaticMesh");
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
                TPT_LOG(GALog, Warning, TEXT("SetItemStaticMesh: %s 행은 있지만 ItemMesh가 null임"), *RowName.ToString());
                return nullptr;
            }
        }
    }

    TPT_LOG(GALog, Warning, TEXT("SetItemStaticMesh: DataTable %s에서 ItemType %d에 해당하는 행 없음"), *GetName(), static_cast<int32>(ItemType));
    return nullptr;
}

// 서버 RPC: 액터 스폰 후 복제되는 StaticMeshComponent 생성, 멀티캐스트로 부착 지시
void UGA_HoldItem::C2S_SpawnAndAttachHeldItem_Implementation(EItemType ItemType)
{
    // 이 코드는 서버에서 실행됨
    AActor* Avatar = GetAvatarActorFromActorInfo();
    APlayerCharacter* Character = Avatar ? Cast<APlayerCharacter>(Avatar) : nullptr;
    if (!Character)
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: 서버에서 Character를 찾을 수 없음."));
        return;
    }

    // 권한 있는 액터에서만 실행되도록 보장
    if (!Character->HasAuthority())
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: 권한 없는 액터에서 호출됨."));
        return;
    }

    UStaticMesh* MeshToUse = SetItemStaticMesh(ItemType);
    if (!MeshToUse)
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: MeshToUse가 null임."));
        return;
    }

    UWorld* World = Character->GetWorld();
    if (!World) return;

    USkeletalMeshComponent* CharMesh = Character->GetMesh();
    if (!CharMesh)
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: Character에 Mesh가 없음."));
        return;
    }

    // 소켓 이름 선택 (스켈레톤에서 사용하는 정확한 소켓명으로 교체 필요)
    FName HandSocketName = TEXT("RightHandSocket");

    if (!CharMesh->DoesSocketExist(HandSocketName))
    {
        HandSocketName = TEXT("");
        TPT_LOG(GALog, Warning, TEXT("잘못된 소켓 이름 입니다."));

    }

    FVector SpawnLocation = Character->GetActorLocation();
    FRotator SpawnRotation = Character->GetActorRotation();

    if (CharMesh->DoesSocketExist(HandSocketName))
    {
        const FTransform SocketWorldTransform = CharMesh->GetSocketTransform(HandSocketName, RTS_World);
        SpawnLocation = SocketWorldTransform.GetLocation();
        SpawnRotation = SocketWorldTransform.Rotator();
    }
    else
    {
        SpawnLocation = CharMesh->GetComponentLocation();
        SpawnRotation = CharMesh->GetComponentRotation();
    }

    // 스폰 파라미터: Owner = Character로 설정하여 서버가 명확한 소유권 가짐
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Character;
    SpawnParams.Instigator = Character->GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 서버에서 기본 AActor 스폰
    AActor* Spawned = World->SpawnActor<AActor>(AActor::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
    if (!Spawned)
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: 액터 스폰 실패."));
        return;
    }

    // 액터와 이동 복제 설정
    Spawned->SetReplicates(true);
    Spawned->SetReplicateMovement(true);

    // 스폰된 액터에 StaticMeshComponent 생성
    UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(Spawned);
    if (!MeshComp)
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: StaticMeshComponent 생성 실패."));
        Spawned->Destroy();
        return;
    }

    MeshComp->SetStaticMesh(MeshToUse);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    MeshComp->SetSimulatePhysics(false);

    // 컴포넌트 복제 활성화
    MeshComp->SetIsReplicated(true);

    // Mesh를 루트 컴포넌트로 설정 → 등록 → 서버에서 캐릭터 Mesh 소켓에 부착
    Spawned->SetRootComponent(MeshComp);
    MeshComp->RegisterComponent();

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

    // 나중에 파괴할 수 있도록 캐시
    ReplicatedHeldActor = Spawned;

    // 소유자 PlayerState 찾아서 각 클라이언트에 어느 플레이어 손에 부착할지 전달
    APlayerState* OwnerPS = Character->GetPlayerState<APlayerState>();

    // 멀티캐스트로 클라이언트에게 메쉬 설정 및 소켓 부착 지시
    S2A_AttachReplicatedActor(Spawned, OwnerPS, HandSocketName, ItemType);

    // 리슨 서버에서는 로컬 즉시 비주얼 제거 (중복 방지)
    if (HeldItemComponent)
    {
        HeldItemComponent->DestroyComponent();
        HeldItemComponent = nullptr;
    }

    TPT_LOG(GALog, Log, TEXT("Server_SpawnAndAttachHeldItem: 복제 액터 스폰 및 멀티캐스트 부착 요청 완료."));
}

void UGA_HoldItem::S2A_AttachReplicatedActor_Implementation(AActor* SpawnedActor, APlayerState* OwnerPlayerState, FName SocketName, EItemType ItemType)
{
    if (!SpawnedActor) return;

    // 각 클라이언트(및 서버)에서, 스폰된 액터의 루트 StaticMeshComponent 확인 후 메쉬 설정
    UStaticMeshComponent* RootMeshComp = Cast<UStaticMeshComponent>(SpawnedActor->GetRootComponent());
    if (!RootMeshComp)
    {
        // StaticMeshComponent가 없으면 탐색
        TArray<UStaticMeshComponent*> Comps;
        SpawnedActor->GetComponents<UStaticMeshComponent>(Comps);
        if (Comps.Num() > 0)
        {
            RootMeshComp = Comps[0];
        }
    }

    // 컴포넌트가 존재하지만 메쉬가 없는 경우 (클라이언트 측), DataTable에서 메쉬 가져와 설정
    if (RootMeshComp && !RootMeshComp->GetStaticMesh())
    {
        UStaticMesh* MeshToUse = SetItemStaticMesh(ItemType); // 로컬 Ability 인스턴스에서 DataTable 참조 가능
        if (MeshToUse)
        {
            RootMeshComp->SetStaticMesh(MeshToUse);
            RootMeshComp->SetIsReplicated(true);
            // 등록되지 않았다면 등록
            if (!RootMeshComp->IsRegistered())
            {
                RootMeshComp->RegisterComponent();
            }
        }
    }

    // OwnerPlayerState에 해당하는 Pawn 찾기 (플레이어 컨트롤러 순회)
    UWorld* World = SpawnedActor->GetWorld();
    if (!World) return;

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
        // Pawn을 찾을 수 없을 경우 (아직 소유되지 않은 경우), 그대로 둠
        TPT_LOG(GALog, Warning, TEXT("Multicast_AttachReplicatedActor: 이 클라이언트에서 OwnerPlayerState에 해당하는 Pawn을 찾지 못함."));
        return;
    }

    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(TargetPawn);
    if (!PlayerChar)
    {
        TPT_LOG(GALog, Warning, TEXT("Multicast_AttachReplicatedActor: TargetPawn이 APlayerCharacter가 아님."));
        return;
    }

    USkeletalMeshComponent* CharMesh = PlayerChar->GetMesh();
    if (!CharMesh)
    {
        TPT_LOG(GALog, Warning, TEXT("Multicast_AttachReplicatedActor: PlayerChar에 Mesh 없음."));
        return;
    }

    // 스폰된 액터 루트 컴포넌트를 해당 캐릭터의 소켓에 부착
    if (CharMesh->DoesSocketExist(SocketName))
    {
        RootMeshComp->AttachToComponent(CharMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
        RootMeshComp->SetRelativeLocation(FVector::ZeroVector);
        RootMeshComp->SetRelativeRotation(FRotator::ZeroRotator);
    }
    else
    {
        // 소켓 없으면 Mesh에 부착하고 월드 좌표 유지
        RootMeshComp->AttachToComponent(CharMesh, FAttachmentTransformRules::KeepWorldTransform);
    }

    // 액터 이동도 복제되도록 설정
    SpawnedActor->SetReplicateMovement(true);
}

// 서버 RPC: 복제된 액터 파괴
void UGA_HoldItem::C2S_DestroyReplicatedHeldItem_Implementation()
{
    if (ReplicatedHeldActor)
    {
        // 서버에서 실행됨; Destroy가 클라이언트에도 복제됨
        ReplicatedHeldActor->Destroy();
        ReplicatedHeldActor = nullptr;
    }
}
