#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

class Mixer
{
public:
    void init(int channels);
    Mix_Chunk *load(const char *path);
    void play(Mix_Chunk *audio, int channel, int loop);
    void pause(int channel);
    void resume(int channel);
    void halt(int channel);
    void volume(int channel, int vol);
    bool playing(int channel);
    bool paused(int channel);
};