#include "GM_Lobby.h"
#include "GS_Lobby.h"

void AGM_Lobby::SetIdentifyChracterData(ECharacterType ChractorType, bool bIsHost)
{
	if (AGS_Lobby* MyGS = GetGameState<AGS_Lobby>())
	{
		MyGS->SetIdentifyChracterData(ChractorType, bIsHost);
	}
}
