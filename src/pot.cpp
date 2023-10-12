
#include "PotPlayer.h"

#ifndef _WINDLL
//in SDL, this is not the real main function, the argc is of utf-8, not ansi on Windows
int main(int argc, char* argv[])
{
#ifdef _WIN32
    system("chcp 65001");
#endif
    auto bp = new PotPlayer(argv[0]);
    bp->beginWithFile(argc > 1 ? argv[1] : "");
    delete bp;
    return 0;
}
#endif
