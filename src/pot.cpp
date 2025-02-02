
#include "PotPlayer.h"
#include "SDL3/SDL_main.h"

#ifndef _WINDLL
//in SDL, this is not the real main function, the argc is of utf-8, not ansi on Windows
int main(int argc, char* argv[])
{
#ifdef _WIN32
    system("chcp 65001");
#endif
    PotPlayer bp(argv[0]);
    bp.beginWithFile(argc > 1 ? argv[1] : "");
    return 0;
}
#endif
