#pragma once
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
#ifdef _WINDLL
HBAPI void* MYTHAPI PotCreateFromHandle(void* handle);
HBAPI void* MYTHAPI PotCreateFromWindow(void* handle);
HBAPI int MYTHAPI PotInputVideo(void* pot, char* filename);
HBAPI int MYTHAPI PotPlayVideo(void* pot, char* filename, float volume);
HBAPI int MYTHAPI PotSeek(void* pot, int seek);
HBAPI int MYTHAPI PotDestory(void* pot);
#endif
