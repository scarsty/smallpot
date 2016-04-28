#pragma once
//#include <Windows.h>
#define MYTHAPI _stdcall
#ifdef __cplusplus   
#define HBAPI extern "C" __declspec (dllexport)   
#else   
#define HBAPI __declspec (dllexport)   
#endif   

HBAPI void* MYTHAPI BigPotInit(void* handle);
HBAPI int MYTHAPI BigPotInputVideo(void* handle,char* filename);
HBAPI int MYTHAPI BigPotSeek(void* handle, int seek);
HBAPI int MYTHAPI BigPotClose(void* handle);