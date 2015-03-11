// BigPot.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include "BigPotPlayer.h"


int main(int argc, char* argv[])
{
	char *s = "";
	if (argc > 1)
	{
		s = argv[1];
	}

	auto bp = new BigPotPlayer;
	bp->setFilePath(argv[0]);
	bp->beginWithFile(s);
	delete bp;

	return 0;
}

