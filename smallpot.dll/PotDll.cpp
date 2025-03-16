﻿#include "PotDll.h"
#include "PotPlayer.h"

#ifdef _WINDLL

HBAPI void* MYTHAPI PotCreateFromHandle(void* handle)
{
    auto bp = new PotPlayer(handle);
    return bp;
}

HBAPI void* MYTHAPI PotCreateFromWindow(void* handle)
{
    auto bp = new PotPlayer((Window*)handle, 1);
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

HBAPI int MYTHAPI PotPlayVideo(void* pot, char* filename, float volume)
{
    int ret = 0;
    if (pot)
    {
        PotPlayer* bp = (PotPlayer*)pot;
        bp->setVolume(volume);
        ret = bp->beginWithFile(filename);
    }
    return ret;
}

HBAPI int MYTHAPI PotSeek(void* pot, int seek)
{
    if (pot)
    {
        PotPlayer* bp = (PotPlayer*)pot;
        bp->seekTime(seek);
    }
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

#endif
