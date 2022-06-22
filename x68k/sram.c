// ------------------------------------------------------------------------------
//  SRAM.C - SRAM (16kb) Area
//$ed0000	'Ｘ68000',$57 SRAMチェックデータ
//$ed000c	ROM 起動アドレスへのポインタ $00fc0000:InSCSI-IPL $00ea0020:ExSCSI-IPL
//$ed0010	$00ed_0100	SRAM 起動アドレス
//$ed0018	$0000=STD boot
//$ed001c	$00 KeyBoardの状態
//$ed005a	Set SASI Active Drive(0~15)
//~
//$ed006f	'V' ($00:SCSI無効 $56='V':SCSI有効)
//$ed0070	$0f SCSI bit0,1,2=Own-ID  bit3:1=CZ-6BS1
//$ed0071	$00 SASIドライブ接続フラグ
// ------------------------------------------------------------------------------

#include	"common.h"
#include	"fileio.h"
#include	"prop.h"
#include	"winx68k.h"
#include	"sysport.h"
#include	"x68kmemory.h"
#include	"sram.h"

	uint8_t	SRAM[0x4000];
	char	SRAMFILE[] = "sram.dat";

// -----------------------------------------------------------
//   SCSI-IPL 起動をSRAMに設定
// -----------------------------------------------------------
void 
SRAM_SetSCSIMode(int32_t mode)/*1:Ex-SCSI 2:In-SCSI*/
{

	Memory_WriteB(0xe8e00d, 0x31);	/* Allow SRAM Access(91byte)*/
	switch(mode){/*SRAM SCSI set*/
		case 0:/*No-SCSI*/
			Memory_WriteB(0xea006f, 0x00);	/*No SCSI*/
			Memory_WriteB(0xea0070, 0x07);	/*Set ID=7*/
			Memory_WriteB(0xea0071, 0x00);	/*SASI flag all 0*/
			break;
		case 1:/*SCSI EX*/
			Memory_WriteB(0xea000c, 0x00);	/*ExSCSI-IPL*/
			Memory_WriteB(0xea000d, 0xea);
			Memory_WriteB(0xea000e, 0x00);
			Memory_WriteB(0xea000f, 0x20);
			Memory_WriteB(0xea006f, 'V');	/*Activate SCSI*/
			Memory_WriteB(0xea0070, 0x0f);	/*ExternalSCSI Set ID=7*/
			Memory_WriteB(0xea0071, 0x00);	/*SASI flag all 0*/
			break;
		case 2:/*SCSI IN*/
			Memory_WriteB(0xea000c, 0x00);	/*InSCSI-IPL*/
			Memory_WriteB(0xea000d, 0xfc);
			Memory_WriteB(0xea000e, 0x00);
			Memory_WriteB(0xea000f, 0x00);
			Memory_WriteB(0xea006f, 'V');	/*Activate SCSI*/
			Memory_WriteB(0xea0070, 0x07);	/*InternalSCSI Set ID=7*/
			Memory_WriteB(0xea0071, 0x00);	/*SASI flag all 0*/
			break;
		default:
			break;
	}
	Memory_WriteB(0xe8e00d, 0x55);	/* Block SRAM Access(91byte)*/

	return;
}

// -----------------------------------------------------------
//   SASI 有効ドライブをSRAMに設定
// -----------------------------------------------------------
void 
SRAM_SetSASIDrive(uint8_t drive)/*0~15 Active SASI drive*/
{
	if(drive>15) return;

	Memory_WriteB(0xe8e00d, 0x31);	/* Allow SRAM Access(91byte)*/

		Memory_WriteB(0xea005a, drive); /*SASI Dive (set active)*/

	Memory_WriteB(0xe8e00d, 0x55);	/* Block SRAM Access(91byte)*/

	return;
}

// -----------------------------------------------------------------------
//   役に立たないうぃるすチェック
// -----------------------------------------------------------------------
void SRAM_VirusCheck(void)
{

	if (!Config.SRAMWarning) return;				// Warning発生モードでなければ帰る

	if ( (cpu_readmem24_dword(0xed3f60)==0x60000002)
	   &&(cpu_readmem24_dword(0xed0010)==0x00ed3f60) )		// 特定うぃるすにしか効かないよxAｷ
	{
#if 0 /* XXX */
		int ret = MessageBox(hWndMain,
			"このSRAMデータはウィルスに感染している可能性があります。\n該当個所のクリーンアップを行いますか？",
			"けろぴーからの警告", MB_ICONWARNING | MB_YESNO);
		if (ret == IDYES)
		{
			for (int_fast16_t i=0x3c00; i<0x4000; i++)
				SRAM[i] = 0xFF;
			SRAM[0x11] = 0x00;
			SRAM[0x10] = 0xed;
			SRAM[0x13] = 0x01;
			SRAM[0x12] = 0x00;
			SRAM[0x19] = 0x00;
		}
#endif /* XXX */
		SRAM_Cleanup();
		SRAM_Init();			// Virusクリーンアップ後のデータを書き込んでおく
	}
}


// -----------------------------------------------------------------------
//   初期化(Init S-RAM)
// -----------------------------------------------------------------------
void SRAM_Init(void)
{
	int_fast32_t i;
	uint8_t tmp;
	FILEH fp;

	for (i=0; i<0x4000; i++)
		SRAM[i] = 0xFF;

	fp = File_OpenCurDir((char *)SRAMFILE);
	if (fp)
	{
		File_Read(fp, SRAM, 0x4000);
		File_Close(fp);
		/*for little endian guys!*/
#ifndef C68K_BIG_ENDIAN
		for (i=0; i<0x4000; i+=2)
		{
			tmp = SRAM[i];
			SRAM[i] = SRAM[i+1];
			SRAM[i+1] = tmp;
		}
#endif
	}
}


// -----------------------------------------------------------------------
//   撤収(Clear S-RAM)
// -----------------------------------------------------------------------
void SRAM_Cleanup(void)
{
	uint8_t tmp;
	FILEH fp;

/*for little endian guys!*/
#ifndef C68K_BIG_ENDIAN
	for (int_fast32_t i=0; i<0x4000; i+=2)
	{
		tmp = SRAM[i];
		SRAM[i] = SRAM[i+1];
		SRAM[i+1] = tmp;
	}
#endif

	fp = File_OpenCurDir((char *)SRAMFILE);
	if (!fp)
		fp = File_CreateCurDir((char *)SRAMFILE, FTYPE_SRAM);
	if (fp)
	{
		File_Write(fp, SRAM, 0x4000);
		File_Close(fp);
	}
}


// -----------------------------------------------------------------------
//   りーど(Read S-RAM)
// -----------------------------------------------------------------------
uint8_t FASTCALL SRAM_Read(uint32_t adr)
{
	adr &= 0xffff;
#ifndef C68K_BIG_ENDIAN
	adr ^= 1;
#endif
	if (adr<0x4000)
		return SRAM[adr];
	else
		return 0xff;
}


// -----------------------------------------------------------------------
//   らいと(Write S-RAM)
// -----------------------------------------------------------------------
void FASTCALL SRAM_Write(uint32_t adr, uint8_t data)
{

	if ( (SysPort[5]==0x31)&&(adr<0xed4000) )
	{
		if ((adr==0xed0018)&&(data==0xb0))	// SRAM起動への切り替え（簡単なウィルス対策）
		{
			if (Config.SRAMWarning)		// Warning発生モード（デフォルト）
			{
#if 0 /* XXX */
				int ret = MessageBox(hWndMain,
					"SRAMブートに切り替えようとしています。\nウィルスの危険がない事を確認してください。\nSRAMブートに切り替え、継続しますか？",
					"けろぴーからの警告", MB_ICONWARNING | MB_YESNO);
				if (ret != IDYES)
				{
					data = 0;	// STDブートにする
				}
#endif /* XXX */
			}
		}
		adr &= 0xffff;
#ifndef C68K_BIG_ENDIAN
		adr ^= 1;
#endif
		SRAM[adr] = data;
	}
}
