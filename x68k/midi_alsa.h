#include "../win32api/mmsystem.h"

uint32_t
midiOutShortMsg(HMIDIOUT hmo, uint32_t dwMsg)
{
	(void)hmo;
	(void)dwMsg;
	return MMSYSERR_NOERROR;
}

uint32_t
midiOutLongMsg(HMIDIOUT hmo, LPMIDIHDR pmh, uint32_t cbmh)
{
	(void)hmo;
	(void)pmh;
	(void)cbmh;
	return MMSYSERR_NOERROR;
}

uint32_t
midiOutUnprepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, uint32_t cbmh)
{
	(void)hmo;
	(void)pmh;
	(void)cbmh;
	return MMSYSERR_NOERROR;
}

uint32_t
midiOutPrepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, uint32_t cbmh)
{
	(void)hmo;
	(void)pmh;
	(void)cbmh;
	return MMSYSERR_NOERROR;
}

uint32_t
midiOutOpen(LPHMIDIOUT phmo, uint32_t uDeviceID, uint32_t dwCallback,
    uint32_t dwInstance, uint32_t fdwOpen)
{
	(void)phmo;
	(void)uDeviceID;
	(void)dwCallback;
	(void)dwInstance;
	(void)fdwOpen;
	return !MMSYSERR_NOERROR;	// (ぃ
}

uint32_t
midiOutClose(HMIDIOUT hmo)
{
	(void)hmo;
	return MMSYSERR_NOERROR;
}

uint32_t
midiOutReset(HMIDIOUT hmo)
{
	(void)hmo;
	return MMSYSERR_NOERROR;
}

