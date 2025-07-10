#include "GM_Lobby.h"
#include "GS_Lobby.h"

void AGM_Lobby::SetIdentifyCharacterData(ECharacterType ChractorType, bool bIsHost)
{
	if (AGS_Lobby* MyGS = GetGameState<AGS_Lobby>())
	{
		MyGS->SetIdentifyCharacterData(ChractorType, bIsHost);
	}
}
