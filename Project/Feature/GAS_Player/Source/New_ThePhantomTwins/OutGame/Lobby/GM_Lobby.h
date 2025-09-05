#pragma once

#include "CoreMinimal.h"
#include "../../GM_PhantomTwins.h"
#include "Data/CharacterType.h"
#include "GM_Lobby.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API AGM_Lobby : public AGM_PhantomTwins
{
	GENERATED_BODY()
	
public:
	void SetIdentifyCharacterData(ECharacterType ChractorType, bool bIsHost);
};
