#include "global.h"
#include "RageSoundDriver_SDL.h"
#include "RageLog.h"
#include "PrefsManager.h"
#include "RageUtil.h"

REGISTER_SOUND_DRIVER_CLASS2( SDL, SDL );

RageSoundDriver_SDL::RageSoundDriver_SDL()
    : RageSoundDriver(), m_iSampleRate(44100), m_iBufferSize(0), m_DeviceID(0), m_iHardwareFrame(0)
{
}

RageSoundDriver_SDL::~RageSoundDriver_SDL()
{
    if( m_DeviceID != 0 )
    {
        SDL_CloseAudioDevice( m_DeviceID );
    }
}

RString RageSoundDriver_SDL::Init()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        return ssprintf("SDL_InitSubSystem(AUDIO) failed: %s", SDL_GetError());
    }

    SDL_AudioSpec want, have;
    SDL_zero(want);

    m_iSampleRate = PREFSMAN->m_iSoundPreferredSampleRate;
    if( m_iSampleRate == 0 )
        m_iSampleRate = 44100;

    want.freq = m_iSampleRate;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 1024; // Reasonable buffer size
    want.callback = AudioCallback;
    want.userdata = this;

    m_DeviceID = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

    if( m_DeviceID == 0 )
    {
        return ssprintf( "SDL_OpenAudioDevice failed: %s", SDL_GetError() );
    }

    m_iSampleRate = have.freq;
    m_iBufferSize = have.samples;

    StartDecodeThread();

    SDL_PauseAudioDevice( m_DeviceID, 0 );

    return "";
}

int64_t RageSoundDriver_SDL::GetPosition() const
{
    return m_iHardwareFrame.load();
}

float RageSoundDriver_SDL::GetPlayLatency() const
{
    if (m_iSampleRate > 0)
        return (float)m_iBufferSize / (float)m_iSampleRate;
    return 0.0f;
}

int RageSoundDriver_SDL::GetSampleRate() const
{
    return m_iSampleRate;
}

void RageSoundDriver_SDL::AudioCallback( void *userdata, Uint8 *stream, int len )
{
    RageSoundDriver_SDL *driver = (RageSoundDriver_SDL*)userdata;
    if( driver )
        driver->FillAudio( stream, len );
}

void RageSoundDriver_SDL::FillAudio( Uint8 *stream, int len )
{
    int16_t *buf = (int16_t*)stream;
    int frames = len / (2 * sizeof(int16_t)); // 2 channels

    int64_t currentFrame = m_iHardwareFrame.load();

    // Mix( buffer, frames, written_frame, current_playing_frame )
    // current_playing_frame is roughly written_frame - buffered_frames.

    int64_t iCurrentFrame = currentFrame - m_iBufferSize;
    if (iCurrentFrame < 0) iCurrentFrame = 0;

    Mix( buf, frames, currentFrame, iCurrentFrame );

    m_iHardwareFrame.fetch_add( frames );
}
