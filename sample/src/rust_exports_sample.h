#pragma once

typedef unsigned bigbool;

extern "C" {

bigbool rust_Sample_GetWindowsInfo(char* outbuf, size_t outbuflen);

}
