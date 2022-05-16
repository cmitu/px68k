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

short	playing = FALSE;

#define PCMBUF_SIZE 2*2*48000
uint8_t pcmbuffer[PCMBUF_SIZE];
uint8_t *pcmbufp = pcmbuffer;
uint8_t *pbsp = pcmbuffer;
uint8_t *pbrp = pcmbuffer, *pbwp = pcmbuffer;
uint8_t *pbep = &pcmbuffer[PCMBUF_SIZE];
uint32_t ratebase = 22050;
int32_t DSound_PreCounter = 0;
uint8_t sdlsndbuf[PCMBUF_SIZE];

int32_t audio_fd = -1;

static void sdlaudio_callback(void *userdata, uint8_t *stream, int32_t len);

#ifndef NOSOUND

#ifndef SDL1
#include	"SDL2/SDL.h"
#include	"SDL2/SDL_audio.h"
#else
#include	"SDL/SDL.h"
#include	"SDL/SDL_audio.h"
#endif

//for SDL2
#if SDL_VERSION_ATLEAST(2, 0, 0)
SDL_AudioDeviceID audio_dev;
SDL_AudioFormat deviceFormat;
#endif


int32_t
DSound_Init(uint32_t rate, uint32_t buflen)
{
	SDL_AudioSpec fmt;
	// Linuxは2倍(SDL1.2)、Android(SDL2.0)は4倍のlenでcallbackされた。
	// この値を小さくした方が音の遅延は少なくなるが負荷があがる
	uint32_t samples = 2048;

	if (playing) {
		return FALSE;
	}

	if (rate == 0) {
		audio_fd = -1;
		return TRUE;
	}

	ratebase = rate;

	memset(&fmt, 0, sizeof(fmt));

	fmt.freq = rate;
	fmt.format = AUDIO_S16SYS;
	fmt.channels = 2;
	fmt.samples = samples;
	fmt.callback = sdlaudio_callback;
	fmt.userdata = NULL;

#if SDL_VERSION_ATLEAST(2, 0, 0)
	audio_dev = SDL_OpenAudioDevice(NULL, 0, &fmt, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if (audio_dev == 0) {
		return FALSE;
	}
	SDL_PauseAudioDevice(audio_dev, 0); 
	deviceFormat=fmt.format; //保存
	audio_fd = 1;
#else
	audio_fd = SDL_OpenAudio(&fmt, NULL);
	if (audio_fd < 0) {
		SDL_Quit();
		return FALSE;
	}
#endif

	playing = TRUE;
	return TRUE;
}

void
DSound_Play(void)
{
	if (audio_fd >= 0){
#if SDL_VERSION_ATLEAST(2, 0, 0)
		SDL_PauseAudioDevice(audio_dev,0);
#else
		SDL_PauseAudio(0);
#endif
	}
}

void
DSound_Stop(void)
{
	if (audio_fd >= 0){
#if SDL_VERSION_ATLEAST(2, 0, 0)
		SDL_PauseAudioDevice(audio_dev,1);
#else
		SDL_PauseAudio(1);
#endif
	}
}

int32_t
DSound_Cleanup(void)
{
	playing = FALSE;

	if (audio_fd >= 0) {
#if SDL_VERSION_ATLEAST(2, 0, 0)
		SDL_CloseAudioDevice(audio_dev);
#else
		SDL_CloseAudio();
		SDL_Quit();
#endif
		audio_fd = -1;
	}
	return TRUE;
}

static void sound_send(int32_t length)
{
	int32_t rate=0;

#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_LockAudioDevice(audio_dev);
#else
	SDL_LockAudio();
#endif

	ADPCM_Update((int16_t *)pbwp, length, rate, pbsp, pbep);
	OPM_Update((int16_t *)pbwp, length, rate, pbsp, pbep);

#ifndef	NO_MERCURY
	//Mcry_Update((short *)pcmbufp, length);
#endif

	pbwp += length * sizeof(uint16_t) * 2;
	if (pbwp >= pbep) {
		pbwp = pbsp + (pbwp - pbep);
	}

#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_UnlockAudioDevice(audio_dev);
#else
	SDL_UnlockAudio();
#endif

}

void FASTCALL DSound_Send0(int32_t clock)
{
	int32_t length = 0;
	int32_t rate;

	if (audio_fd < 0) {
		return;
	}

	DSound_PreCounter += (ratebase * clock);
	while (DSound_PreCounter >= 10000000L) {
		length++;
		DSound_PreCounter -= 10000000L;
	}
	if (length == 0) {
		return;
	}
	sound_send(length);
}

static void FASTCALL DSound_Send(int32_t length)
{
	int32_t rate;

	if (audio_fd < 0) {
		return;
	}
	sound_send(length);
}

static void
sdlaudio_callback(void *userdata, uint8_t *stream, int32_t len)
{
	int32_t lena, lenb, datalen, rate;
	uint8_t *buf;
	static uint32_t bef;
	uint32_t now;

	now = timeGetTime();

	//p6logd("tdiff %4d : len %d ", now - bef, len);

cb_start:
	if (pbrp <= pbwp) {
		// pcmbuffer
		// +---------+-------------+----------+
		// |         |/////////////|          |
		// +---------+-------------+----------+
		// A         A<--datalen-->A          A
		// |         |             |          |
		// pbsp     pbrp          pbwp       pbep

		datalen = pbwp - pbrp;
		if (datalen < len) {
			// needs more data
			DSound_Send((len - datalen) / 4);
		}
#if 0
		datalen = pbwp - pbrp;
		if (datalen < len) {
			printf("xxxxx not enough sound data xxxxx\n");
		}
#endif
		if (pbrp > pbwp) {
			// chage to TYPEC or TYPED
			goto cb_start;
		}

		buf = pbrp;
		pbrp += len;
		//printf("TYPEA: ");

	} else {
		// pcmbuffer
		// +---------+-------------+----------+
		// |/////////|             |//////////|
		// +------+--+-------------+----------+
		// <-lenb->  A             <---lena--->
		// A         |             A          A
		// |         |             |          |
		// pbsp     pbwp          pbrp       pbep

		lena = pbep - pbrp;
		if (lena >= len) {
			buf = pbrp;
			pbrp += len;
			//printf("TYPEC: ");
		} else {
			lenb = len - lena;
			if (pbwp - pbsp < lenb) {
				DSound_Send((lenb - (pbwp - pbsp)) / 4);
			}
#if 0
			if (pbwp - pbsp < lenb) {
				printf("xxxxx not enough sound data xxxxx\n");
			}
#endif
			memcpy(sdlsndbuf, pbrp, lena);
			memcpy(&sdlsndbuf[lena], pbsp, lenb);
			buf = sdlsndbuf;
			pbrp = pbsp + lenb;
			//printf("TYPED: ");
		}
	}

	memset(stream, 0, len); // clear stream buffer for SDL2.(and SDL1)
#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_MixAudioFormat(stream, buf, deviceFormat, len, SDL_MIX_MAXVOLUME);
#else
	SDL_MixAudio(stream, buf, len, SDL_MIX_MAXVOLUME);
#endif

	bef = now;
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

void FASTCALL
DSound_Send0(long clock)
{
}
#endif	/* !NOSOUND */
