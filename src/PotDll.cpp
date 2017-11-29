#include "PotDll.h"
#include "PotPlayer.h"

#ifdef _LIB

void* PotCreateFromHandle(void* handle)
{
    auto bp = new PotPlayer(handle);
    return bp;
}

void* PotCreateFromWindow(void* handle)
{
    auto bp = new PotPlayer((BP_Window*)handle, 1);
    return bp;
}

int PotInputVideo(void* pot, char* filename)
{
    int ret = 0;
    if (pot)
    {
        PotPlayer* bp = (PotPlayer*)pot;
        ret = bp->beginWithFile(filename);
    }
    return ret;
}

int PotSeek(void* pot, int seek)
{
    return 0;
}

int PotDestory(void* pot)
{
    if (pot)
    {
        PotPlayer* bp = (PotPlayer*)pot;
        delete bp;
    }
    return 0;
}

#endif
