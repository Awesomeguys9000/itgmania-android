#include "global.h"
#include "InputHandler_SDL.h"
#include "RageLog.h"
#include "ArchHooks/ArchHooks.h"
#include "RageInputDevice.h"
#include "RageDisplay.h"

REGISTER_INPUT_HANDLER_CLASS2( SDL, SDL );

InputHandler_SDL::InputHandler_SDL()
{
	if (SDL_InitSubSystem( SDL_INIT_GAMECONTROLLER ) < 0) {
        LOG->Warn("SDL_InitSubSystem(GAMECONTROLLER) failed: %s", SDL_GetError());
    }
}

InputHandler_SDL::~InputHandler_SDL()
{
	SDL_QuitSubSystem( SDL_INIT_GAMECONTROLLER );
}

void InputHandler_SDL::Update()
{
	SDL_Event event;
	while( SDL_PollEvent( &event ) )
	{
		HandleEvent( event );
	}

    InputHandler::UpdateTimer();
}

void InputHandler_SDL::GetDevicesAndDescriptions( std::vector<InputDeviceInfo>& vDevicesOut )
{
	vDevicesOut.push_back( InputDeviceInfo( DEVICE_KEYBOARD, "Keyboard" ) );
    for( int i = 0; i < SDL_NumJoysticks(); ++i )
    {
        vDevicesOut.push_back( InputDeviceInfo( (InputDevice)(DEVICE_JOY1 + i), ssprintf("Joystick %d", i+1) ) );
    }
}

static DeviceButton SDLKeyToDeviceButton( SDL_Keycode key )
{
    if( key >= SDLK_a && key <= SDLK_z ) return (DeviceButton)(KEY_Ca + (key - SDLK_a));
    if( key >= SDLK_0 && key <= SDLK_9 ) return (DeviceButton)(KEY_C0 + (key - SDLK_0));

    switch( key )
    {
    case SDLK_ESCAPE: return KEY_ESC;
    case SDLK_RETURN: return KEY_ENTER;
    case SDLK_TAB: return KEY_TAB;
    case SDLK_SPACE: return KEY_SPACE;
    case SDLK_BACKSPACE: return KEY_BACK;

    case SDLK_UP: return KEY_UP;
    case SDLK_DOWN: return KEY_DOWN;
    case SDLK_LEFT: return KEY_LEFT;
    case SDLK_RIGHT: return KEY_RIGHT;

    case SDLK_LSHIFT: return KEY_LSHIFT;
    case SDLK_RSHIFT: return KEY_RSHIFT;
    case SDLK_LCTRL: return KEY_LCTRL;
    case SDLK_RCTRL: return KEY_RCTRL;
    case SDLK_LALT: return KEY_LALT;
    case SDLK_RALT: return KEY_RALT;

    case SDLK_F1: return KEY_F1;
    case SDLK_F2: return KEY_F2;
    case SDLK_F3: return KEY_F3;
    case SDLK_F4: return KEY_F4;
    case SDLK_F5: return KEY_F5;
    case SDLK_F6: return KEY_F6;
    case SDLK_F7: return KEY_F7;
    case SDLK_F8: return KEY_F8;
    case SDLK_F9: return KEY_F9;
    case SDLK_F10: return KEY_F10;
    case SDLK_F11: return KEY_F11;
    case SDLK_F12: return KEY_F12;

    case SDLK_DELETE: return KEY_DEL;
    case SDLK_INSERT: return KEY_INSERT;
    case SDLK_HOME: return KEY_HOME;
    case SDLK_END: return KEY_END;
    case SDLK_PAGEUP: return KEY_PGUP;
    case SDLK_PAGEDOWN: return KEY_PGDN;

    case SDLK_KP_0: return KEY_KP_C0;
    case SDLK_KP_1: return KEY_KP_C1;
    case SDLK_KP_2: return KEY_KP_C2;
    case SDLK_KP_3: return KEY_KP_C3;
    case SDLK_KP_4: return KEY_KP_C4;
    case SDLK_KP_5: return KEY_KP_C5;
    case SDLK_KP_6: return KEY_KP_C6;
    case SDLK_KP_7: return KEY_KP_C7;
    case SDLK_KP_8: return KEY_KP_C8;
    case SDLK_KP_9: return KEY_KP_C9;
    case SDLK_KP_DIVIDE: return KEY_KP_SLASH;
    case SDLK_KP_MULTIPLY: return KEY_KP_ASTERISK;
    case SDLK_KP_MINUS: return KEY_KP_HYPHEN;
    case SDLK_KP_PLUS: return KEY_KP_PLUS;
    case SDLK_KP_PERIOD: return KEY_KP_PERIOD;
    case SDLK_KP_ENTER: return KEY_KP_ENTER;
    case SDLK_KP_EQUALS: return KEY_KP_EQUAL;

    case SDLK_AC_BACK: return KEY_ESC;

    default: return DeviceButton_Invalid;
    }
}

static DeviceButton SDLControllerButtonToDeviceButton( Uint8 button )
{
    switch( button )
    {
    case SDL_CONTROLLER_BUTTON_A: return JOY_BUTTON_1;
    case SDL_CONTROLLER_BUTTON_B: return JOY_BUTTON_2;
    case SDL_CONTROLLER_BUTTON_X: return JOY_BUTTON_3;
    case SDL_CONTROLLER_BUTTON_Y: return JOY_BUTTON_4;
    case SDL_CONTROLLER_BUTTON_BACK: return JOY_BUTTON_5;
    case SDL_CONTROLLER_BUTTON_GUIDE: return JOY_BUTTON_6;
    case SDL_CONTROLLER_BUTTON_START: return JOY_BUTTON_7;
    case SDL_CONTROLLER_BUTTON_LEFTSTICK: return JOY_BUTTON_8;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return JOY_BUTTON_9;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return JOY_BUTTON_10;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return JOY_BUTTON_11;
    case SDL_CONTROLLER_BUTTON_DPAD_UP: return JOY_UP;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return JOY_DOWN;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return JOY_LEFT;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return JOY_RIGHT;
    default: return DeviceButton_Invalid;
    }
}

void InputHandler_SDL::HandleEvent( const SDL_Event &event )
{
	switch( event.type )
	{
	case SDL_QUIT:
		ArchHooks::SetUserQuit();
		break;

    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
        DeviceButton button = SDLKeyToDeviceButton( event.key.keysym.sym );
        if( button != DeviceButton_Invalid )
        {
            ButtonPressed( DeviceInput( DEVICE_KEYBOARD, button, event.key.type == SDL_KEYDOWN ? 1.0f : 0.0f ) );
        }
        break;
    }

    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
    {
        InputDevice device = DEVICE_JOY1;

        DeviceButton button = SDLControllerButtonToDeviceButton( event.cbutton.button );
        if( button != DeviceButton_Invalid )
        {
            ButtonPressed( DeviceInput( device, button, event.cbutton.state == SDL_PRESSED ? 1.0f : 0.0f ) );
        }
        break;
    }

    case SDL_CONTROLLERDEVICEADDED:
        SDL_GameControllerOpen( event.cdevice.which );
        break;

    case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
            event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            if (DISPLAY)
                DISPLAY->ResolutionChanged();
        }
        break;
	}
}
