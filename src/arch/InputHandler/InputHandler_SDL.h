#ifndef INPUT_HANDLER_SDL_H
#define INPUT_HANDLER_SDL_H

#include "InputHandler.h"
#include <SDL.h>

class InputHandler_SDL : public InputHandler
{
public:
	InputHandler_SDL();
	virtual ~InputHandler_SDL();

	virtual void Update();
	virtual void GetDevicesAndDescriptions( std::vector<InputDeviceInfo>& vDevicesOut );

private:
	void HandleEvent( const SDL_Event &event );
};

#endif
