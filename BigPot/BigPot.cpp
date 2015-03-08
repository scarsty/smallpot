// BigPot.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include "BigPotPlayer.h"


int main(int argc, char* argv[])
{
	auto bp = new BigPotPlayer;
	char *s = "";
	if (argc > 1)
	{
		s = argv[1];
	}
	bp->beginWithFile(s);
	return 0;
}

