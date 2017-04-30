#include "PotDll.h"
#include "PotPlayer.h"

HBAPI void* MYTHAPI PotInit(void* handle)
{
    auto bp = new PotPlayer(handle);
    return bp;
}

HBAPI int MYTHAPI PotInputVideo(void* handle, char* filename)
{
    if (handle)
    {
        PotPlayer* bp = (PotPlayer*)handle;
        bp->beginWithFile(filename);
    }
    return 0;
}

HBAPI int MYTHAPI PotSeek(void* handle, int seek)
{
    return 0;
}

HBAPI int MYTHAPI PotClose(void* handle)
{
    if (handle)
    {
        PotPlayer* bp = (PotPlayer*)handle;
        delete bp;
    }
    return 0;
}
