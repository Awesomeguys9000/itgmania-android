#ifndef LOW_LEVEL_WINDOW_SDL_H
#define LOW_LEVEL_WINDOW_SDL_H

#include "LowLevelWindow.h"
#include <SDL.h>

class LowLevelWindow_SDL : public LowLevelWindow
{
public:
	LowLevelWindow_SDL();
	virtual ~LowLevelWindow_SDL();

	virtual void *GetProcAddress( RString s );
	virtual RString TryVideoMode( const VideoModeParams &p, bool &bNewDeviceOut );
	virtual void GetDisplaySpecs( DisplaySpecs &out ) const;

	virtual void SwapBuffers();
	virtual void Update();

	virtual const ActualVideoModeParams GetActualVideoModeParams() const;

private:
	SDL_Window *m_Window;
	SDL_GLContext m_Context;
	ActualVideoModeParams m_ActualParams;
};

#ifdef ARCH_LOW_LEVEL_WINDOW
#error "More than one LowLevelWindow selected!"
#endif
#define ARCH_LOW_LEVEL_WINDOW LowLevelWindow_SDL

#endif
