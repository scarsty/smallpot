// BigPot.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include "BigPotPlayer.h"


int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		auto bp = new BigPotPlayer;
		bp->playFile(argv[1]);
	}
	return 0;
}

