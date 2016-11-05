// main.cpp

// OpenAL Includes
#include <AL/al.h>
#include <AL/alc.h>
// SDL2 Includes
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
// STL Includes
#include <iostream>
#include <string>

ALCdevice* g_Device;
ALCcontext* g_Context;

bool Initialize()
{
  int flags = MIX_INIT_OGG;
  if ((Mix_Init(flags) & flags) != flags)
  {
    SDL_Log("Failed to initialize audio: %s", Mix_GetError());
    return false;
  }

  int frequency = 44100;
  int channels = 2;
  int chunksize = 1024;
  if (Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, channels, chunksize) < 0)
  {
    SDL_Log("Failed to open audio: %s", Mix_GetError());
    return false;
  }

  g_Device = alcOpenDevice(nullptr);
  if (g_Device == nullptr)
  {
    SDL_Log("Failed to open device.");
    return false;
  }

  g_Context = alcCreateContext(g_Device, nullptr);
  if (g_Context == nullptr)
  {
    SDL_Log("Failed to create context.");
    return false;
  }

  if (alcMakeContextCurrent(g_Context) != ALC_TRUE)
  {
    SDL_Log("Failed to set current context.");
    return false;
  }

  return true;
}

void Finalize()
{
  alcDestroyContext(g_Context);
  if (alcCloseDevice(g_Device) != ALC_TRUE)
  {
    SDL_Log("Failed to close device.");
  }
  Mix_CloseAudio();
  Mix_Quit();
}

ALuint CreateBuffer(const std::string& p_Filename)
{
  Mix_Chunk* chunk = Mix_LoadWAV(p_Filename.c_str());
  if (chunk == nullptr)
  {
    SDL_Log("Failed to load audio file \"%s\": %s", p_Filename.c_str(), Mix_GetError());
    return -1;
  }

  ALuint id;
  alGenBuffers(1, &id);
  alBufferData(id, AL_FORMAT_MONO16, chunk->abuf, chunk->alen, 44100);

  Mix_FreeChunk(chunk);

  return id;
}

void DestroyBuffer(ALuint p_BufferID)
{
  alDeleteBuffers(1, &p_BufferID);
}

ALuint CreateSource(ALuint p_BufferID)
{
  ALfloat orientation[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };

  ALuint id;
  alGenSources(1, &id);
  alSourcei(id, AL_BUFFER, p_BufferID);
  alSource3f(id, AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(id, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSourcefv(id, AL_ORIENTATION, orientation);
  alSourcef(id, AL_PITCH, 1.0f);
  alSourcef(id, AL_GAIN, 1.0f);
  alSourcei(id, AL_LOOPING, AL_TRUE);

  return id;
}

void PlaySource(ALuint p_SourceID)
{
  alSourcePlay(p_SourceID);
}

void DestroySource(ALuint p_SourceID)
{
  alDeleteSources(1, &p_SourceID);
}

void SetupListener()
{
  ALfloat orientation[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };

  alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
  alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alListenerfv(AL_ORIENTATION, orientation);
}

int main(int p_ArgCount, char** p_ArgVars)
{
  std::cout << "HybridAudio" << std::endl;

  if (!Initialize())
  {
    return -1;
  }

  std::string filename = "resources/explosion.ogg";
  ALuint bufferID = CreateBuffer(filename);
  ALuint sourceID = CreateSource(bufferID);
  SetupListener();

  PlaySource(sourceID);

  SDL_Delay(2500);

  DestroySource(sourceID);
  DestroyBuffer(bufferID);

  Finalize();

  return 0;
}
