#include <iostream>
#include <Windows.h>
#include <string>
#include "steam/steam_api.h"
#include "steam/isteamuserstats.h"
#include "steam/isteamremotestorage.h"
#include "steam/isteammatchmaking.h"
#include "steam/steam_gameserver.h"

#pragma comment(lib, "steam_api.lib")
#pragma comment(lib, "win64/steam_api64")

using std::cout;
using std::endl;
using std::cin;

std::string GameToSearchFor = "";
std::string ChecksumToFind = "";
std::string FindStr = "";

void ShowSteamIDInfo(const char* info, CSteamID id) {
	printf("%s %llu - %u %u %u %u\n", info,
		id.ConvertToUint64(),
		id.GetAccountID(), id.GetUnAccountInstance(), id.GetEAccountType(), id.GetEUniverse());
}

class CLobbyListManager
{
private:
	CCallResult<CLobbyListManager, LobbyMatchList_t > m_CallResultLobbyMatchList;
	void OnLobbyMatchList(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure)
	{
		for (int iLobby = 0; iLobby < pLobbyMatchList->m_nLobbiesMatching; iLobby++)
		{
			CSteamID steamIDLobby = SteamMatchmaking()->GetLobbyByIndex(iLobby);
			ShowSteamIDInfo("\nLobby SteamID:", steamIDLobby);
			int nData = SteamMatchmaking()->GetLobbyDataCount(steamIDLobby);
			char key[k_nMaxLobbyKeyLength];
			char value[k_cubChatMetadataMax];
			for (int i = 0; i < nData; ++i)
			{
				bool bSuccess = SteamMatchmaking()->GetLobbyDataByIndex(steamIDLobby, i, key, k_nMaxLobbyKeyLength, value, k_cubChatMetadataMax);
				if (bSuccess)
				{
					printf("Lobby Data %d, Key: \"%s\" - Value: \"%s\"\n", i, key, value);
				}
			}
		}
	}
public:
	void FindLobbies()
	{
		SteamMatchmaking()->AddRequestLobbyListResultCountFilter(9999);
		SteamMatchmaking()->AddRequestLobbyListDistanceFilter(k_ELobbyDistanceFilterWorldwide);
		SteamMatchmaking()->AddRequestLobbyListStringFilter("version", FindStr.c_str(), k_ELobbyComparisonEqual);
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
		m_CallResultLobbyMatchList.Set(hSteamAPICall, this, &CLobbyListManager::OnLobbyMatchList);
		Sleep(5000);
		SteamAPI_RunCallbacks();
	}
};

int main()
{
	if (!SteamAPI_Init())
	{
		cout << "[MastoidSteamworks] SteamAPI_Init() failed" << endl;
		cout << "[MastoidSteamworks] Fatal Error: Steam must be running to play this game (SteamAPI_Init() failed)." << endl;
	}
	uint32 AppID = SteamUtils()->GetAppID();

	cout << "\n[MastoidSteamworks] Loaded.\n[MastoidSteamworks] Please input checksum to search for:" << endl;
	cin >> ChecksumToFind;
	if (AppID == 394360)
	{
		FindStr = std::string("Barbarossa v1.11.4.e26e (") + std::string(ChecksumToFind) + std::string(")");
		GameToSearchFor = "hoi4";
	}
	else if (AppID == 236850)
	{
		FindStr = std::string("EU4 v1.32.2.0 Songhai (") + std::string(ChecksumToFind) + std::string(")");
		GameToSearchFor = "eu4";
	}
	else
	{
		cout << "[MastoidSteamworks] steam_appid.txt does not match eu4 or hoi4." << endl;
	}
	cout << "[MastoidSteamworks] Now searching for game " << GameToSearchFor << " with checksum " << ChecksumToFind << ". Press <insert> to search. (eta 5 seconds)" << endl;

	do
	{
		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			CLobbyListManager m;

			m.FindLobbies();
		}

		Sleep(100);
	} while (true);

	return EXIT_SUCCESS;
}
