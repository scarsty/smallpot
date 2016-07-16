#include "BigPotDll.h"
#include "BigPotPlayer.h"

HBAPI void* MYTHAPI BigPotInit(void* handle)
{
    auto bp = new BigPotPlayer(handle);
    return bp;
}

HBAPI int MYTHAPI BigPotInputVideo(void* handle, char* filename)
{
    if (handle)
    {
        BigPotPlayer* bp = (BigPotPlayer*)handle;
        bp->beginWithFile(filename);
    }
    return 0;
}

HBAPI int MYTHAPI BigPotSeek(void* handle, int seek)
{
    return 0;
}

HBAPI int MYTHAPI BigPotClose(void* handle)
{
    if (handle)
    {
        BigPotPlayer* bp = (BigPotPlayer*)handle;
        delete bp;
    }
    return 0;
}
