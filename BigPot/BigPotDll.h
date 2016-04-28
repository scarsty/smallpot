#pragma once
//#include <Windows.h>
#define MYTHAPI _stdcall
#ifdef __cplusplus   
#define HBAPI extern "C" __declspec (dllexport)   
#else   
#define HBAPI __declspec (dllexport)   
#endif   

HBAPI int MYTHAPI BigPotInit(void* handle);
HBAPI int MYTHAPI BigPotInputVideo(char* filename);
HBAPI int MYTHAPI BigPotSeek(int seek);
HBAPI int MYTHAPI BigPotClose();