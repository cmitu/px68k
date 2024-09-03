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
#include	"fmg_wrap.h"

uint32_t   ratebase = 44100;
uint16_t   playing  = FALSE;
int32_t    audio_fd = -1;

#ifndef NOSOUND

#include	"SDL3/SDL.h"
#include	"SDL3/SDL_audio.h"

//for SDL3
SDL_AudioDeviceID audio_dev;
SDL_AudioSpec fmt_pc; //PC出力
SDL_AudioSpec fmt_x68; //X68出力
SDL_AudioStream *stream;

static void sdlaudio_callback(void *userdata, SDL_AudioStream *stream, int len , int total);

int32_t
DSound_Init(uint32_t rate)
{

	if (playing) {
		return FALSE;
	}

	if (rate == 0) {
		audio_fd = -1;
		return TRUE;
	}

	ratebase = rate;

	memset(&fmt_pc, 0, sizeof(fmt_pc));

	fmt_pc.freq = rate;
	fmt_pc.channels = 2;
	fmt_pc.format = SDL_AUDIO_S16LE;
	audio_dev = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &fmt_pc);
	if (audio_dev == 0) {
	    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_OpenAudioDevice() failed: %s\n", SDL_GetError());
	    return FALSE;
	}

	memset(&fmt_x68, 0, sizeof(fmt_x68));

	fmt_x68.freq = rate;
	fmt_x68.channels = 2;
	fmt_x68.format = SDL_AUDIO_S16LE;
	stream = SDL_CreateAudioStream(&fmt_x68, &fmt_pc);
	if (stream == NULL) {
	    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateAudioStream() failed: %s\n", SDL_GetError());
	    SDL_CloseAudioDevice(audio_dev);
	    return FALSE;
	}
	SDL_BindAudioStream(audio_dev,stream);

	SDL_SetAudioStreamGetCallback(stream, sdlaudio_callback, NULL);

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
		SDL_CloseAudioDevice(audio_dev);//SDL2/3
		SDL_DestroyAudioStream(stream);//SDL3
		audio_fd = -1;
	}
	return TRUE;
}

static void
sdlaudio_callback(void *userdata, SDL_AudioStream *stream, int len , int total)
{
	int16_t pcmbuffer[len + 2];

	//波形生成
	ADPCM_Update((int16_t *)pcmbuffer, len /4, pcmbuffer, &pcmbuffer[len]);
	OPM_Update  ((int16_t *)pcmbuffer, len /4, pcmbuffer, &pcmbuffer[len]);
#ifndef	NO_MERCURY
	Mcry_Update((int16_t *)pcmbuffer, len / 4);
#endif

	//Streaming!
	SDL_PutAudioStreamData(stream, pcmbuffer, len );
	SDL_FlushAudioStream(stream);

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
