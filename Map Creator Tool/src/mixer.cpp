#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#ifdef __EMSCRIPTEN__
#include "mixer.h"
#else
#include "include/mixer.h"
#endif

void Mixer::init(int channels)
{
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, channels, 2048);
}

Mix_Chunk *Mixer::load(const char *path)
{
    return Mix_LoadWAV(path);
}

void Mixer::play(Mix_Chunk *audio, int channel, int loop)
{
    Mix_PlayChannel(channel, audio, loop);
}

void Mixer::pause(int channel)
{
    Mix_Pause(channel);
}

void Mixer::resume(int channel)
{
    Mix_Resume(channel);
}

void Mixer::halt(int channel)
{
    Mix_HaltChannel(channel);
}

void Mixer::volume(int channel, int vol)
{
    Mix_Volume(channel, vol);
}

bool Mixer::playing(int channel)
{
    return Mix_Playing(channel);
}

bool Mixer::paused(int channel)
{
    return Mix_Paused(channel);
}