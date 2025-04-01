#include "../../../srcwrtimer/extshared/src/extension.h"
#include "../../../srcwrtimer/extshared/src/coreident.hpp"
#include "rust_exports_sample.h"


extern const sp_nativeinfo_t SampleNatives[];


void MyExtension::OnHandleDestroy(HandleType_t type, void* object) {}
bool MyExtension::GetHandleApproxSize(HandleType_t type, void* object, unsigned int* size) { return false; }


bool Extension_OnLoad(char* error, size_t maxlength)
{
	sharesys->AddNatives(myself, SampleNatives);
	return true;
}

void Extension_OnUnload()
{
}

void Extension_OnAllLoaded() {}

static cell_t N_Sample_GetWindowsInfo(IPluginContext* ctx, const cell_t* params)
{
	char *outbuf;
	(void)ctx->LocalToString(params[1], &outbuf);
	cell_t outbuflen = params[2];
	return rust_Sample_GetWindowsInfo(outbuf, outbuflen);
}

extern const sp_nativeinfo_t SampleNatives[] = {
	{"Sample_GetWindowsInfo", N_Sample_GetWindowsInfo},
	{NULL, NULL}
};
