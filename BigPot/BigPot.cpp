// BigPot.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include "BigPotPlayer.h"


int main(int argc, char* argv[])
{
	auto bp = new BigPotPlayer(argv[0]);
	bp->beginWithFile(argc > 1 ? argv[1] : "");
	delete bp;
	return 0;
}

