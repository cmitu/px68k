/* 
 * Copyright (c) 2003 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include	"windows.h"
#include	"common.h"
#include	"dswin.h"
#include	"prop.h"
#include	"adpcm.h"
#include	"mercury.h"
#include	"ymfm_wrap.h"

uint16_t   playing  = FALSE;
int32_t    audio_fd = -1;

#ifndef NOSOUND

#include	"SDL3/SDL.h"
#include	"SDL3/SDL_audio.h"

//for SDL3
SDL_AudioSpec fmt_pc; //PC出力
SDL_AudioDeviceID audio_dev16;
SDL_AudioDeviceID audio_dev32;
SDL_AudioSpec fmt_x68_16; //X68 16bitAudio出力
SDL_AudioSpec fmt_x68_32; //X68 32bitAudio出力
SDL_AudioStream *stream16;
SDL_AudioStream *stream32;

static void sdlaudio_callback16(void *userdata, SDL_AudioStream *stream16, int len , int total);
static void sdlaudio_callback32(void *userdata, SDL_AudioStream *stream32, int len , int total);

int32_t
DSound_Init(uint32_t rate)
{

	if (playing) {
		return FALSE;
	}

	if (rate == 0) {//44100 or 22050 or 11025
		audio_fd = -1;
		return TRUE;
	}

	//PC 出力
	memset(&fmt_pc, 0, sizeof(fmt_pc));

	fmt_pc.freq = rate;
	fmt_pc.channels = 2;
	fmt_pc.format = SDL_AUDIO_S32LE;
	audio_dev32 = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &fmt_pc);
	if (audio_dev32 == 0) {
	    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_OpenAudioDevice() failed: %s\n", SDL_GetError());
	    return FALSE;
	}

	//ADPCM 出力(16bitPCM)
	memset(&fmt_x68_16, 0, sizeof(fmt_x68_16));

	fmt_x68_16.freq = rate;
	fmt_x68_16.channels = 2;
	fmt_x68_16.format = SDL_AUDIO_S16LE;
	stream16 = SDL_CreateAudioStream(&fmt_x68_16, &fmt_pc);
	if (stream16 == NULL) {
	    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateAudioStream() failed: %s\n", SDL_GetError());
	    SDL_CloseAudioDevice(audio_dev32);
	    return FALSE;
	}

	//FM 出力(32bitPCM)
	memset(&fmt_x68_32, 0, sizeof(fmt_x68_32));

	fmt_x68_32.freq = rate;
	fmt_x68_32.channels = 2;
	fmt_x68_32.format = SDL_AUDIO_S32LE;
	stream32 = SDL_CreateAudioStream(&fmt_x68_32, &fmt_pc);
	if (stream32 == NULL) {
	    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateAudioStream() failed: %s\n", SDL_GetError());
	    SDL_CloseAudioDevice(audio_dev32);
	    return FALSE;
	}

	//Streaming
	SDL_BindAudioStream(audio_dev32,stream32);
	SDL_SetAudioStreamGetCallback(stream32, sdlaudio_callback32, NULL);

	SDL_BindAudioStream(audio_dev32,stream16);
	SDL_SetAudioStreamGetCallback(stream16, sdlaudio_callback16, NULL);

	audio_fd = 1; //flag
	playing = TRUE;

	return TRUE;
}

void
DSound_Play(void)
{
	if (audio_fd >= 0){
		ADPCM_SetVolume((uint8_t)Config.PCM_VOL);
		OPM_SetVolume((uint8_t)Config.OPM_VOL);
#ifndef	NO_MERCURY
		Mcry_SetVolume((uint8_t)Config.MCR_VOL);
#endif
	}
}

void
DSound_Stop(void)
{
	if (audio_fd >= 0){
		ADPCM_SetVolume(0);
		OPM_SetVolume(0);
#ifndef	NO_MERCURY
		Mcry_SetVolume(0);
#endif
	}
}

int32_t
DSound_Cleanup(void)
{
	playing = FALSE;

	if (audio_fd >= 0) {
		SDL_CloseAudioDevice(audio_dev32);
		SDL_DestroyAudioStream(stream16);
		SDL_DestroyAudioStream(stream32);
		audio_fd = -1;
	}
	return TRUE;
}

static void
sdlaudio_callback16(void *userdata, SDL_AudioStream *stream16, int len , int total)
{
	int16_t pcmbuffer[len + 2];

	//波形生成
	ADPCM_Update((int16_t *)pcmbuffer, len / 4, pcmbuffer, &pcmbuffer[len]);
#ifndef	NO_MERCURY
	Mcry_Update((int16_t *)pcmbuffer, len / 4);
#endif

	//Streaming!
	SDL_PutAudioStreamData(stream16, pcmbuffer, len );
	SDL_FlushAudioStream(stream16);

 return;
}

static void
sdlaudio_callback32(void *userdata, SDL_AudioStream *stream32, int len , int total)
{
	int32_t pcmbuffer[len + 2];
	//波形生成
	OPM_Update  ((int32_t *)pcmbuffer, len / 4 );

	//Streaming!
	SDL_PutAudioStreamData(stream32, pcmbuffer, len );
	SDL_FlushAudioStream(stream32);

 return;
}

#else	/* NOSOUND */
int32_t
DSound_Init(uint32_t rate, uint32_t buflen)
{
	return FALSE;
}

void
DSound_Play(void)
{
}

void
DSound_Stop(void)
{
}

int32_t
DSound_Cleanup(void)
{
	return TRUE;
}
#endif	/* !NOSOUND */
