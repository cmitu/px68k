// macOS用CoreMIDI 対応の関数群
// Win.のMIDI関数とコンパチ(winmm.lib)で置き換えてます。
// LIST表示して選択するまでもないのでPort0決め打ち出力です。
// 
// 2022/3/27  by kameya

#import <CoreMIDI/MIDIServices.h>
#import <CoreMIDI/CoreMIDI.h>
#import <mach/mach_time.h>

typedef struct midihdr {
	char*			lpData;
	uint32_t			dwBufferLength;
	uint32_t			dwBytesRecorded;
	uint32_t			dwUser;
	uint32_t			dwFlags;
	struct midihdr *	lpNext;
	uint32_t			reserved;
	uint32_t			dwOffset;
	uint32_t			dwReserved[8];
} MIDIHDR, *PMIDIHDR, *NPMIDIHDR, *LPMIDIHDR;

typedef	HANDLE		HMIDIOUT;
typedef	HMIDIOUT *	LPHMIDIOUT;

#define MIDBUF_SIZE 200
#define	MMSYSERR_NOERROR	0
#define	MIDIERR_STILLPLAYING	2
#define	MIDI_MAPPER		-1
#define	CALLBACK_NULL		0x00000000L

// for CoreMIDI 
	MIDIPortRef mid_port;
	MIDIClientRef mid_client;
	MIDIEndpointRef mid_endpoint;
	MIDIPacket* mid_Packet = 0;


	uint8_t MIDI_EXCVWAIT;
	const char mid_name[] = {"px68k-MIDI"};


// -----------------------------------------------------------------------
//   MIDI port Open (Client -> Source -> Port ,EndPoint)
//   select Port0 for output MIDI.
// -----------------------------------------------------------------------
uint32_t
midiOutOpen(LPHMIDIOUT phmo, uint32_t uDeviceID, uint32_t dwCallback,
    uint32_t dwInstance, uint32_t fdwOpen)
{
	(void)dwCallback;
	(void)dwInstance;
	(void)fdwOpen;

		OSStatus err_sts;

		// Create a MIDI client
		err_sts = MIDIClientCreate(CFSTR("px68k"), NULL, NULL, &mid_client);

		if (err_sts != noErr)
		{
			p6logd("MIDI:CoreMIDI: No client created.");
			return !MMSYSERR_NOERROR;
		}

		// Create a MIDI Source
		err_sts = MIDISourceCreate(mid_client, CFSTR("px68k MIDI Source"),
					&mid_endpoint);

		if (err_sts != noErr)
		{
			p6logd("MIDI:CoreMIDI: No Sorce created.");
			return !MMSYSERR_NOERROR;
		}

		// Create a MIDI  port
		err_sts = MIDIOutputPortCreate(mid_client, CFSTR("px68k MIDI Port"), &mid_port);

		if (err_sts != noErr)
		{
			p6logd("MIDI:CoreMIDI: No port created.");
			return !MMSYSERR_NOERROR;
		}

		// Get the MIDIEndPoint
		mid_endpoint = 0;
		//uint32_t destId = MIDIGetNumberOfDevices();//仮想ポート含まない
		//mid_endpoint = MIDIGetDevice(destId);
	    uint32_t destId = MIDIGetNumberOfDestinations();//仮想ポート含む
		if (destId == 0)
		{
			p6logd("No MIDI-port found.\n");
			return !MMSYSERR_NOERROR;
		}
		destId = 0;// 最初のMIDI port固定
		mid_endpoint = MIDIGetDestination(destId);
		CFStringRef strRef;
		char mididevicename[64];
		MIDIObjectGetStringProperty(mid_endpoint, kMIDIPropertyName, &strRef);
		CFStringGetCString(strRef, mididevicename, sizeof(mididevicename), 0);
		CFRelease(strRef);
		printf("Find MIDI:%s\n",mididevicename);

	*phmo = (HANDLE)mid_name; //MIDI Active!(ダミーを代入しておく)

	return MMSYSERR_NOERROR;

}

// -----------------------------------------------------------------------
//   MIDI Port close
// -----------------------------------------------------------------------
uint32_t
midiOutClose(HMIDIOUT hmo)
{
	(void)hmo;

	// Dispose the port
	MIDIPortDispose(mid_port);

	// Dispose the client
	MIDIClientDispose(mid_client);

	// Dispose the endpoint
	MIDIEndpointDispose(mid_endpoint);

	return MMSYSERR_NOERROR;
}

// -----------------------------------------------------------------------
//   Send Short Message (演奏データ送信)
// -----------------------------------------------------------------------
uint32_t
midiOutShortMsg(HMIDIOUT hmo, uint32_t msg)
{
	uint8_t messg[4];

	// (uint32)msg を 4byte に分解
	messg[0] =   msg       & 0xff;
	messg[1] =  (msg >> 8) & 0xff;
	messg[2] =  (msg >> 16) & 0xff;
	messg[3] =  (msg >> 24) & 0xff;

	// Acquire a MIDIPacketList
	uint8_t packetBuf[MIDBUF_SIZE];
	MIDIPacketList *packetList = (MIDIPacketList *)packetBuf;
	mid_Packet = MIDIPacketListInit(packetList);

	// length of msg
	uint32_t len;
	switch(messg[0] & 0xf0){
		case 0xc0:
		case 0xd0:
			len = 2;
			break;
		case 0x80:
		case 0x90:
		case 0xa0:
		case 0xb0:
		case 0xe0:
			len = 3;
			break;
		case 0xf0:
			len = 1; //ないと思うけど
			break;
		default:
			len = 0; //ありえないハズ
	}

	// Add msg to the MIDIPacketList
	MIDITimeStamp midiTime = mach_absolute_time();
	MIDIPacketListAdd(packetList, (ByteCount)sizeof(packetBuf), mid_Packet, (MIDITimeStamp)midiTime, len, messg);

	// Send the MIDIPacketList
	MIDISend(mid_port,mid_endpoint,packetList);

	return MMSYSERR_NOERROR;
}

// -----------------------------------------------------------------------
//   Exclusive GO!  (設定データ送信)
// -----------------------------------------------------------------------
uint32_t
midiOutLongMsg(HMIDIOUT hmo, LPMIDIHDR pmh, uint32_t cbmh)
{
	(void)hmo;
	(void)cbmh;

	// Acquire a MIDIPacketList
	uint8_t packetBuf[MIDBUF_SIZE];
	uint32_t pos=0;
	MIDIPacketList *packetList = (MIDIPacketList *)packetBuf;
	mid_Packet = MIDIPacketListInit(packetList);

	// Add msg to the MIDIPacketList
	MIDITimeStamp midiTime = mach_absolute_time();
	MIDIPacketListAdd(packetList, (ByteCount)sizeof(packetBuf), mid_Packet, 
				(MIDITimeStamp)midiTime, pmh->dwBufferLength, (uint8_t *)pmh->lpData);

	// Send the MIDIPacketList
	MIDISend(mid_port,mid_endpoint,packetList);

	MIDI_EXCVWAIT = 1;

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
midiOutReset(HMIDIOUT hmo)
{
	(void)hmo;
	return MMSYSERR_NOERROR;
}

