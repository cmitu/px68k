/*
 macOS用CoreMIDI 対応の関数群
 Win.のMIDI関数とコンパチ(winmm.lib)で置き換えてます。
 CoreAudioのシンセサイザをdefault、CoreMIDIで検出された機器を
 リストします。 SoundFontにも対応します。
 MIMPIの音色変更FileはConfigで指定してください。
 
 2022/3/27  by kameya
*/

#import <CoreMIDI/MIDIServices.h>
#import <CoreMIDI/CoreMIDI.h>
#import <AudioToolbox/AUGraph.h>
#import <CoreServices/CoreServices.h>
#import <mach/mach_time.h>
#include "prop.h"
#include "winui.h"
#include "midi.h"

uint32_t midiOutShortMsg(HMIDIOUT , uint32_t );

	/* for CoreMIDI */
	MIDIPortRef mid_port;
	MIDIClientRef mid_client;
	MIDIEndpointRef mid_endpoint;
	MIDIPacket* mid_Packet = 0;

	/* for CoreAudio */
	AUGraph mid_auGraph;
	AudioUnit mid_synth;
	const char *soundfont;

	/* Play CoreAudio:0 or CoreMIDI:1 */
	uint32_t set_midout = 0;

	/* Create MIDIPacketList from packetbuff */
	const uint8_t packetBuf[MIDBUF_SIZE];
	MIDIPacketList *packetList = (MIDIPacketList *)packetBuf;

	const char mid_name[] = {"px68k-MIDI"};
	const char synth_name[] = {"DLSSynth(CoreAudio)"};

/*-----------------------------------------
   Load SoundFont file
------------------------------------------*/
uint32_t
load_soundfont(char *sf_name)
{
	OSStatus err_sts;

	if (sf_name[0]) {

			/* Get Sound Font URL */
			//soundfont = file_getcd(sf_name);
			soundfont = sf_name;

			/* SoundFont allocate */
			CFURLRef url = CFURLCreateFromFileSystemRepresentation(
								kCFAllocatorDefault,
								(const UInt8 *)soundfont,
								strlen(soundfont), false  );
			if (!url) {
				p6logd("CoreAudio:Failed to allocate CFURLRef from  %s\n",soundfont);
				return !MMSYSERR_NOERROR;
			}

			/* SoundFont load */
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

/*
   MIDI port Open (CoreAudio synth)
*/
uint32_t
mid_synthA_open(uint32_t num)
{
	AUNode outputNode;
	AUNode synthNode;
	AudioComponentDescription desc;

	OSStatus err_sts;

	if (mid_auGraph)
		return num;

	/* Open the Audio Unit Graph. */
	err_sts = NewAUGraph(&mid_auGraph);
	err_sts = AUGraphOpen(mid_auGraph);
	if (err_sts != noErr){
		p6logd("CoreAudio:Can't Open AUGraph\n");
		return num;
	}

	/* The default output AudioUnit */
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_DefaultOutput;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
	AUGraphAddNode(mid_auGraph, &desc, &outputNode);

	/* The built-in default (softsynth) AudioUnit */
	desc.componentType = kAudioUnitType_MusicDevice;
	desc.componentSubType = kAudioUnitSubType_DLSSynth;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	AUGraphAddNode(mid_auGraph, &desc, &synthNode);

	/* Connect the softsynth to the default output */
	AUGraphConnectNodeInput(mid_auGraph, synthNode, 0, outputNode, 0);

	/* initialize the graph */
	err_sts = AUGraphInitialize(mid_auGraph);
	if (err_sts != noErr){
		p6logd("CoreAudio:AUGraph Open/Init error\n");
		return num;
	}

	/* Get the music device from the graph. */
	AUGraphNodeInfo(mid_auGraph, synthNode, NULL, &mid_synth);

	/* load a soundfont (if needed) */
	load_soundfont((char *)Config.SoundFontFile);

	/* Start the graph */
	err_sts = AUGraphStart(mid_auGraph);
	if (err_sts != noErr){
		p6logd("CoreAudio:AUGraph can't start\n");
		return num;
	}

	strcpy(menu_items[8][num],synth_name);
	num++;
	strcpy(menu_items[8][num],"\0"); // Menu END

return num;
}

/*
   MIDI port Open (CoreMIDI synth)
*/
uint32_t
mid_synthM_open(uint32_t num)
{
	uint32_t Device_num = 0;
	OSStatus err_sts;

	/* Create Client for coreMIDI */
	err_sts = MIDIClientCreate(CFSTR("px68k"), NULL, NULL, &mid_client);

	if (err_sts != noErr)
	{
		p6logd("MIDI:CoreMIDI: No client created.");
		return num;
	}

	/* Create Source for client */
	//err_sts = MIDISourceCreate(mid_client, CFSTR("px68k MIDI Source"),
	//			&mid_endpoint);

	//if (err_sts != noErr)
	//{
	//	p6logd("MIDI:CoreMIDI: No Sorce created.");
	//	return !MMSYSERR_NOERROR;
	//}

	/* Create OutPort for client */
	err_sts = MIDIOutputPortCreate(mid_client, CFSTR("px68k MIDI Port"), &mid_port);

	if (err_sts != noErr)
	{
		p6logd("MIDI:CoreMIDI: No port created.");
		return num;
	}

	/* Get the MIDIEndPoint */
	mid_endpoint = 0;
	//uint32_t core_mid_num = MIDIGetNumberOfDevices();//仮想ポート含まない
	//mid_endpoint = MIDIGetDevice(core_mid_num);
	uint32_t core_mid_num = MIDIGetNumberOfDestinations();//仮想ポート含む
	if (core_mid_num == 0)
	{
		  return num; // No found
	}

	/* Store MIDI out port LIST */
	CFStringRef strRef;
	char mididevicename[64];
	Device_num = num;
	 for(uint32_t i = 0; i<core_mid_num; i++){
		mid_endpoint = MIDIGetDestination(i);
		MIDIObjectGetStringProperty(mid_endpoint, kMIDIPropertyName, &strRef);
		if(i<8){// MAX item check(８個までLISTに制限)
			CFStringGetCString(strRef, menu_items[8][Device_num], sizeof(menu_items[8][Device_num]), 0);
			p6logd("Find MIDI:%s\n",menu_items[8][Device_num]);
			Device_num ++;
		}
	 }
	//CFRelease(strRef);
	strcpy(menu_items[8][Device_num],"\0"); // Menu END 

return Device_num;
}

/*
 Search and Store MIDI Device LIST 
*/
uint32_t
mid_DevList(LPHMIDIOUT phmo)
{
	uint32_t Device_numA = 0;
	uint32_t Device_numM = 0;
	uint32_t Total_Device_num = 0;

	/*CoreAudio Device get*/
	Device_numA = mid_synthA_open(0);

	/*CoreMIDI Device search*/
	Total_Device_num = mid_synthM_open(Device_numA);

	if(Total_Device_num != 0){
	  *phmo = (HANDLE)mid_name; //MIDI Active!(ダミーを代入しておく)
	}

return Total_Device_num;
}

/*-----------------------------------------
   set/change MIDI Port 
------------------------------------------*/
void midOutChg(uint32_t port_no, uint32_t bank)
{
	OSStatus err_sts;
	uint32_t coremid_port = port_no;
	HMIDIOUT hmo;

	/* All note off */
	for (uint32_t msg=0x7bb0; msg<0x7bc0; msg++) {
		midiOutShortMsg(hmo, msg);
	}

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

	/* CoreMIDI endpoint change */
	mid_endpoint = MIDIGetDestination(coremid_port);
	if (err_sts != noErr){
		mid_endpoint = MIDIGetDestination((uint32_t)0);/* エラーだったらとりあえずPort0に再セットしておく*/
	}

	/* select BANK CC20 LSB */
	uint32_t msg = ((bank << 16) | (0x20 << 8) | 0xb0);/*Bank select2*/
	midiOutShortMsg(hmo, msg);

 return;
}

/*-----------------------------------------
   MIDI Port close
------------------------------------------*/
uint32_t
midiOutClose(HMIDIOUT hmo)
{
	(void)hmo;

	OSStatus err_sts;

	/* CoreAudio close */
	if (mid_auGraph) {
		AUGraphStop(mid_auGraph);
		DisposeAUGraph(mid_auGraph);
		mid_auGraph = 0;
	}

	/* CoreMIDI close */
	// Disconnect IN/OUT Port
	//err_sts = MIDIPortDisconnectSource(mid_port, mid_endpoint);
	//if (err_sts != noErr) p6logd("Disconnect MIDI-Source err\n");

	/* Dispose Port */
	err_sts = MIDIPortDispose(mid_port);
	if (err_sts != noErr) p6logd("Dispose MIDI-Port err\n");

	/* Dispose endpoint */
	//err_sts = MIDIEndpointDispose(mid_endpoint);
	//if (err_sts != noErr) p6logd("Dispose MIDI-Endpoint err\n");

	/* Dispose Client */
	err_sts = MIDIClientDispose(mid_client);
	if (err_sts != noErr) p6logd("Dispose MIDI-Client err\n");

	return MMSYSERR_NOERROR;
}

/*-----------------------------------------
   Send Short Message (演奏データ送信)
------------------------------------------*/
uint32_t
midiOutShortMsg(HMIDIOUT hmo, uint32_t msg)
{
	(void)hmo;
	uint8_t messg[4];

	/* (uint32)msg を 4byte に分解 */
	messg[0] =   msg       & 0xff; //status byte
	messg[1] =  (msg >> 8) & 0xff; //note No.
	messg[2] =  (msg >> 16) & 0xff;//velocity
	messg[3] =  (msg >> 24) & 0xff;// none

	/* Send the MIDI-Packet(CoreAudio) */
	if(set_midout == 0){
		MusicDeviceMIDIEvent(mid_synth, messg[0], messg[1], messg[2], 0);
		return MMSYSERR_NOERROR;
	}

	/* initialize MIDIPacketList */
	mid_Packet = MIDIPacketListInit(packetList);

	/* length of msg */
	uint32_t len;
	switch(messg[0] & 0xf0){
		case 0xc0://prog. chg
		case 0xd0://chnnel press.
			len = 2;
			break;
		case 0x80://note off
		case 0x90://     on
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

	/* 電文とタイムスタンプ入れてPacketList更新 */
	MIDITimeStamp midiTime = mach_absolute_time();
	MIDIPacketListAdd(packetList, (ByteCount)sizeof(packetBuf), mid_Packet, (MIDITimeStamp)midiTime, len, messg);

	/* PacketList送信(CoreMIDI) */
	MIDISend(mid_port,mid_endpoint,packetList);

	return MMSYSERR_NOERROR;
}

/*-----------------------------------------
   Exclusive GO!  (設定データ送信)
------------------------------------------*/
uint32_t
midiOutLongMsg(HMIDIOUT hmo, LPMIDIHDR pmh, uint32_t cbmh)
{
	(void)hmo;
	(void)cbmh;

	/* Send the MIDI-Packet(CoreAudio) */
	if(set_midout == 0){
		MusicDeviceSysEx(mid_synth, (uint8_t *)pmh->lpData, pmh->dwBufferLength);
		return MMSYSERR_NOERROR;
	}

	/* initialize MIDIPacketList */
	mid_Packet = MIDIPacketListInit(packetList);

	/* 電文とタイムスタンプ入れてPacketList更新 */
	MIDITimeStamp midiTime = mach_absolute_time();
	MIDIPacketListAdd(packetList, (ByteCount)sizeof(packetBuf), mid_Packet, 
				(MIDITimeStamp)midiTime, pmh->dwBufferLength, (uint8_t *)pmh->lpData);

	/* PacketList送信 */
	MIDISend(mid_port,mid_endpoint,packetList);

	return MMSYSERR_NOERROR;
}

/*---Dummy--*/

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

