#include <SDL.h>

extern int sm_main(int argc, char* argv[]);

extern "C" int SDL_main(int argc, char* argv[])
{
    // sm_main expects argv[0] to be the program path.
    // SDL might provide it, or we might need to construct it.
    // argc/argv from SDL_main should be passed through.
    return sm_main(argc, argv);
}
