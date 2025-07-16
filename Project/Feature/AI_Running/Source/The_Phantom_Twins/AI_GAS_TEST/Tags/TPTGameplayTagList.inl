// TPTGameplayTagList.inl


// 형식: TPT_TAG_GEN(태그종류, 이름, 설명)
TPT_TAG_GEN(InputTag, Sprint,	"Input: Sprint (Shift)")
TPT_TAG_GEN(InputTag, Interact, "Input: Interact (F)")
TPT_TAG_GEN(InputTag, Dash,		"Input: Dash (E)")
TPT_TAG_GEN(InputTag, Heal,		"Input: Heal (Q)")
TPT_TAG_GEN(InputTag, Crouch,	"Input: Crouch (C)")

TPT_TAG_GEN(Ability, Dash,	"Ability: Dash")
TPT_TAG_GEN(Ability, Heal, "Ability: Heal")

TPT_TAG_GEN(Cooldown, Dash, "Cooldown: Dash")
TPT_TAG_GEN(Cooldown, Heal, "Cooldown: Heal")

TPT_TAG_GEN(State, Stunned, "Character is stunned")

TPT_TAG_GEN(Data, StaminaRegen, "Stamina for delivering recovery")
TPT_TAG_GEN(Data, MPRegen,		"MP recovery for delivering recovery")



// AI
TPT_TAG_GEN(AIState, Default,			"AI State: Default")
TPT_TAG_GEN(AIState, Suspicion,			"AI State: Suspicion")
TPT_TAG_GEN(AIState, Combat,			"AI State: Combat")
TPT_TAG_GEN(AIState, Stun,				"AI State: Stun")
TPT_TAG_GEN(AIState, PerformingAction,	"AI State: PerformingAction")


TPT_TAG_GEN(AIAction, OpenDoor,			"AI Action: OpenDoor")
TPT_TAG_GEN(AIAction, MeleeAttack,		"AI Action: MeleeAttack")
TPT_TAG_GEN(AIAction, SmashObstacle,	"AI Action: SmashObstacle")

// VFX
TPT_TAG_GEN(VFX, MeleeImpact, "VFX: MeleeAttack")