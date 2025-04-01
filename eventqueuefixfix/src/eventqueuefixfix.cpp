#include "../../../srcwrtimer/extshared/src/extension.h"
#include <CDetour/detours.h>

#include <variant_t.h>
#include "entityoutput.h"

IGameConfig *g_GameConfig1 = nullptr;
IGameConfig *g_GameConfig2 = nullptr;
CDetour *gD_FireOutput = nullptr;
void *g_DeleteAllocator = nullptr;
#if defined(WIN64)
typedef void (*CppDeleteType)(void* allocator, void* thing);
#else
typedef void (__fastcall *CppDeleteType)(void* allocator, void* edx, void* thing);
#endif
CppDeleteType CppDelete = nullptr;
AddEventType AddEvent = nullptr;

void DeleteForMe(void* thing)
{
	//__debugbreak();
#if defined(WIN64)
	CppDelete(g_DeleteAllocator, thing);
#else
	CppDelete(g_DeleteAllocator, NULL, thing);
#endif
}

bool IsPlayer(CBaseEntity* activator)
{
	int entidx = gamehelpers->EntityToBCompatRef(activator);
	return (entidx >= 1 && entidx <= playerhelpers->GetMaxClients());
}

DETOUR_DECL_MEMBER4(CBaseEntityOutput_FireOutput, void, variant_t, Value, CBaseEntity*, pActivator, CBaseEntity*, pCaller, float, fDelay)
{
	if (!IsPlayer(pActivator))
	{
		DETOUR_MEMBER_CALL(CBaseEntityOutput_FireOutput)(Value, pActivator, pCaller, fDelay);
		return;
	}

	//__debugbreak();
	CBaseEntityOutput *self = reinterpret_cast<CBaseEntityOutput*>(this);
	CBaseEntityOutput_FireOutput2(self, Value, pActivator, pCaller, fDelay, DeleteForMe, AddEvent);
}

bool Extension_OnLoad(char* error, size_t maxlength)
{
	if (!gameconfs->LoadGameConfigFile("eventfix.games", &g_GameConfig1, error, maxlength))
		return false;
	if (!g_GameConfig1->GetMemSig("AddEventThree", (void**)&AddEvent))
	{
		strcpy(error, "failed to find AddEventThree signature from the eventqueuefix plugin's eventfix.games.txt file");
		return false;
	}
	if (!gameconfs->LoadGameConfigFile("eventfix2.games", &g_GameConfig2, error, maxlength))
		return false;
	if (!g_GameConfig2->GetMemSig("CppDelete", (void**)&CppDelete))
	{
		strcpy(error, "failed to find CppDelete signature from the eventfix2.games.txt");
		return false;
	}
	if (!g_GameConfig2->GetMemSig("CppDelete_Allocator", &g_DeleteAllocator))
	{
		strcpy(error, "failed to find CppDelete_Allocator signature from the eventfix2.games.txt");
		return false;
	}
#if defined(WIN64)
	g_DeleteAllocator = *(void**)(((uintptr_t)g_DeleteAllocator) + 7 + *(int*)(((uintptr_t)g_DeleteAllocator)+3));
#else
	g_DeleteAllocator = *(void**)(((uintptr_t)g_DeleteAllocator) + 1);
#endif

	CDetourManager::Init(smutils->GetScriptingEngine(), g_GameConfig2);

	gD_FireOutput = DETOUR_CREATE_MEMBER(CBaseEntityOutput_FireOutput, "CBaseEntityOutput::FireOutput");
	gD_FireOutput->EnableDetour();

	return true;
}

void Extension_OnUnload()
{
	gD_FireOutput->Destroy();
	gameconfs->CloseGameConfigFile(g_GameConfig2);
	gameconfs->CloseGameConfigFile(g_GameConfig1);
}

void Extension_OnAllLoaded() {}
void MyExtension::OnHandleDestroy(HandleType_t type, void* object) {}
bool MyExtension::GetHandleApproxSize(HandleType_t type, void* object, unsigned int* size) { return false; }
