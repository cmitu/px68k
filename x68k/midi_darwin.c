/*
 macOS用CoreMIDI 対応の関数群
 Win.のMIDI関数とコンパチ(winmm.lib)で置き換えてます。
 CoreMIDIで検出された機器をリストします。
 MIMPIの音色変更FileはConfigで指定してください。
 
 2022/3/27  by kameya
*/

#import <CoreMIDI/MIDIServices.h>
#import <CoreMIDI/CoreMIDI.h>
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

	/* Create MIDIPacketList from packetbuff */
	const uint8_t packetBuf[MIDBUF_SIZE];
	MIDIPacketList *packetList = (MIDIPacketList *)packetBuf;

	const char mid_name[] = {"px68k-MIDI"};

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
	uint32_t UTF8 = 134217984; //CFStringBuiltInEncodings.UTF8
	CFStringRef strRef;
	char mididevicename[64];
	Device_num = num;
	 for(uint32_t i = 0; i<core_mid_num; i++){
		mid_endpoint = MIDIGetDestination(i);
		MIDIObjectGetStringProperty(mid_endpoint, kMIDIPropertyDisplayName, &strRef);
		if(i<8){// MAX item check(８個までLISTに制限)
			CFStringGetCString(strRef, menu_items[8][Device_num], sizeof(menu_items[8][Device_num]), UTF8);
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
	uint32_t Device_numM = 0;
	uint32_t Total_Device_num = 0;

	/*CoreMIDI Device search*/
	Total_Device_num = mid_synthM_open(Device_numM);

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
	HMIDIOUT hmo;

	/* All note off */
	for (uint32_t msg=0x7bb0; msg<0x7bc0; msg++) {
		midiOutShortMsg(hmo, msg);
	}

	/* CoreMIDI endpoint change */
	mid_endpoint = MIDIGetDestination(port_no);
	if (mid_endpoint == 0){
		p6logd("MIDI Change error.\n");
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
	messg[0] = (uint8_t)(msg & 0xff);
	msg>>=8;
	messg[1] = (uint8_t)(msg & 0xff);
	msg>>=8;
	messg[2] = (uint8_t)(msg & 0xff);

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
			return MMSYSERR_NOERROR;//ないと思うけど
			break;
		default:
			return MMSYSERR_NOERROR;//ありえないハズ
			break;
	}

	/*(CoreMIDI) 電文とタイムスタンプ入れてPacketList更新 */
	MIDIPacketListAdd(packetList, (ByteCount)sizeof(packetBuf), mid_Packet, mach_absolute_time(), len, (uint8_t *)messg);

	/*(CoreMIDI) PacketList送信 */
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

	if(pmh->dwBufferLength == 0){ //length check
	  return MMSYSERR_NOERROR;
	}

	/*(CoreMIDI) initialize MIDIPacketList */
	mid_Packet = MIDIPacketListInit(packetList);

	/*(CoreMIDI) 電文とタイムスタンプ入れてPacketList更新 */
	MIDIPacketListAdd(packetList, (ByteCount)sizeof(packetBuf), mid_Packet, 
				mach_absolute_time(), pmh->dwBufferLength, (uint8_t *)pmh->lpData);

	/*(CoreMIDI) PacketList送信 */
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

