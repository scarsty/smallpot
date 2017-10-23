// BigPot.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include "PotPlayer.h"

#ifndef _LIB
int main(int argc, char* argv[])
{
    auto bp = new PotPlayer(argv[0]);
    bp->beginWithFile(argc > 1 ? argv[1] : "");
    delete bp;
    return 0;
}
#endif

