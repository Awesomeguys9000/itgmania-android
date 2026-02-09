#ifndef RAGE_SOUND_DRIVER_SDL_H
#define RAGE_SOUND_DRIVER_SDL_H

#include "RageSoundDriver.h"
#include <SDL.h>
#include <atomic>

class RageSoundDriver_SDL : public RageSoundDriver
{
public:
	RageSoundDriver_SDL();
	virtual ~RageSoundDriver_SDL();

    // RageSoundDriver impl
    virtual RString Init();
	virtual int64_t GetPosition() const;
	virtual float GetPlayLatency() const;
	virtual int GetSampleRate() const;

    // SDL Callback
    static void AudioCallback( void *userdata, Uint8 *stream, int len );
    void FillAudio( Uint8 *stream, int len );

private:
	int m_iSampleRate;
    int m_iBufferSize; // in frames
    SDL_AudioDeviceID m_DeviceID;
    std::atomic<int64_t> m_iHardwareFrame;
};

#endif
