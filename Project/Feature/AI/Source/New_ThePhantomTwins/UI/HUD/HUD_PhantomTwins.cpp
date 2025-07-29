
#include "HUD_PhantomTwins.h"

#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"

#include "PlayerStatusWidget.h"
#include "ClearItemCounterWidget.h"
#include "PlayerStaminaWidget.h"
#include "PlayerSkillWidget.h"
#include "InventoryWidget.h"
#include "../../Log/TPTLog.h"
#include "Attribute/PlayerAttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "Player/PC_Player.h"
#include "Player/PlayerCharacter.h"


void AHUD_PhantomTwins::BeginPlay()
{
    Super::BeginPlay();

    if (AimDotWidgetClass)
    {
        AimDotWidget = CreateWidget<UUserWidget>(GetWorld(), AimDotWidgetClass);
        if (AimDotWidget)
        {
            AimDotWidget->AddToViewport();
        }
    }

    if (PlayerStatusWidgetClass)
    {
        PlayerStatusWidget = CreateWidget<UPlayerStatusWidget>(GetWorld(), PlayerStatusWidgetClass);
        if (PlayerStatusWidget)
        {
            PlayerStatusWidget->AddToViewport();
        }
    }

    if (PlayerStaminaWidgetClass)
    {
        PlayerStaminaWidget = CreateWidget<UPlayerStaminaWidget>(GetWorld(), PlayerStaminaWidgetClass);
        if (PlayerStaminaWidget)
        {
            PlayerStaminaWidget->AddToViewport();
        }
    }

    if (ClearItemCounterWidgetClass)
    {
        ClearItemCounterWidget = CreateWidget<UClearItemCounterWidget>(GetWorld(), ClearItemCounterWidgetClass);
        if (ClearItemCounterWidget)
        {
            ClearItemCounterWidget->AddToViewport();
        }
    }

    if (PlayerSkillWidgetClass)
    {
        PlayerSkillWidget = CreateWidget<UPlayerSkillWidget>(GetWorld(), PlayerSkillWidgetClass);
        if (PlayerSkillWidget)
        {
            PlayerSkillWidget->AddToViewport();
        }
    }

    if (InventoryWidgetClass)
    {
        InventoryWidget = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidgetClass);
        if (InventoryWidget)
        {
            InventoryWidget->AddToViewport();
            InventoryWidget->LoadInventory(nullptr);       // 테스트용 임시 코드
        }
    }

 //   ASC->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetHPAttribute())
	//.AddUObject(this, &AHUD_PhantomTwins::UpdateHP);
 //   ASC->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetMentalPointAttribute())
 //       .AddUObject(this, &AHUD_PhantomTwins::UpdateMental);
    //ASC->GetGameplayAttributeValueChangeDelegate(AttSet->GetStaminaAttribute())
    //    .AddUObject(this, &AHUD_PhantomTwins::UpdateStamina);

    /*FDelegateHandle TEMP = ASC->GetGameplayAttributeValueChangeDelegate(AttSet->GetCoreEnergyAttribute())
        .AddUObject(this, &AHUD_PhantomTwins::UpdateCoreEnergy);*/
    //TPT_LOG(HUDLog, Log, TEXT("CoreEnergy Delegate 바인딩 실행됨. %d"), TEMP.IsValid());
}

void AHUD_PhantomTwins::TryInitHUD()
{
    APC_Player* PC = Cast<APC_Player>(GetOwningPlayerController());
    if (!PC)
    {
        ScheduleNextInit(); return;
    }

    APlayerCharacter* Pawn = Cast<APlayerCharacter>(PC->GetPawn());
    if (!Pawn)
    {
        ScheduleNextInit(); return;
    }

    APlayerState* PS = Pawn->GetPlayerState();
    if (!PS)
    {
        ScheduleNextInit(); return;
    }

    UAbilitySystemComponent* ASC = PS->FindComponentByClass<UAbilitySystemComponent>();
    if (!ASC)
    {
        ScheduleNextInit(); return;
    }

    const UPlayerAttributeSet* AttSet = ASC->GetSet<UPlayerAttributeSet>();
    if (!AttSet)
    {
        ScheduleNextInit(); return;
    }

    // 모든 것이 준비 완료. 한 번만 실행!
    InitHUD_Internal(PC, Pawn, PS, ASC, AttSet);
    bIsHUDInitDone = true; // 멤버 변수로 만들 것
}
void AHUD_PhantomTwins::ScheduleNextInit()
{
    if (!bIsHUDInitDone)
    {
        // 0.05초 후에 다시 Init 시도 (틱마다 말고, 부담 적게)
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AHUD_PhantomTwins::TryInitHUD);
    }
}

void AHUD_PhantomTwins::InitHUD_Internal(APC_Player* PC, APlayerCharacter* Pawn, APlayerState* PS, UAbilitySystemComponent* ASC, const UPlayerAttributeSet* AttSet)
{
    // HUD 값들 세팅
    UpdateHP(AttSet->GetHP());
    UpdateMental(AttSet->GetMentalPoint());
    UpdateStamina(AttSet->GetStamina());
    UpdateCoreEnergy(AttSet->GetCoreEnergy());
    // 기타 위젯 바인딩, 이벤트 바인드 등등
}
/* PlayerStatus */
void AHUD_PhantomTwins::UpdateHP(const int32 val)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->SetHP(val);
    }
}

void AHUD_PhantomTwins::UpdateMental(const int32 val)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->SetMental(val);
    }
}

void AHUD_PhantomTwins::SetCharPortrait(UTexture2D* PortraitTexture)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->SetCharPortrait(PortraitTexture);
    }
}

void AHUD_PhantomTwins::UpdateStamina(const int32 val)
{
    if (PlayerStaminaWidget)
    {
        PlayerStaminaWidget->SetStamina(val);
    }
}


/* ClearItem */
void AHUD_PhantomTwins::UpdateClearItem(int32 CurrentClearItem)
{
    if (ClearItemCounterWidget)
    {
        ClearItemCounterWidget->SetClearItemCount(CurrentClearItem);
    }
}


/* Skill */
void AHUD_PhantomTwins::SetActiveSkillIcon(UTexture2D* ActiveSkillIcon)
{
    if (PlayerSkillWidget)
    {
        PlayerSkillWidget->SetActiveSkillIcon(ActiveSkillIcon);
    }
}

void AHUD_PhantomTwins::SetPassiveSkillIcon(UTexture2D* PassiveSkillIcon)
{
    if (PlayerSkillWidget)
    {
        PlayerSkillWidget->SetPassiveSkillIcon(PassiveSkillIcon);
    }
}

void AHUD_PhantomTwins::UpdateCoreEnergy(const int32 val)
{
    if (PlayerSkillWidget)
    {
        PlayerSkillWidget->ShowCoreEnergy(val);
    }
}