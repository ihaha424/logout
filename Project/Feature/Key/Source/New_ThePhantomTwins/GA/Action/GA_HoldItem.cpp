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
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

    // Warning about AbilityTags API is informational; leaving as-is for now.
    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_HoldItem);

    HeldItemComponent = nullptr;
    ReplicatedHeldActor = nullptr;
}

void UGA_HoldItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    // Avatar / local check
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

    USkeletalMeshComponent* MeshComp = Character->GetMesh();
    const FName HandSocketName = TEXT("RightHandSocket"); // 프로젝트 실제 소켓명으로 바꿔주세요

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
                    // ChoiceItem expects 0-based index
                    choiceItemType = InventoryComponent->ChoiceItem(static_cast<int32>(SlotNumber - 1));
                }
            }
        }
    }

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

        TPT_LOG(GALog, Log, TEXT("Found mesh for item type, attached to hand socket locally."));
    }
    else
    {
        TPT_LOG(GALog, Warning, TEXT("No mesh found in DataTable for this item type or MeshComp is null."));
    }

    // 서버에 replicated actor 스폰 요청 (서버가 각 클라이언트에 보이도록)
    if (choiceItemType != EItemType::None)
    {
        C2S_SpawnAndAttachHeldItem(choiceItemType);
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

UStaticMesh* UGA_HoldItem::SetItemStaticMesh(EItemType ItemType)
{
    if (ItemType == EItemType::None) return nullptr;
    if (!ItemAbilityTable)
    {
        TPT_LOG(GALog, Warning, TEXT("SetItemStaticMesh: ItemAbilityTable not assigned on %s"), *GetName());
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
                TPT_LOG(GALog, Warning, TEXT("SetItemStaticMesh: Found row %s but ItemMesh is null"), *RowName.ToString());
                return nullptr;
            }
        }
    }

    TPT_LOG(GALog, Warning, TEXT("SetItemStaticMesh: No matching row for ItemType %d in DataTable %s"), static_cast<int32>(ItemType), *GetName());
    return nullptr;
}

// Server RPC: spawn actor and create a replicated StaticMeshComponent on it, then multicast attach instruction
void UGA_HoldItem::C2S_SpawnAndAttachHeldItem_Implementation(EItemType ItemType)
{
    // This runs on server
    AActor* Avatar = GetAvatarActorFromActorInfo();
    APlayerCharacter* Character = Avatar ? Cast<APlayerCharacter>(Avatar) : nullptr;
    if (!Character)
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: No Character available on server."));
        return;
    }

    // Ensure this server-side code runs on authoritative actor
    if (!Character->HasAuthority())
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: Called on non-authority for this character."));
        return;
    }

    UStaticMesh* MeshToUse = SetItemStaticMesh(ItemType);
    if (!MeshToUse)
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: MeshToUse is null."));
        return;
    }

    UWorld* World = Character->GetWorld();
    if (!World) return;

    USkeletalMeshComponent* CharMesh = Character->GetMesh();
    if (!CharMesh)
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: Character has no mesh."));
        return;
    }

    // select socket name (replace with exact socket used by your skeleton)
    FName HandSocketName = TEXT("RightHandSocket");
    if (!CharMesh->DoesSocketExist(HandSocketName))
    {
        HandSocketName = TEXT("Hand_Socket");
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

    // Spawn parameters with Owner = Character so server has obvious owner
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Character;
    SpawnParams.Instigator = Character->GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Spawn a plain AActor on server
    AActor* Spawned = World->SpawnActor<AActor>(AActor::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
    if (!Spawned)
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: Failed to spawn actor."));
        return;
    }

    // replicate actor and movement
    Spawned->SetReplicates(true);
    Spawned->SetReplicateMovement(true);

    // create static mesh component on spawned actor
    UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(Spawned);
    if (!MeshComp)
    {
        TPT_LOG(GALog, Warning, TEXT("Server_SpawnAndAttachHeldItem: Failed to create StaticMeshComponent."));
        Spawned->Destroy();
        return;
    }

    MeshComp->SetStaticMesh(MeshToUse);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    MeshComp->SetSimulatePhysics(false);

    // enable replication for component
    MeshComp->SetIsReplicated(true);

    // Make mesh the root component of spawned actor, register it, then attach to character mesh socket on server
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

    // cache spawned for possible destruction later
    ReplicatedHeldActor = Spawned;

    // find owner playerstate to tell clients which player's hand to attach to
    APlayerState* OwnerPS = Character->GetPlayerState<APlayerState>();

    // multicast to clients to set mesh (if needed) and attach to the correct player's hand socket
    S2A_AttachReplicatedActor(Spawned, OwnerPS, HandSocketName, ItemType);

    // remove local immediate visual on listen-server to avoid duplicates
    if (HeldItemComponent)
    {
        HeldItemComponent->DestroyComponent();
        HeldItemComponent = nullptr;
    }

    TPT_LOG(GALog, Log, TEXT("Server_SpawnAndAttachHeldItem: Spawned replicated actor and requested multicast attach."));
}

void UGA_HoldItem::S2A_AttachReplicatedActor_Implementation(AActor* SpawnedActor, APlayerState* OwnerPlayerState, FName SocketName, EItemType ItemType)
{
    if (!SpawnedActor) return;

    // On each client (and server too), ensure the mesh is set on the spawned actor's root static mesh component.
    UStaticMeshComponent* RootMeshComp = Cast<UStaticMeshComponent>(SpawnedActor->GetRootComponent());
    if (!RootMeshComp)
    {
        // try to find any static mesh component
        TArray<UStaticMeshComponent*> Comps;
        SpawnedActor->GetComponents<UStaticMeshComponent>(Comps);
        if (Comps.Num() > 0)
        {
            RootMeshComp = Comps[0];
        }
    }

    // If the component exists but has no static mesh (client-side), set it using our DataTable lookup
    if (RootMeshComp && !RootMeshComp->GetStaticMesh())
    {
        UStaticMesh* MeshToUse = SetItemStaticMesh(ItemType); // local ability instance has access to the DataTable asset
        if (MeshToUse)
        {
            RootMeshComp->SetStaticMesh(MeshToUse);
            RootMeshComp->SetIsReplicated(true);
            // ensure it's registered
            if (!RootMeshComp->IsRegistered())
            {
                RootMeshComp->RegisterComponent();
            }
        }
    }

    // Find the local pawn that corresponds to OwnerPlayerState by iterating player controllers
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
        // Could not find pawn: maybe player not possessed locally yet. Defer or leave as-is.
        TPT_LOG(GALog, Warning, TEXT("Multicast_AttachReplicatedActor: Could not find pawn for OwnerPlayerState on this client."));
        return;
    }

    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(TargetPawn);
    if (!PlayerChar)
    {
        TPT_LOG(GALog, Warning, TEXT("Multicast_AttachReplicatedActor: TargetPawn is not APlayerCharacter."));
        return;
    }

    USkeletalMeshComponent* CharMesh = PlayerChar->GetMesh();
    if (!CharMesh)
    {
        TPT_LOG(GALog, Warning, TEXT("Multicast_AttachReplicatedActor: PlayerChar has no mesh."));
        return;
    }

    // Attach the spawned actor's root component to the found character's socket
    if (CharMesh->DoesSocketExist(SocketName))
    {
        RootMeshComp->AttachToComponent(CharMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
        RootMeshComp->SetRelativeLocation(FVector::ZeroVector);
        RootMeshComp->SetRelativeRotation(FRotator::ZeroRotator);
    }
    else
    {
        // fallback: attach to mesh and keep world transform
        RootMeshComp->AttachToComponent(CharMesh, FAttachmentTransformRules::KeepWorldTransform);
    }

    // Ensure the actor replicates movement so clients see it move if needed
    SpawnedActor->SetReplicateMovement(true);
}

// Server RPC: destroy replicated actor
void UGA_HoldItem::C2S_DestroyReplicatedHeldItem_Implementation()
{
    if (ReplicatedHeldActor)
    {
        // This runs on server; Destroy will replicate to clients
        ReplicatedHeldActor->Destroy();
        ReplicatedHeldActor = nullptr;
    }
}
