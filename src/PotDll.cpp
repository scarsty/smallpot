#include "PotDll.h"
#include "PotPlayer.h"

HBAPI void* MYTHAPI PotCreateFromHandle(void* handle)
{
    auto bp = new PotPlayer(handle);
    return bp;
}

HBAPI void* MYTHAPI PotCreateFromWindow(void* handle)
{
    auto bp = new PotPlayer((BP_Window*)handle, 1);
    return bp;
}

HBAPI int MYTHAPI PotInputVideo(void* pot, char* filename)
{
    int ret = 0;
    if (pot)
    {
        PotPlayer* bp = (PotPlayer*)pot;
        ret = bp->beginWithFile(filename);
    }
    return ret;
}

HBAPI int MYTHAPI PotSeek(void* pot, int seek)
{
    return 0;
}

HBAPI int MYTHAPI PotDestory(void* pot)
{
    if (pot)
    {
        PotPlayer* bp = (PotPlayer*)pot;
        delete bp;
    }
    return 0;
}
