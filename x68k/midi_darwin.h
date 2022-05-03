// macOS用CoreMIDI 対応の関数群
// Win.のMIDI関数とコンパチ(winmm.lib)で置き換えてます。
// CoreAudioのシンセサイザをdefault、CoreMIDIで検出された機器を
// リストします。 SoundFontにも対応します。
// MIMPIの音色変更FileはConfigで指定してください。
// 
// 2022/3/27  by kameya

#import <CoreMIDI/MIDIServices.h>
#import <CoreMIDI/CoreMIDI.h>
#import <AudioToolbox/AUGraph.h>
#import <CoreServices/CoreServices.h>
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

	// for CoreAudio 
	AUGraph mid_auGraph;
	AudioUnit mid_synth;
	const char *soundfont;

	// Play CoreAudio:0 or CoreMIDI:1
	uint32_t set_midout = 0;

	// Create MIDIPacketList from packetbuff
	const uint8_t packetBuf[MIDBUF_SIZE];
	MIDIPacketList *packetList = (MIDIPacketList *)packetBuf;


	const char mid_name[] = {"px68k-MIDI"};
	const char synth_name[] = {"DLSSynth(CoreAudio)"};

	// SoundFont URL (MT-32:bank 0)
	const char sf_conf[] = {"Phoenix_MT-32.sf2"};

// ------------------------------------------
//   Load SoundFont file
// ------------------------------------------
uint32_t
load_soundfont(char *sf_name)
{
	OSStatus err_sts;

	if (sf_name[0]) {

			//Sound Font URL
			soundfont = file_getcd(sf_name);

			// SoundFont allocate
			CFURLRef url = CFURLCreateFromFileSystemRepresentation(
								kCFAllocatorDefault,
								(const UInt8 *)soundfont,
								strlen(soundfont), false  );
			if (!url) {
				p6logd("CoreAudio:Failed to allocate CFURLRef from  %s\n",soundfont);
				return !MMSYSERR_NOERROR;
			}

			// SoundFont load
			err_sts = AudioUnitSetProperty(
							mid_synth, kMusicDeviceProperty_SoundBankURL,
							kAudioUnitScope_Global, 0, &url, sizeof(url) );
			CFRelease(url);
			if (err_sts != noErr){
				p6logd("CoreAudio:Error loading CoreAudio SoundFont %s\n",soundfont);
				return !MMSYSERR_NOERROR;
			}

	p6logd("CoreAudio:loading SoundFont %s OK\n",soundfont);
	return MMSYSERR_NOERROR;

	}

return !MMSYSERR_NOERROR;
}

// ---------------------------------------------
//   MIDI port Open (CoreAudio synth)
// ---------------------------------------------
uint32_t
mid_synth_open()
{
	AUNode outputNode;
	AUNode synthNode;
	AudioComponentDescription desc;

	OSStatus err_sts;

	if (mid_auGraph)
		return !MMSYSERR_NOERROR;

	// Open the Music Device.
	err_sts = NewAUGraph(&mid_auGraph);
	err_sts = AUGraphOpen(mid_auGraph);
	if (err_sts != noErr){
		p6logd("CoreAudio:Can't Open AUGraph\n");
		return !MMSYSERR_NOERROR;
	}

	// The default output AudioUnit
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_DefaultOutput;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
	AUGraphAddNode(mid_auGraph, &desc, &outputNode);

	// The built-in default (softsynth) AudioUnit
	desc.componentType = kAudioUnitType_MusicDevice;
	desc.componentSubType = kAudioUnitSubType_DLSSynth;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	AUGraphAddNode(mid_auGraph, &desc, &synthNode);

	// Connect the softsynth to the default output
	AUGraphConnectNodeInput(mid_auGraph, synthNode, 0, outputNode, 0);

	// Open and initialize the graph
	err_sts = AUGraphOpen(mid_auGraph);
	err_sts = AUGraphInitialize(mid_auGraph);
	if (err_sts != noErr){
		p6logd("CoreAudio:AUGraph Open/Init error\n");
		return !MMSYSERR_NOERROR;
	}

	// Get the music device from the graph.
	AUGraphNodeInfo(mid_auGraph, synthNode, NULL, &mid_synth);

	// load a soundfont (if needed)
	load_soundfont((char *)sf_conf);

	// Start the graph
	err_sts = AUGraphStart(mid_auGraph);
	if (err_sts != noErr){
		p6logd("CoreAudio:AUGraph can't start\n");
		return !MMSYSERR_NOERROR;
	}

return MMSYSERR_NOERROR;
}

// -----------------------------------------------------------------------
//   MIDI port Open (Client -> Source -> Port ,EndPoint)
//   CoreAudio and CoreMIDI
//   default select Port0 for output MIDI.
// -----------------------------------------------------------------------
uint32_t
midiOutOpen(LPHMIDIOUT phmo, uint32_t uDeviceID, uint32_t dwCallback,
    uint32_t dwInstance, uint32_t fdwOpen)
{
	(void)dwCallback;
	(void)dwInstance;
	(void)fdwOpen;

	uint32_t Device_num = 0;
	OSStatus err_sts;

	//CoreAudio Open synth OK
	if (mid_synth_open() == MMSYSERR_NOERROR){
		strcpy(menu_items[8][Device_num],synth_name);
		Device_num ++;
		strcpy(menu_items[8][Device_num],"\0"); // Menu END 
	}

	// Create Client for coreMIDI
	err_sts = MIDIClientCreate(CFSTR("px68k"), NULL, NULL, &mid_client);

	if (err_sts != noErr)
	{
		p6logd("MIDI:CoreMIDI: No client created.");
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
	}

	// Get the MIDIEndPoint
	mid_endpoint = 0;
	//uint32_t core_mid_num = MIDIGetNumberOfDevices();//仮想ポート含まない
	//mid_endpoint = MIDIGetDevice(core_mid_num);
	uint32_t core_mid_num = MIDIGetNumberOfDestinations();//仮想ポート含む
	if (core_mid_num == 0)
	{
		if(Device_num == 0){
		  p6logd("No MIDI-port found.\n");
		  strcpy(menu_items[8][Device_num],"No Device found.");
		  Device_num ++;
		  strcpy(menu_items[8][Device_num],"\0"); // Menu END 
		  return !MMSYSERR_NOERROR; // No found Error
		}
	}

	// Store MIDI out port LIST
	CFStringRef strRef;
	char mididevicename[64];
	 for(uint32_t i = 0; i<core_mid_num; i++){
		mid_endpoint = MIDIGetDestination(i);
		MIDIObjectGetStringProperty(mid_endpoint, kMIDIPropertyName, &strRef);
		if(i<8){// MAX item check
			CFStringGetCString(strRef, menu_items[8][Device_num], sizeof(menu_items[8][Device_num]), 0);
			p6logd("Find MIDI:%s\n",menu_items[8][Device_num]);
			Device_num ++;
		}
	 }
	//CFRelease(strRef);
	strcpy(menu_items[8][Device_num],"\0"); // Menu END 


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
	uint32_t coremid_port = port_no;

	if(strcmp(menu_items[8][0],synth_name) == 0){// CoreAudio Synth chk
		if(port_no == 0){
			set_midout=0; // CoreAudio select
		}
		else{
			set_midout=1; // CoreMIDI select
			coremid_port--;
		}
	}
	else{
		set_midout=1; // CoreMIDI select
	}

	// CoreMIDI endpoint change
	mid_endpoint = MIDIGetDestination(coremid_port);
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

	// CoreAudio close
	if (mid_auGraph) {
		AUGraphStop(mid_auGraph);
		DisposeAUGraph(mid_auGraph);
		mid_auGraph = 0;
	}

	// CoreMIDI close
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

	// Send the MIDI-Packet(CoreAudio)
	if(set_midout == 0){
		MusicDeviceMIDIEvent(mid_synth, messg[0], messg[1], messg[2], 0);
		return MMSYSERR_NOERROR;
	}

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

	// PacketList送信(CoreMIDI)
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

	// Send the MIDI-Packet(CoreAudio)
	if(set_midout == 0){
		MusicDeviceSysEx(mid_synth, (uint8_t *)pmh->lpData, pmh->dwBufferLength);
		return MMSYSERR_NOERROR;
	}

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

