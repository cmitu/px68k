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

	// Create MIDIPacketList from packetbuff
	const uint8_t packetBuf[MIDBUF_SIZE];
	MIDIPacketList *packetList = (MIDIPacketList *)packetBuf;

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

	// Create Client for MIDI
	err_sts = MIDIClientCreate(CFSTR("px68k"), NULL, NULL, &mid_client);

	if (err_sts != noErr)
	{
		p6logd("MIDI:CoreMIDI: No client created.");
		return !MMSYSERR_NOERROR;
	}

	// Create Source for client
	//err_sts = MIDISourceCreate(mid_client, CFSTR("px68k MIDI Source"),
	//			&mid_endpoint);

	//if (err_sts != noErr)
	//{
	//	p6logd("MIDI:CoreMIDI: No Sorce created.");
	//	return !MMSYSERR_NOERROR;
	//}

	// Create OutPort for client
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
		strcpy(menu_items[8][0],"No Device found.");
		strcpy(menu_items[8][1],"\0"); // Menu END 
		return !MMSYSERR_NOERROR; // No found Error
	}

	// Store MIDI out port LIST
	CFStringRef strRef;
	char mididevicename[64];
	for(uint32_t i = 0; i<destId; i++){
		mid_endpoint = MIDIGetDestination(i);
		MIDIObjectGetStringProperty(mid_endpoint, kMIDIPropertyName, &strRef);
		if(i<8){// MAX item check
			CFStringGetCString(strRef, menu_items[8][i], sizeof(menu_items[8][i]), 0);
			p6logd("Find MIDI:%s\n",menu_items[8][i]);
		}
	}
	CFRelease(strRef);
	strcpy(menu_items[8][destId],"\0"); // Menu END 

	// Set MIDI outport (default=Port0)
	midOutChg((uint32_t)0);

	*phmo = (HANDLE)mid_name; //MIDI Active!(ダミーを代入しておく)

	return MMSYSERR_NOERROR;

}

// -----------------------------------------------------------------------
//   set/change MIDI Port 
// -----------------------------------------------------------------------
void midOutChg(uint32_t port_no)
{
	OSStatus err_sts;

	mid_endpoint = MIDIGetDestination(port_no);
	if (err_sts != noErr){
		mid_endpoint = MIDIGetDestination((uint32_t)0);// エラーだったらとりあえずPort0に再セットしておく
	}

}

// -----------------------------------------------------------------------
//   MIDI Port close
// -----------------------------------------------------------------------
uint32_t
midiOutClose(HMIDIOUT hmo)
{
	(void)hmo;

	OSStatus err_sts;

	// Disconnect IN/OUT Port
	//err_sts = MIDIPortDisconnectSource(mid_port, mid_endpoint);
	//if (err_sts != noErr) p6logd("Disconnect MIDI-Source err\n");

	// Dispose Port
	err_sts = MIDIPortDispose(mid_port);
	if (err_sts != noErr) p6logd("Dispose MIDI-Port err\n");

	// Dispose endpoint
	err_sts = MIDIEndpointDispose(mid_endpoint);
	if (err_sts != noErr) p6logd("Dispose MIDI-Endpoint err\n");

	// Dispose Client
	err_sts = MIDIClientDispose(mid_client);
	if (err_sts != noErr) p6logd("Dispose MIDI-Client err\n");

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

	// initialize MIDIPacketList
	mid_Packet = MIDIPacketListInit(packetList);

	// length of msg
	uint32_t len;
	switch(messg[0] & 0xf0){
		case 0xc0://prog. chg
		case 0xd0://chnnel press.
			len = 2;
			break;
		case 0x80://note on
		case 0x90://     off
		case 0xa0://key press.
		case 0xb0://cont.chg
		case 0xe0://pitch wheel chg
			len = 3;
			break;
		case 0xf0:
			len = 1; //ないと思うけど
			break;
		default:
			len = 0; //ありえないハズ
			break;
	}

	// 電文とタイムスタンプ入れてPacketList更新
	MIDITimeStamp midiTime = mach_absolute_time();
	MIDIPacketListAdd(packetList, (ByteCount)sizeof(packetBuf), mid_Packet, (MIDITimeStamp)midiTime, len, messg);

	// PacketList送信
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

	// initialize MIDIPacketList
	mid_Packet = MIDIPacketListInit(packetList);

	// 電文とタイムスタンプ入れてPacketList更新
	MIDITimeStamp midiTime = mach_absolute_time();
	MIDIPacketListAdd(packetList, (ByteCount)sizeof(packetBuf), mid_Packet, 
				(MIDITimeStamp)midiTime, pmh->dwBufferLength, (uint8_t *)pmh->lpData);

	// PacketList送信
	MIDISend(mid_port,mid_endpoint,packetList);

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

