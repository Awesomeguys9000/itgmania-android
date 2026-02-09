#include "global.h"
#include "LowLevelWindow_SDL.h"
#include "RageLog.h"
#include "RageDisplay.h"
#include "DisplaySpec.h"
#include "RageUtil.h"
#include <SDL.h>

LowLevelWindow_SDL::LowLevelWindow_SDL()
{
	m_Window = nullptr;
	m_Context = nullptr;
}

LowLevelWindow_SDL::~LowLevelWindow_SDL()
{
	if( m_Context )
		SDL_GL_DeleteContext( m_Context );
	if( m_Window )
		SDL_DestroyWindow( m_Window );
}

void *LowLevelWindow_SDL::GetProcAddress( RString s )
{
	return (void*)SDL_GL_GetProcAddress( s.c_str() );
}

RString LowLevelWindow_SDL::TryVideoMode( const VideoModeParams &p, bool &bNewDeviceOut )
{
	LOG->Info( "TryVideoMode: %dx%d %s", p.width, p.height, p.windowed ? "Windowed" : "Fullscreen" );

	if( m_Window == nullptr )
	{
		// Set OpenGL ES 2.0 attributes
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

		// Create window (fullscreen on Android)
		m_Window = SDL_CreateWindow(
			"ITGMania",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			0, 0,
			SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE
		);

		if( !m_Window )
			return ssprintf( "SDL_CreateWindow failed: %s", SDL_GetError() );

		m_Context = SDL_GL_CreateContext( m_Window );
		if( !m_Context )
			return ssprintf( "SDL_GL_CreateContext failed: %s", SDL_GetError() );

        // Enable VSync based on params
        SDL_GL_SetSwapInterval( p.vsync ? 1 : 0 );

		bNewDeviceOut = true;
	}
    else
    {
        bNewDeviceOut = false;
    }

	// Get actual window size
	int w, h;
	SDL_GetWindowSize( m_Window, &w, &h );

    // Update actual params
    m_ActualParams = ActualVideoModeParams( p );
    m_ActualParams.width = w;
    m_ActualParams.height = h;
    m_ActualParams.windowWidth = w;
    m_ActualParams.windowHeight = h;
    m_ActualParams.windowed = false; // Always fullscreen on Android

	return ""; // Success
}

void LowLevelWindow_SDL::GetDisplaySpecs( DisplaySpecs &out ) const
{
    if( m_Window )
    {
        int w, h;
        SDL_GetWindowSize( m_Window, &w, &h );

        DisplayMode mode = { (unsigned int)w, (unsigned int)h, 60.0 }; // Assume 60Hz

        DisplaySpec spec("0", "Android Display", mode);
        out.insert(spec);
    }
}

void LowLevelWindow_SDL::SwapBuffers()
{
	if( m_Window )
		SDL_GL_SwapWindow( m_Window );
}

void LowLevelWindow_SDL::Update()
{
}

const ActualVideoModeParams LowLevelWindow_SDL::GetActualVideoModeParams() const
{
	return m_ActualParams;
}
