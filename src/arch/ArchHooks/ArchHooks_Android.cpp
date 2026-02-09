#include "global.h"
#include "ArchHooks_Android.h"
#include "RageLog.h"
#include <SDL.h>
#include <unistd.h>

ArchHooks_Android::ArchHooks_Android()
{
}

ArchHooks_Android::~ArchHooks_Android()
{
    SDL_Quit();
}

void ArchHooks_Android::Init()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0)
    {
        LOG->Warn("SDL_Init failed: %s", SDL_GetError());
        RageException::Throw("SDL_Init failed: %s", SDL_GetError());
    }

    // Set working directory to internal storage
    const char *path = SDL_AndroidGetInternalStoragePath();
    if (path) {
        LOG->Info("Setting CWD to: %s", path);
        if (chdir(path) != 0) {
            LOG->Warn("chdir failed: %s", strerror(errno));
        }
    } else {
        LOG->Warn("SDL_AndroidGetInternalStoragePath returned NULL");
    }
}

void ArchHooks_Android::DumpDebugInfo()
{
    ArchHooks::DumpDebugInfo();
    LOG->Info("ArchHooks_Android::DumpDebugInfo");
    SDL_version v;
    SDL_GetVersion(&v);
    LOG->Info("SDL Version: %d.%d.%d", v.major, v.minor, v.patch);
}
