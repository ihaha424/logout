#pragma once

#include "CoreMinimal.h"
#include "../../GM_PhantomTwins.h"
#include "../../CharacterType.h"
#include "GM_Lobby.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API AGM_Lobby : public AGM_PhantomTwins
{
	GENERATED_BODY()
	
public:
	void SetIdentifyChracterData(ECharacterType ChractorType, bool bIsHost);
};
