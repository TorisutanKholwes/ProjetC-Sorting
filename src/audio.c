/*
 * Copyright (c) 2025 Torisutan
 * All rights reserved
 */
#include "audio.h"

#include "logger.h"
#include "utils.h"

Mix_Chunk* Audio_createNoise(int frequency, int duration_ms) {
    constexpr int sample_rate = 44100;
    int samples = (duration_ms * sample_rate) / 1000;

    int16_t* buffer = malloc(samples * sizeof(int16_t));
    if (!buffer) {
        error("Failed to allocate memory for audio buffer");
        return NULL;
    }

    for (int i = 0; i < samples; i++) {
        constexpr int16_t amplitude = 28000;
        double time = (double)i / sample_rate;
        buffer[i] = (int16_t)(amplitude * sin(2.0 * M_PI * frequency * time));
    }

    Mix_Chunk* chunk = Mix_QuickLoad_RAW((Uint8*)buffer, samples * sizeof(int16_t));
    if (!chunk) {
        error("Failed to create Mix_Chunk: %s", Mix_GetError());
        safe_free((void**)&buffer);
        return NULL;
    }

    safe_free((void**)&buffer);
    return chunk;
}

void Audio_play(Mix_Chunk* chunk, int volume, bool loop) {
    Mix_VolumeChunk(chunk, volume);
    int loops = loop ? -1 : 0;
    if (Mix_PlayChannel(-1, chunk, loops) == -1) {
        error("Failed to play audio chunk: %s", Mix_GetError());
        Mix_FreeChunk(chunk);
        return;
    }

    SDL_Delay(chunk->alen / (44100 * sizeof(int16_t)) * 1000 + 50);
    Mix_FreeChunk(chunk);
}

void Audio_playSweepingNoise(int start_freq, int end_freq, int duration_ms) {
    const int sample_rate = 44100;
    const int16_t amplitude = 28000;
    int64_t samples = (int64_t)duration_ms * sample_rate / 1000;
    if (samples <= 0) return;

    int16_t *buffer = malloc(samples * sizeof(int16_t));
    if (!buffer) return;

    double phase = 0.0;
    const double two_pi = 2.0 * M_PI;
    double fs = (double)sample_rate;

    for (int64_t i = 0; i < samples; ++i) {
        double t = (double)i / (double)(samples - 1); /* 0..1 */
        double fc = (double)start_freq + (double)(end_freq - start_freq) * t;
        if (fc < 1.0) fc = 1.0;

        /* intégrer la phase : phase += 2*pi*fc/fs */
        phase += two_pi * fc / fs;
        if (phase > two_pi) phase = fmod(phase, two_pi);

        double sample = sin(phase) * amplitude;
        if (sample > INT16_MAX) sample = INT16_MAX;
        if (sample < INT16_MIN) sample = INT16_MIN;
        buffer[i] = (int16_t)sample;
    }

    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = sample_rate;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 4096;

    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (dev == 0) {
        free(buffer);
        return;
    }

    SDL_ClearQueuedAudio(dev);
    SDL_QueueAudio(dev, buffer, (uint32_t)(samples * sizeof(int16_t)));
    SDL_PauseAudioDevice(dev, 0);

    SDL_Delay(duration_ms + 100);

    SDL_CloseAudioDevice(dev);
    free(buffer);
}
