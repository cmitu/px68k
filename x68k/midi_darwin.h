

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



	MIDIPortRef m_port;
	MIDIClientRef m_client;
	MIDIEndpointRef m_endpoint;
	MIDIPacket* m_pCurPacket = 0;

	uint8_t MIDI_EXCVWAIT;

//MidiHandler_coremidi(){ m_pCurPacket = 0;}

	const char mid_name[] = {"px68k-MIDI"};

	//const char * GetName(void) { return "coremidi"; }


// -----------------------------------------------------------------------
//   MIDI Port open (Get endpoint Create client , port)
//   bool MIDI_Open(const char * conf)
// -----------------------------------------------------------------------
uint32_t
midiOutOpen(LPHMIDIOUT phmo, uint32_t uDeviceID, uint32_t dwCallback,
    uint32_t dwInstance, uint32_t fdwOpen)
{
	//phmo; 
	(void)dwCallback;
	(void)dwInstance;
	(void)fdwOpen;

		OSStatus err_sts;

		// Create a MIDI client
		err_sts = MIDIClientCreate(CFSTR("px68k"), NULL, NULL, &m_client);

		if (err_sts != noErr)
		{
			printf("MIDI:coremidi: No client created.");
			return !MMSYSERR_NOERROR;
		}

		// Create a MIDI Source
		err_sts = MIDISourceCreate(m_client, CFSTR("px68k MIDI Source"),
                     &m_endpoint);

		if (err_sts != noErr)
		{
			printf("MIDI:coremidi: No Sorce created.");
			return !MMSYSERR_NOERROR;
		}

		// Create a MIDI  port
		err_sts = MIDIOutputPortCreate(m_client, CFSTR("px68k MIDI Port"), &m_port);

		if (err_sts != noErr)
		{
			printf("MIDI:coremidi: No port created.");
			return !MMSYSERR_NOERROR;
		}

		// Get the MIDIEndPoint
		m_endpoint = 0;
		//uint32_t destId = MIDIGetNumberOfDevices();//仮想ポート含まない
		//m_endpoint = MIDIGetDevice(destId);
	    uint32_t destId = MIDIGetNumberOfDestinations();//仮想ポート含む
		if (destId == 0)
		{
			printf("No MIDI-port.\n");
			return !MMSYSERR_NOERROR;
		}
		destId = 0;// 最初のMIDI port固定
		m_endpoint = MIDIGetDestination(destId);
		CFStringRef strRef;
		char mididevicename[64];
		MIDIObjectGetStringProperty(m_endpoint, kMIDIPropertyName, &strRef);
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
	MIDIPortDispose(m_port);

	// Dispose the client
	MIDIClientDispose(m_client);

	// Dispose the endpoint
	MIDIEndpointDispose(m_endpoint);

	return MMSYSERR_NOERROR;
}

// -----------------------------------------------------------------------
//   Send Short Message (演奏データ送信)
// -----------------------------------------------------------------------
uint32_t
midiOutShortMsg(HMIDIOUT hmo, uint32_t msg)
{
	uint8_t messg[4];

uint8_t MIDI_evt_len[256] = {
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x00
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x10
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x20
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x30
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x40
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x50
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x60
  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  // 0x70

  3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3,  // 0x80
  3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3,  // 0x90
  3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3,  // 0xa0
  3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3,  // 0xb0

  2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,2,  // 0xc0
  2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,2,  // 0xd0

  3,3,3,3, 3,3,3,3, 3,3,3,3, 3,3,3,3,  // 0xe0

  0,2,3,2, 0,0,1,0, 1,0,1,1, 1,0,1,0   // 0xf0
};

	//#define MIDIOUTS(a,b,c) (((uint32_t)c << 16) | ((uint32_t)b << 8) | (uint32_t)a) の逆
	messg[0] =   msg       & 0xff;
	messg[1] =  (msg >> 8) & 0xff;
	messg[2] =  (msg >> 16) & 0xff;
	messg[3] =  (msg >> 24) & 0xff;

	// Acquire a MIDIPacketList
	uint8_t packetBuf[MIDBUF_SIZE];
	MIDIPacketList *packetList = (MIDIPacketList *)packetBuf;
	m_pCurPacket = MIDIPacketListInit(packetList);

	// Determine the length of msg
	uint32_t len=MIDI_evt_len[messg[0]];
	



	// Add msg to the MIDIPacketList
	MIDITimeStamp midiTime = mach_absolute_time();
	MIDIPacketListAdd(packetList, (ByteCount)sizeof(packetBuf), m_pCurPacket, (MIDITimeStamp)midiTime, len, messg);

	// Send the MIDIPacketList
	MIDISend(m_port,m_endpoint,packetList);

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
	m_pCurPacket = MIDIPacketListInit(packetList);

	// Add msg to the MIDIPacketList
	MIDITimeStamp midiTime = mach_absolute_time();
	MIDIPacketListAdd(packetList, (ByteCount)sizeof(packetBuf), m_pCurPacket, (MIDITimeStamp)midiTime, pmh->dwBufferLength, (uint8_t *)pmh->lpData);

		
	// Send the MIDIPacketList
	MIDISend(m_port,m_endpoint,packetList);

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

