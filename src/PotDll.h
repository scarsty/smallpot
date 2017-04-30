#pragma once
//#include <Windows.h>
#ifdef _MSC_VER 
#define MYTHAPI _stdcall
#ifdef __cplusplus
#define HBAPI extern "C" __declspec (dllexport)
#else
#define HBAPI __declspec (dllexport)
#endif
#else
#define HBAPI
#define  MYTHAPI
#endif

HBAPI void* MYTHAPI PotInit(void* handle);
HBAPI int MYTHAPI PotInputVideo(void* handle, char* filename);
HBAPI int MYTHAPI PotSeek(void* handle, int seek);
HBAPI int MYTHAPI PotClose(void* handle);