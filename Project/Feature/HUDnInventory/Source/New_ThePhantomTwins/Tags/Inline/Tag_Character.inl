// TPTGameplayTagList.inl


// Çü½Ä: TPT_TAG_GEN(Type, Properties, Name, "Description")


/*
	Player
*/
// State
TPT_TAG_GEN(Character, State, LowHP, "Character's HP is Low (HP < MaxHP * 0.3)")
TPT_TAG_GEN(Character, State, Downed, "Character is Downed (HP <= 0)")
TPT_TAG_GEN(Character, State, Confused1st, "Character is Confused 1st")
TPT_TAG_GEN(Character, State, Confused2nd, "Character is Confused 2nd")
TPT_TAG_GEN(Character, State, Confused3rd, "Character is Confused 3rd")


TPT_TAG_GEN(Character, State, SkillCoolDown, "Character's skill is CoolDown")
TPT_TAG_GEN(Character, State, Sprinting, "Character is Sprinting")
TPT_TAG_GEN(Character, State, UsingCamera, "Character is Using Camera")
TPT_TAG_GEN(Character, State, SeeingThrough, "Character is Seeing Through")


// Active Skill
TPT_TAG_GEN(Character, Skill, Sprint, "Character use Sprint Skill")
TPT_TAG_GEN(Character, Skill, Camera, "Character use Camera Skill")
TPT_TAG_GEN(Character, Skill, SeeThrough, "Character use SeeThrough Skill")
TPT_TAG_GEN(Character, Skill, RandomActive, "Character use Random Active skill")

// Passive Skill
TPT_TAG_GEN(Character, Skill, HPBuff, "Character HPBuff")
TPT_TAG_GEN(Character, Skill, StaminaBuff, "Character StaminaBuff")
TPT_TAG_GEN(Character, Skill, MentalBuff, "Character MentalBuff")
TPT_TAG_GEN(Character, Skill, StarterKit, "Character take Item")


/*
	AI
*/
// AI State
TPT_TAG_GEN(Character, AIState, Default, "AI State: Default")
TPT_TAG_GEN(Character, AIState, Suspicion, "AI State: Suspicion")
TPT_TAG_GEN(Character, AIState, Combat, "AI State: Combat")
TPT_TAG_GEN(Character, AIState, Stun, "AI State: Stun")
TPT_TAG_GEN(Character, AIState, PerformingAction, "AI State: PerformingAction")


// AI Action
TPT_TAG_GEN(Character, Action, MeleeAttack, "AI Action: MeleeAttack")
TPT_TAG_GEN(Character, Action, SmashObstacle, "AI Action: SmashObstacle")
TPT_TAG_GEN(Character, Action, OpenObstacle, "AI Action: OpenObstacle")



/*
	Common
*/
// Action
TPT_TAG_GEN(Character, Action, InteractActor, "Action: GA_InteractActor")



// Identifier
TPT_TAG_GEN(Character, Identifier, Player, "Identifier: Player")
TPT_TAG_GEN(Character, Identifier, AI, "Identifier: AI")
TPT_TAG_GEN(Character, Identifier, Object, "Identifier: Object")

