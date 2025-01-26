// ciscタンノエロガゾウキボンヌを強引にけろぴーに繋ぐための
// extern "C" の入れ方がきちゃなくてステキ（ぉ

// readme.txtに従って、改変点：
//  - opna.cppにYMF288用のクラス追加してます。OPNAそのまんまだけどね（ほんとは正しくないがまあいいや）
//  - 多分他は弄ってないはず……
// 2025/1/4  add YMFM support by KAMEYA
//  - YM2151をYMFM生成に変更 16bitPCMから32bitPCMで波形生成 

extern "C" {

#include "common.h"
#include "winx68k.h"
#include "dswin.h"
#include "prop.h"
#include "juliet.h"
#include "mfp.h"
#include "adpcm.h"
#include "mercury.h"
#include "fdc.h"
#include "ymfm_wrap.h"

#define RMBUFSIZE (256*1024)

typedef struct {
	uint32_t time;
	int32_t reg;
	uint8_t data;
} RMDATA;

};

#include "opm.h"
#include "opna.h"

// ===== YMFM =====

#include "ymfm_misc.h"
#include "ymfm_opl.h"
#include "ymfm_opm.h"
#include "ymfm_opn.h"

// ----- YMFM GLOBAL TYPES -----

// run this many dummy clocks of each chip before generating
#define EXTRA_CLOCKS (0)

// we use an int64_t as emulated time, as a 32.32 fixed point value
using emulated_time = int64_t;
emulated_time output_pos = 0;
emulated_time YM2151_output_step = 0x100000000ull / 44100;
int32_t ymfm_ym2151_vol;
uint8_t YM2151_reg;

// enumeration of the different types of chips we support
enum chip_type
{
	CHIP_YM2149,// AY-3-8910 互換
	CHIP_YM2151,// OPM X68000
	CHIP_YM2203,
	CHIP_YM2413,
	CHIP_YM2608,
	CHIP_YM2610,
	CHIP_YM2612,
	CHIP_YM3526,
	CHIP_Y8950,
	CHIP_YM3812,
	CHIP_YMF262,
	CHIP_YMF278B,
	CHIP_YMF288,// Mercury
	CHIP_TYPES
};

//  YMFM CLASSES
// abstract base class for a Yamaha chip; we keep a list of these for processing
// as new commands come in
class x68_chip_base
{
public:
	// construction
	x68_chip_base(uint32_t clock, chip_type type, char const *name) :
		m_type(type),
		m_name(name)
	{
	}

	// destruction
	virtual ~x68_chip_base()
	{
	}

	// simple getters
	chip_type type() const { return m_type; }
	virtual uint32_t sample_rate() const = 0;

	// required methods for derived classes to implement
	virtual void write(uint32_t reg, uint8_t data) = 0;
	virtual void generate(emulated_time output_start, emulated_time output_step, int32_t *buffer) = 0;

	// write data to the ADPCM-A buffer
	void write_data(ymfm::access_class type, uint32_t base, uint32_t length, uint8_t const *src)
	{
		uint32_t end = base + length;
		if (end > m_data[type].size())
			m_data[type].resize(end);
		memcpy(&m_data[type][base], src, length);
	}

	// seek within the PCM stream
	void seek_pcm(uint32_t pos) { m_pcm_offset = pos; }
	uint8_t read_pcm() { auto &pcm = m_data[ymfm::ACCESS_PCM]; return (m_pcm_offset < pcm.size()) ? pcm[m_pcm_offset++] : 0; }

protected:
	// internal state
	chip_type m_type;
	std::string m_name;
	std::vector<uint8_t> m_data[ymfm::ACCESS_CLASSES];
	uint32_t m_pcm_offset;

};

// ======================> x68_chip
template<typename ChipType>
class x68_chip : public x68_chip_base, public ymfm::ymfm_interface
{
public:
	// construction
	x68_chip(uint32_t clock, chip_type type, char const *name) :
		x68_chip_base(clock, type, name),
		m_chip(*this),
		m_clock(clock),
		m_clocks(0),
		m_step(0x100000000ull / m_chip.sample_rate(clock)),
		m_pos(0)
	{
		m_chip.reset();

		for (int clock = 0; clock < EXTRA_CLOCKS; clock++)
			m_chip.generate(&m_output);

	}

	virtual uint32_t sample_rate() const override
	{
		return m_chip.sample_rate(m_clock);
	}

	// handle a register write: just queue for now
	virtual void write(uint32_t reg, uint8_t data) override
	{
		m_queue.push_back(std::make_pair(reg, data));
	}

	// generate one output sample of output
	virtual void generate(emulated_time output_start, emulated_time output_step, int32_t *buffer) override
	{
		uint32_t addr1 = 0xffff, addr2 = 0xffff;
		uint8_t data1 = 0, data2 = 0;

		// see if there is data to be written; if so, extract it and dequeue
		if (!m_queue.empty())
		{
			auto front = m_queue.front();
			addr1 = 0 + 2 * ((front.first >> 8) & 3);
			data1 = front.first & 0xff;
			addr2 = addr1 + ((m_type == CHIP_YM2149) ? 2 : 1);
			data2 = front.second;
			m_queue.erase(m_queue.begin());
		}

		// write to the chip
		if (addr1 != 0xffff)
		{
			//p6logd("%10.5f: %s %03X=%02X\n", double(output_start) / double(1LL << 32), m_name.c_str(), data1 + 0x100 * (addr1/2), data2);
			m_chip.write(addr1, data1);
			m_chip.write(addr2, data2);
		}

		// generate at the appropriate sample rate
		for ( ; m_pos <= output_start; m_pos += m_step)
		{
			m_chip.generate(&m_output);
		}

		// add the final result to the buffer
		if (m_type == CHIP_YMF278B)
		{
			*buffer++ += m_output.data[4 % ChipType::OUTPUTS];
			*buffer++ += m_output.data[5 % ChipType::OUTPUTS];
		}
		else if (ChipType::OUTPUTS == 1)
		{
			*buffer++ += m_output.data[0];
			*buffer++ += m_output.data[0];
		}
		else
		{
			*buffer++ += m_output.data[0];
			*buffer++ += m_output.data[1 % ChipType::OUTPUTS];
		}
		m_clocks++;
	}

protected:
	// handle a read from the buffer
	virtual uint8_t ymfm_external_read(ymfm::access_class type, uint32_t offset) override
	{
		auto &data = m_data[type];
		return (offset < data.size()) ? data[offset] : 0;
	}

	// internal state
	ChipType m_chip;
	uint32_t m_clock;
	uint64_t m_clocks;
	typename ChipType::output_data m_output;
	emulated_time m_step;
	emulated_time m_pos;
	std::vector<std::pair<uint32_t, uint8_t>> m_queue;
};


// global list of active chips
std::vector<std::unique_ptr<x68_chip_base>> active_chips;

//-------------------------------------------------
//  add_chips - add 1 or 2 instances of the given
//  supported chip type
//-------------------------------------------------

template<typename ChipType>
void add_chips(uint32_t clock, chip_type type, char const *chipname)
{
	uint32_t clockval = clock & 0x3fffffff;
	int numchips = (clock & 0x40000000) ? 2 : 1;
	p6logd("Adding %s%s @ %dHz\n", (numchips == 2) ? "2 x " : "", chipname, clockval);
	for (int index = 0; index < numchips; index++)
	{
		char name[100];
		snprintf(name, sizeof(name) ,"%s #%d", chipname, index);
		active_chips.push_back(std::make_unique<x68_chip<ChipType>>(clockval, type, (numchips == 2) ? name : chipname));
	}

}

//-------------------------------------------------
//  find_chip - find the given chip and index
//-------------------------------------------------

x68_chip_base *find_chip(chip_type type, uint8_t index)
{
	for (auto &chip : active_chips)
		if (chip->type() == type && index-- == 0)
			return chip.get();
	return nullptr;
}

//-------------------------------------------------
//  write_chip - handle a write to the given chip
//  and index
//-------------------------------------------------

void write_chip(chip_type type, uint8_t index, uint32_t reg, uint8_t data)
{
	x68_chip_base *chip = find_chip(type, index);
	if (chip != nullptr){
		chip->write(reg, data);
	}
}


static RMDATA RMData[RMBUFSIZE];
static int32_t RMPtrW;
static int32_t RMPtrR;

class MyOPM : public FM::OPM
{
public:
	MyOPM();
	virtual ~MyOPM() {}
	void WriteIO(uint32_t adr, uint8_t data);
	void Count2(uint32_t clock);
private:
	virtual void Intr(bool);
	int32_t CurReg;
	uint32_t CurCount;
};


MyOPM::MyOPM()
{
	CurReg = 0;
}


void MyOPM::WriteIO(uint32_t adr, uint8_t data)
{
	if( adr&1 ) {
		if ( CurReg==0x1b ) {
			::ADPCM_SetClock((data>>5)&4);
			::FDC_SetForceReady((data>>6)&1);
		}
		SetReg((int)CurReg, (int)data);
	} else {
		CurReg = (int)data;
	}
}

void MyOPM::Intr(bool f)
{
	if ( f ) ::MFP_Int(12);
}


void MyOPM::Count2(uint32_t clock)
{
	CurCount += clock;
	Count(CurCount/10);
	CurCount %= 10;
}


static MyOPM* opm = NULL;

int32_t OPM_Init(int32_t clock, int32_t rate)
{

	RMPtrW = RMPtrR = 0;
	memset(RMData, 0, sizeof(RMData));

	opm = new MyOPM();
	if ( !opm ) return FALSE;
	if ( !opm->Init(clock, rate, TRUE) ) {
		delete opm;
		opm = NULL;
		return FALSE;
	}

	add_chips<ymfm::ym2151>(clock, CHIP_YM2151, "YM2151");
	YM2151_output_step = 0x100000000ull / rate;
	//add_chips<ymfm::ym2149>(clock, CHIP_YM2149, "YM2149");//AY-3-8910

	return TRUE;
}


void OPM_Cleanup(void)
{
	delete opm;
	opm = NULL;
}


void OPM_Reset(void)
{
	RMPtrW = RMPtrR = 0;
	memset(RMData, 0, sizeof(RMData));
	
	if ( opm ) opm->Reset();

	//x68_chip_base *chip = find_chip(CHIP_YM2151, 0);
	//chip->reset();
}


uint8_t FASTCALL OPM_Read(uint32_t adr)
{
	uint8_t ret = 0;
	(void)adr;
	if ( opm ) ret = opm->ReadStatus();

	//x68_chip_base *chip = find_chip(CHIP_YM2151, 0);
	//ret = chip->read();

	return ret;
}


void FASTCALL OPM_Write(uint32_t adr, uint8_t data)
{
	if ( opm )opm->WriteIO(adr, data);

	if(adr==0){
	 YM2151_reg = data;
	}
	else{
	 write_chip(CHIP_YM2151, 0, YM2151_reg, data);
	}

	return;
}


void OPM_Update(int32_t *buffer, int32_t length )
{
#if YMFM

	for(int32_t i=0; i<length; i+=2)
	{
		int32_t outputs[2] = { 0 };
		for (auto &chip : active_chips)
			chip->generate(output_pos, YM2151_output_step, outputs);
		output_pos += YM2151_output_step;
		buffer[ i ] = outputs[0] * ymfm_ym2151_vol ;
		buffer[i+1] = outputs[1] * ymfm_ym2151_vol ;
	}

#else

	int16_t output16[length];
	memset(&output16, 0, sizeof(output16));

	if ( opm ) opm->Mix((FM::Sample*)output16, length, output16, &output16[length]);

	for(int32_t i=0; i<length; i+=2)
	{
		buffer[ i ] = (int32_t)output16[i]   * 40000;
		buffer[i+1] = (int32_t)output16[i+1] * 40000;
	}

#endif

  return;
}


void FASTCALL OPM_Timer(uint32_t step)
{
	if ( opm ) opm->Count2(step);
}


void OPM_SetVolume(uint8_t vol)
{
	if ( vol>16 ) vol=16;// vol:0~16

	int32_t v = (vol)?((16-vol)*4):192;		// このくらいかなぁ
	if ( opm ) opm->SetVolume(-v);

	ymfm_ym2151_vol = (int32_t)vol * 1300;// YMFMはこのくらいかなぁ？
	//printf("vol:%d\n",ymfm_ym2151_vol);
}


/*void OPM_RomeoOut(uint32_t delay)
{
	uint32_t t = timeGetTime();
	if ( (juliet_YM2151IsEnable())&&(Config.SoundROMEO) ) {printf("juliet RomeoOut\n");
		while ( RMPtrW!=RMPtrR ) {
			if ( (t-RMData[RMPtrR].time)>=delay ) {
				juliet_YM2151W(RMData[RMPtrR].reg, RMData[RMPtrR].data);
				RMPtrR = (RMPtrR+1)%RMBUFSIZE;
			} else
				break;
		}
	}

}*/

// ----------------------------------------------------------
// ---------------------------- YMF288 (満開版ま〜きゅり〜)
// ----------------------------------------------------------
// TODO : ROMEOの288を叩くの

class YMF288 : public FM::Y288
{
public:
	YMF288();
	virtual ~YMF288() {}
	void WriteIO(uint32_t adr, uint8_t data);
	uint8_t ReadIO(uint32_t adr);
	void Count2(uint32_t clock);
	void SetInt(int32_t f) { IntrFlag = f; };
private:
	virtual void Intr(bool);
	int32_t CurReg[2];
	uint32_t CurCount;
	int32_t IntrFlag;
};

YMF288::YMF288()
{
	CurReg[0] = 0;
	CurReg[1] = 0;
	IntrFlag = 0;
}


void YMF288::WriteIO(uint32_t adr, uint8_t data)
{
	if( adr&1 ) {
		SetReg(((adr&2)?(CurReg[1]+0x100):CurReg[0]), (uint32_t)data);
	} else {
		CurReg[(adr>>1)&1] = (uint32_t)data;
	}
}


uint8_t YMF288::ReadIO(uint32_t adr)
{
	uint8_t ret = 0;
	if ( adr&1 ) {
		ret = GetReg(((adr&2)?(CurReg[1]+0x100):CurReg[0]));
	} else {
		ret = ((adr)?(ReadStatusEx()):(ReadStatus()));
	}
	return ret;
}


void YMF288::Intr(bool f)
{
	if ( (f)&&(IntrFlag) ) ::Mcry_Int();
}


void YMF288::Count2(uint32_t clock)
{
	CurCount += clock;
	Count(CurCount/10);
	CurCount %= 10;
}


static YMF288* ymf288a = NULL;
static YMF288* ymf288b = NULL;


int32_t M288_Init(int32_t clock, int32_t rate, const char* path)
{
	ymf288a = new YMF288();
	ymf288b = new YMF288();
	if ( (!ymf288a)||(!ymf288b) ) {
		M288_Cleanup();
		return FALSE;
	}
	if ( (!ymf288a->Init(clock, rate, TRUE, path))||(!ymf288b->Init(clock, rate, TRUE, path)) ) {
		M288_Cleanup();
		return FALSE;
	}

	ymf288a->SetInt(1);
	ymf288b->SetInt(0);

	add_chips<ymfm::ymf288>(clock, CHIP_YMF288, "YMF288");//ymfm
	add_chips<ymfm::ymf288>(clock, CHIP_YMF288, "YMF288");

	return TRUE;
}


void M288_Cleanup(void)
{
	delete ymf288a;
	delete ymf288b;
	ymf288a = ymf288b = NULL;
}


void M288_SetRate(int32_t clock, int32_t rate)
{
	if ( ymf288a ) ymf288a->SetRate(clock, rate, TRUE);
	if ( ymf288b ) ymf288b->SetRate(clock, rate, TRUE);
}


void M288_Reset(void)
{
	if ( ymf288a ) ymf288a->Reset();
	if ( ymf288b ) ymf288b->Reset();
}


uint8_t FASTCALL M288_Read(uint8_t adr)
{
	if ( adr<=3 ) {
		if ( ymf288a )
			return ymf288a->ReadIO(adr);
		else
			return 0;
	} else {
		if ( ymf288b )
			return ymf288b->ReadIO(adr&3);
		else
			return 0;
	}
}

uint8_t adr_A288,adr_B288;
void FASTCALL M288_Write(uint8_t adr, uint8_t data)
{
	if ( adr<=3 ) {
		if ( ymf288a ) ymf288a->WriteIO(adr, data);
		if(adr&0x01){
		  write_chip(CHIP_YMF288, 0, (uint8_t)adr_A288, (uint8_t)data);//ymfm
		}
		else{
		  adr_A288 = data;
		}
		
	} else {
		if ( ymf288b ) ymf288b->WriteIO(adr&3, data);
		if(adr&0x01){
		  write_chip(CHIP_YMF288, 1, (uint8_t)adr_B288, (uint8_t)data);//ymfm
		}
		else{
		  adr_B288 = data;
		}
	}

}


void M288_Update(int16_t *buffer, int32_t length)
{
	if ( ymf288a ) ymf288a->Mix((FM::Sample*)buffer, length);
	if ( ymf288b ) ymf288b->Mix((FM::Sample*)buffer, length);
}


void FASTCALL M288_Timer(uint32_t step)
{
	if ( ymf288a ) ymf288a->Count2(step);
	if ( ymf288b ) ymf288b->Count2(step);
}


void M288_SetVolume(uint8_t vol)
{
	int32_t v1 = (vol)?((16-vol)*4-24):192;		// このくらいかなぁ
	int32_t v2 = (vol)?((16-vol)*4):192;		// 少し小さめに
	if ( ymf288a ) {
		ymf288a->SetVolumeFM(-v1);
		ymf288a->SetVolumePSG(-v2);
	}
	if ( ymf288b ) {
		ymf288b->SetVolumeFM(-v1);
		ymf288b->SetVolumePSG(-v2);
	}
}
