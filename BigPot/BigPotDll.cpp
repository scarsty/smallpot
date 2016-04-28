#include "BigPotDll.h"
#include "BigPotPlayer.h"

HBAPI int MYTHAPI BigPotInit(void* handle)
{

	auto bp = new BigPotPlayer();
	bp->beginWithFile(argc > 1 ? argv[1] : "");
	delete bp;
	return 0;
}

HBAPI int MYTHAPI BigPotInputVideo(char* filename)
{

}

HBAPI int MYTHAPI BigPotSeek(int seek)
{

}

HBAPI int MYTHAPI BigPotClose()
{

}

