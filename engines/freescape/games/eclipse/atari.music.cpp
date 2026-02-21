/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * Total Eclipse Atari ST music player (YM2149 PSG).
 *
 * Plays background music from the TEMUSIC.ST embedded GEMDOS executable.
 * Uses the same Wally Beben byte-stream pattern format as the Amiga
 * Dark Side engine (wb.cpp), but outputs to the YM2149/AY-3-8912 PSG
 * instead of Amiga Paula.
 *
 * TEMUSIC.ST data table offsets (TEXT-relative):
 *   $0B24  Period table (96 x uint16 BE)
 *   $0CC8  Arpeggio interval lookup (8 bytes)
 *   $0D60  Instrument table (12 x 8 bytes)
 *   $0DC0  Song table (2 songs x 3 channels x uint32 BE order-list pointers)
 *   $0DCC  Pattern pointer table (up to 31 x uint32 BE)
 */

#include "audio/softsynth/ay8912.h"

#include "freescape/freescape.h"

#include "common/endian.h"
#include "common/debug.h"
#include "common/util.h"

namespace Freescape {

// TEXT-relative offsets for data tables within TEMUSIC.ST
static const uint32 kTEPeriodTableOffset      = 0x0B24; // 96 x uint16 BE
static const uint32 kTEArpeggioIntervalsOffset = 0x0CC8; // 8 bytes
static const uint32 kTEInstrumentTableOffset   = 0x0D60; // 12 x 8 bytes
static const uint32 kTESongTableOffset         = 0x0DC0; // 2 songs x 3 ch x uint32 BE
static const uint32 kTEPatternPtrTableOffset   = 0x0DCC; // up to 31 x uint32 BE

static const int kTENumChannels    = 3;
static const int kTENumPeriods     = 96;
static const int kTENumInstruments = 12;
static const int kTEMaxPatterns    = 31;

class EclipseAtariMusicStream : public Audio::AY8912Stream {
public:
	EclipseAtariMusicStream(const byte *data, uint32 dataSize, int songNum, int rate = 44100);
	~EclipseAtariMusicStream() override {}

	int readBuffer(int16 *buffer, const int numSamples) override;
	bool endOfData() const override { return !_musicActive; }
	bool endOfStream() const override { return !_musicActive; }

private:
	// --- Data tables ---
	const byte *_data;
	uint32 _dataSize;

	uint16 _periods[kTENumPeriods];

	struct InstrumentDesc {
		byte volume;       // Initial volume (0-$3F)
		byte targetVol;    // Sustain/target volume
		byte attackRate;   // Volume increment per tick
		byte releaseRate;  // Volume decrement per tick
		byte envFlags;     // Bit 7: hardware envelope
		byte effectType;   // Effect configuration
		byte arpeggioData; // Arpeggio bit pattern
		byte flags;        // Additional flags
	};
	InstrumentDesc _instruments[kTENumInstruments];

	// Song order list pointers (TEXT-relative)
	uint32 _songOrderPtrs[2][kTENumChannels];

	// Pattern pointer table
	uint32 _patternPtrs[kTEMaxPatterns];
	uint32 _numPatterns;

	// Arpeggio interval lookup
	byte _arpeggioIntervals[8];

	// --- Per-channel state ---
	struct ChannelState {
		// Order list
		uint32 orderListOffset;
		int orderListPos;
		int8 transpose;

		// Current pattern
		uint32 patternOffset;
		int patternPos;

		// Note state
		byte note;
		byte prevNote;
		byte duration;
		int durationCounter;

		// Instrument
		byte instrumentIdx;

		// Volume envelope
		byte volume;       // Current volume (0-63 internal scale)
		byte attackLevel;  // Initial volume on note-on
		byte decayTarget;  // Target volume to hold
		byte attackRate;   // Increment per tick
		byte releaseRate;  // Decrement per tick
		byte envelopePhase; // 0=attack, 1=decay, 2=sustain, 3=release

		// Effects
		byte effectMode;   // 0=none, 1=vibrato, 2=arpeggio
		bool portaUp;
		bool portaDown;
		int16 portaStep;
		int16 portaTarget;
		byte arpeggioMask;
		byte arpeggioPos;

		// Vibrato
		byte vibratoSpeed;  // Phase increment per tick
		byte vibratoDepth;  // Amplitude in period units
		int8 vibratoPos;    // Current phase position (oscillates)
		int8 vibratoDir;    // +1 or -1

		// Noise
		bool noiseEnabled;  // Instrument flags bit 1: noise mode
		bool freqSweep;     // Instrument flags bit 2: frequency sweep

		// Period
		int16 basePeriod;
		int16 outputPeriod;

		bool active;
	};
	ChannelState _channels[kTENumChannels];

	// --- Global state ---
	bool _musicActive;
	byte _tickSpeed;
	byte _tickCounter;
	int _tickSampleCount;

	// Arpeggio working table
	byte _arpeggioTable[16];
	int _arpeggioTableLen;

	// --- Methods ---
	void loadTables();
	void startSong(int songNum);
	void initChannel(int ch);
	void readOrderList(int ch);
	void readPatternCommands(int ch);
	void triggerNote(int ch);
	void processEffects(int ch);
	void processEnvelope(int ch);
	void buildArpeggioTable(byte mask);
	void tickUpdate();
	void writeYMRegisters();

	uint16 getPeriod(int note) const {
		if (note < 0 || note >= kTENumPeriods)
			return 0;
		return _periods[note];
	}

	byte readDataByte(uint32 offset) const {
		if (offset < _dataSize)
			return _data[offset];
		return 0;
	}

	uint16 readDataWord(uint32 offset) const {
		if (offset + 1 < _dataSize)
			return READ_BE_UINT16(_data + offset);
		return 0;
	}

	uint32 readDataLong(uint32 offset) const {
		if (offset + 3 < _dataSize)
			return READ_BE_UINT32(_data + offset);
		return 0;
	}
};

// ---------------------------------------------------------------------------
// Construction / data loading
// ---------------------------------------------------------------------------

EclipseAtariMusicStream::EclipseAtariMusicStream(const byte *data, uint32 dataSize,
                                                   int songNum, int rate)
	: AY8912Stream(rate, 2000000), // YM2149 at 2 MHz on Atari ST
	  _data(data), _dataSize(dataSize),
	  _musicActive(false), _tickSpeed(6), _tickCounter(0),
	  _tickSampleCount(0),
	  _arpeggioTableLen(0), _numPatterns(0) {

	memset(_periods, 0, sizeof(_periods));
	memset(_instruments, 0, sizeof(_instruments));
	memset(_songOrderPtrs, 0, sizeof(_songOrderPtrs));
	memset(_patternPtrs, 0, sizeof(_patternPtrs));
	memset(_arpeggioIntervals, 0, sizeof(_arpeggioIntervals));
	memset(_channels, 0, sizeof(_channels));
	memset(_arpeggioTable, 0, sizeof(_arpeggioTable));

	// Reset all YM registers
	for (int r = 0; r < 14; r++)
		setReg(r, 0);
	// Mixer: all channels disabled initially
	setReg(7, 0x3F);

	loadTables();
	startSong(songNum);
}

void EclipseAtariMusicStream::loadTables() {
	// Period table: 96 x uint16 BE at TEXT+$0B24
	for (int i = 0; i < kTENumPeriods; i++) {
		_periods[i] = readDataWord(kTEPeriodTableOffset + i * 2);
	}

	// Fix note 46: corrupted by data artifact ($3095 instead of $010D).
	// Correct value interpolated from surrounding notes (45=$011D, 47=$00FE).
	if (_periods[46] == 0x3095) {
		_periods[46] = 0x010D;
		debug(3, "TE-Atari: Fixed corrupted period for note 46 ($3095 -> $010D)");
	}

	// Arpeggio interval table: 8 bytes at TEXT+$0CC8
	for (int i = 0; i < 8; i++) {
		_arpeggioIntervals[i] = readDataByte(kTEArpeggioIntervalsOffset + i);
	}

	// Instrument table: 12 x 8 bytes at TEXT+$0D60
	for (int i = 0; i < kTENumInstruments; i++) {
		uint32 off = kTEInstrumentTableOffset + i * 8;
		_instruments[i].volume      = readDataByte(off + 0);
		_instruments[i].targetVol   = readDataByte(off + 1);
		_instruments[i].attackRate  = readDataByte(off + 2);
		_instruments[i].releaseRate = readDataByte(off + 3);
		_instruments[i].envFlags    = readDataByte(off + 4);
		_instruments[i].effectType  = readDataByte(off + 5);
		_instruments[i].arpeggioData = readDataByte(off + 6);
		_instruments[i].flags       = readDataByte(off + 7);
	}

	// Song table: 2 songs x 3 channels x uint32 BE at TEXT+$0DC0
	for (int s = 0; s < 2; s++) {
		for (int ch = 0; ch < kTENumChannels; ch++) {
			_songOrderPtrs[s][ch] = readDataLong(kTESongTableOffset + s * 12 + ch * 4);
		}
	}

	// Pattern pointer table at TEXT+$0DCC
	_numPatterns = 0;
	for (uint32 i = 0; i < kTEMaxPatterns; i++) {
		uint32 ptr = readDataLong(kTEPatternPtrTableOffset + i * 4);
		_patternPtrs[i] = ptr;
		if (ptr > 0 && ptr < _dataSize)
			_numPatterns = i + 1;
	}

	debug(3, "TE-Atari: Loaded music data (%u bytes)", _dataSize);
	debug(3, "TE-Atari: %d valid patterns", _numPatterns);

	for (int s = 0; s < 2; s++) {
		debug(3, "TE-Atari: Song %d order ptrs: $%X $%X $%X",
			s + 1, _songOrderPtrs[s][0], _songOrderPtrs[s][1], _songOrderPtrs[s][2]);
	}

	for (int i = 0; i < kTENumInstruments; i++) {
		const InstrumentDesc &inst = _instruments[i];
		if (inst.volume > 0 || inst.targetVol > 0)
			debug(3, "TE-Atari: Inst %d: vol=%d target=%d atk=%d rel=%d envFlags=$%02X effect=$%02X arp=$%02X flags=$%02X",
				i, inst.volume, inst.targetVol, inst.attackRate, inst.releaseRate,
				inst.envFlags, inst.effectType, inst.arpeggioData, inst.flags);
	}
}

// ---------------------------------------------------------------------------
// Song init
// ---------------------------------------------------------------------------

void EclipseAtariMusicStream::startSong(int songNum) {
	_musicActive = false;

	if (songNum < 1 || songNum > 2)
		return;

	int songIdx = songNum - 1;
	_tickSpeed = 6;
	_tickCounter = 0;
	_arpeggioTableLen = 0;

	// Silence all YM channels
	for (int r = 0; r < 14; r++)
		setReg(r, 0);
	setReg(7, 0x3F); // All disabled

	for (int ch = 0; ch < kTENumChannels; ch++) {
		initChannel(ch);
		_channels[ch].orderListOffset = _songOrderPtrs[songIdx][ch];
		_channels[ch].orderListPos = 0;
		_channels[ch].active = true;
		readOrderList(ch);
	}

	_musicActive = true;

	debug(3, "TE-Atari: Song %d started, tickSpeed=%d", songNum, _tickSpeed);
	for (int ch = 0; ch < kTENumChannels; ch++) {
		debug(3, "TE-Atari: ch%d orderList=$%X pattern=$%X",
			ch, _channels[ch].orderListOffset, _channels[ch].patternOffset);
	}
}

void EclipseAtariMusicStream::initChannel(int ch) {
	ChannelState &c = _channels[ch];
	memset(&c, 0, sizeof(ChannelState));
	c.duration = 1;
	c.durationCounter = 0;
	c.envelopePhase = 3; // Start in release (silent)
	c.attackLevel = 0x36; // Default from instrument 1
	c.decayTarget = 0x36;
}

// ---------------------------------------------------------------------------
// Order list reader
// Same format as wb.cpp: $00-$C0=pattern#, $C1-$FE=transpose, $FF=loop
// ---------------------------------------------------------------------------

void EclipseAtariMusicStream::readOrderList(int ch) {
	ChannelState &c = _channels[ch];

	for (int safety = 0; safety < 256; safety++) {
		if (c.orderListOffset + c.orderListPos >= _dataSize)
			break;

		byte cmd = readDataByte(c.orderListOffset + c.orderListPos);
		c.orderListPos++;

		if (cmd == 0xFF) {
			c.orderListPos = 0;
			continue;
		}

		if (cmd > 0xC0) {
			c.transpose = (int8)((cmd + 0x20) & 0xFF);
			continue;
		}

		if (cmd < _numPatterns && _patternPtrs[cmd] > 0 && _patternPtrs[cmd] < _dataSize) {
			c.patternOffset = _patternPtrs[cmd];
			c.patternPos = 0;
			debugC(3, kFreescapeDebugParser, "TE-Atari: ch%d order -> pattern %d (offset $%04X)", ch, cmd, c.patternOffset);
		} else {
			// Invalid pattern index — skip it and try next order entry
			debugC(3, kFreescapeDebugParser, "TE-Atari: ch%d skipping invalid pattern index %d", ch, cmd);
			continue;
		}
		return;
	}

	warning("TE-Atari: ch%d order list safety limit hit", ch);
}

// ---------------------------------------------------------------------------
// Pattern command reader
// Same format as wb.cpp: $FF=end-pattern, $FE=end-song, $F0+=speed,
//   $C0+=instrument, $80+=duration, $7F/$7E=portamento,
//   $7D/$7C=vibrato/arpeggio, $00-$5F=note
// ---------------------------------------------------------------------------

void EclipseAtariMusicStream::readPatternCommands(int ch) {
	ChannelState &c = _channels[ch];

	for (int safety = 0; safety < 256; safety++) {
		if (c.patternOffset + c.patternPos >= _dataSize)
			break;

		byte cmd = readDataByte(c.patternOffset + c.patternPos);
		c.patternPos++;

		if (cmd == 0xFF) {
			readOrderList(ch);
			continue;
		}

		if (cmd == 0xFE) {
			_musicActive = false;
			return;
		}

		if (cmd == 0xFC) {
			// Song change command — read parameter, restart
			byte newSong = readDataByte(c.patternOffset + c.patternPos);
			c.patternPos++;
			if (newSong >= 1 && newSong <= 2) {
				startSong(newSong);
			}
			return;
		}

		if (cmd >= 0xF0) {
			_tickSpeed = cmd & 0x0F;
			if (_tickSpeed == 0)
				_tickSpeed = 1;
			continue;
		}

		if (cmd >= 0xC0) {
			// Instrument select: (cmd & $1F) = instrument index
			byte instIdx = cmd & 0x1F;
			if (instIdx < kTENumInstruments) {
				c.instrumentIdx = instIdx;
				const InstrumentDesc &inst = _instruments[instIdx];
				c.attackLevel = inst.volume;
				c.decayTarget = inst.targetVol;
				c.attackRate = inst.attackRate;
				c.releaseRate = inst.releaseRate;

				// Instrument effect type: high nibble = vibrato speed,
				// low nibble = vibrato depth (in period units)
				if (inst.effectType != 0) {
					c.effectMode = 1; // vibrato
					c.vibratoSpeed = (inst.effectType >> 4) & 0x0F;
					c.vibratoDepth = inst.effectType & 0x0F;
					c.vibratoPos = 0;
					c.vibratoDir = 1;
					c.portaUp = false;
					c.portaDown = false;
				} else if (inst.arpeggioData != 0) {
					c.effectMode = 2; // arpeggio
					c.arpeggioMask = inst.arpeggioData;
					buildArpeggioTable(inst.arpeggioData);
				} else {
					c.effectMode = 0;
				}

				// Noise flags from instrument byte 7
				c.noiseEnabled = (inst.flags & 0x02) != 0;
				c.freqSweep = (inst.flags & 0x04) != 0;
			}
			continue;
		}

		if (cmd >= 0x80) {
			c.duration = cmd & 0x3F;
			if (c.duration == 0)
				c.duration = 1;
			continue;
		}

		if (cmd == 0x7F) {
			c.portaUp = true;
			c.portaDown = false;
			c.effectMode = 1;
			continue;
		}

		if (cmd == 0x7E) {
			c.portaDown = true;
			c.portaUp = false;
			c.effectMode = 1;
			continue;
		}

		if (cmd == 0x7D) {
			// Arpeggio / effect mode 1 — param is bitmask into interval table
			byte param = readDataByte(c.patternOffset + c.patternPos);
			c.patternPos++;
			c.effectMode = 2; // arpeggio
			c.arpeggioMask = param;
			c.arpeggioPos = 0;
			buildArpeggioTable(param);
			continue;
		}

		if (cmd == 0x7C) {
			// Effect mode 2 — alternate arpeggio/vibrato
			byte param = readDataByte(c.patternOffset + c.patternPos);
			c.patternPos++;
			c.effectMode = 2; // arpeggio
			c.arpeggioMask = param;
			c.arpeggioPos = 0;
			buildArpeggioTable(param);
			continue;
		}

		if (cmd == 0x7A) {
			// Delay command — skip parameter byte (same as wb.cpp)
			c.patternPos++;
			continue;
		}

		// Note value ($00-$5F)
		c.prevNote = c.note;
		c.note = cmd;
		c.durationCounter = c.duration;
		triggerNote(ch);
		return;
	}

	warning("TE-Atari: ch%d pattern read safety limit hit", ch);
}

// ---------------------------------------------------------------------------
// Note trigger — set YM period, reset envelope
// ---------------------------------------------------------------------------

void EclipseAtariMusicStream::triggerNote(int ch) {
	ChannelState &c = _channels[ch];

	if (c.note == 0) {
		// Rest — silence channel
		c.outputPeriod = 0;
		c.volume = 0;
		c.envelopePhase = 3;
		return;
	}

	// Apply transpose and clamp
	int note = c.note + c.transpose;
	if (note < 1) note = 1;
	if (note >= kTENumPeriods) note = kTENumPeriods - 1;

	c.basePeriod = getPeriod(note);
	c.outputPeriod = c.basePeriod;

	if (c.basePeriod == 0) {
		warning("TE-Atari: ch%d note %d has period 0", ch, note);
		return;
	}

	// Reset envelope
	c.envelopePhase = 0;
	c.volume = c.attackLevel;

	debugC(3, kFreescapeDebugParser, "TE-Atari: ch%d NOTE note=%d(+%d) period=%d inst=%d vol=%d",
		ch, c.note, c.transpose, c.basePeriod, c.instrumentIdx, c.volume);

	// Set up portamento if active
	if (c.portaUp || c.portaDown) {
		int prevNote = c.prevNote + c.transpose;
		if (prevNote < 1) prevNote = 1;
		if (prevNote >= kTENumPeriods) prevNote = kTENumPeriods - 1;

		int16 prevPeriod = (c.prevNote > 0) ? getPeriod(prevNote) : c.basePeriod;
		int16 delta = ABS(c.basePeriod - prevPeriod);
		int steps = (_tickSpeed > 0) ? _tickSpeed : 1;
		c.portaStep = delta / steps;
		if (c.portaStep == 0)
			c.portaStep = 1;
		c.portaTarget = c.basePeriod;
		c.basePeriod = prevPeriod;
		c.outputPeriod = prevPeriod;
	}
}

// ---------------------------------------------------------------------------
// Effects processing — runs every tick (50 Hz)
// ---------------------------------------------------------------------------

void EclipseAtariMusicStream::processEffects(int ch) {
	ChannelState &c = _channels[ch];

	// Portamento takes priority (active during porta regardless of effectMode)
	if (c.portaUp) {
		c.basePeriod -= c.portaStep;
		if (c.basePeriod <= c.portaTarget) {
			c.basePeriod = c.portaTarget;
			c.portaUp = false;
		}
		c.outputPeriod = c.basePeriod;
		return;
	}

	if (c.portaDown) {
		c.basePeriod += c.portaStep;
		if (c.basePeriod >= c.portaTarget) {
			c.basePeriod = c.portaTarget;
			c.portaDown = false;
		}
		c.outputPeriod = c.basePeriod;
		return;
	}

	if (c.effectMode == 1 && c.vibratoDepth > 0) {
		// Vibrato: triangle wave pitch oscillation around basePeriod
		c.vibratoPos += c.vibratoDir;
		if (c.vibratoPos >= (int8)c.vibratoDepth) {
			c.vibratoPos = c.vibratoDepth;
			c.vibratoDir = -1;
		} else if (c.vibratoPos <= -(int8)c.vibratoDepth) {
			c.vibratoPos = -(int8)c.vibratoDepth;
			c.vibratoDir = 1;
		}
		// Scale vibrato offset by speed (higher speed = faster oscillation)
		int16 offset = c.vibratoPos * c.vibratoSpeed;
		c.outputPeriod = c.basePeriod + offset;
		if (c.outputPeriod < 1) c.outputPeriod = 1;
		return;
	}

	if (c.effectMode == 2 && _arpeggioTableLen > 0) {
		// Arpeggio: cycle through note offsets from interval table
		int note = c.note + c.transpose;
		int offset = _arpeggioTable[c.arpeggioPos % _arpeggioTableLen];
		note += offset;
		if (note < 1) note = 1;
		if (note >= kTENumPeriods) note = kTENumPeriods - 1;
		c.outputPeriod = getPeriod(note);
		c.arpeggioPos++;
		if (c.arpeggioPos >= _arpeggioTableLen)
			c.arpeggioPos = 0;
		return;
	}

	c.outputPeriod = c.basePeriod;
}

// ---------------------------------------------------------------------------
// Volume envelope — runs every tick (50 Hz)
// Volume range: 0-63 internal, written to YM as >>2 (0-15)
// ---------------------------------------------------------------------------

void EclipseAtariMusicStream::processEnvelope(int ch) {
	ChannelState &c = _channels[ch];

	switch (c.envelopePhase) {
	case 0: // Attack — volume set to attackLevel in triggerNote
		if (c.attackRate == 0) {
			c.volume = c.decayTarget;
			c.envelopePhase = 2;
		} else {
			c.envelopePhase = 1;
		}
		break;

	case 1: // Decay — fade toward target
		if (c.volume < c.decayTarget) {
			c.volume++;
		} else if (c.volume > c.decayTarget) {
			c.volume--;
		} else {
			c.envelopePhase = 2;
		}
		break;

	case 2: // Sustain — hold
		break;

	case 3: // Release
		if (c.releaseRate > 0) {
			if (c.volume > c.releaseRate) {
				c.volume -= c.releaseRate;
			} else {
				c.volume = 0;
			}
		} else {
			c.volume = 0;
		}
		break;
	}

	if (c.volume > 63)
		c.volume = 63;
}

// ---------------------------------------------------------------------------
// Arpeggio table builder
// ---------------------------------------------------------------------------

void EclipseAtariMusicStream::buildArpeggioTable(byte mask) {
	_arpeggioTableLen = 0;
	_arpeggioTable[_arpeggioTableLen++] = 0; // Base note

	for (int i = 0; i < 8 && _arpeggioTableLen < 16; i++) {
		if (mask & (1 << i)) {
			_arpeggioTable[_arpeggioTableLen++] = _arpeggioIntervals[i];
		}
	}

	if (_arpeggioTableLen <= 1)
		_arpeggioTableLen = 0;
}

// ---------------------------------------------------------------------------
// Write channel state to YM2149 registers
// ---------------------------------------------------------------------------

void EclipseAtariMusicStream::writeYMRegisters() {
	byte mixer = 0x3F; // Start with all disabled (bits 0-2=tone, bits 3-5=noise)

	for (int ch = 0; ch < kTENumChannels; ch++) {
		ChannelState &c = _channels[ch];

		if (!c.active || c.outputPeriod == 0 || c.volume == 0) {
			// Channel silent
			setReg(8 + ch, 0); // Volume = 0
			continue;
		}

		// Enable tone for this channel (bits 0-2)
		mixer &= ~(1 << ch);

		// Enable noise for this channel if instrument has noise flag (bits 3-5)
		if (c.noiseEnabled) {
			mixer &= ~(1 << (ch + 3));
			// Set noise period from note (lower period = higher pitched noise)
			byte noisePeriod = (c.outputPeriod >> 4) & 0x1F;
			if (noisePeriod == 0) noisePeriod = 1;
			setReg(6, noisePeriod);
		}

		// Set tone period (2 registers per channel)
		uint16 period = (uint16)c.outputPeriod;
		setReg(ch * 2, period & 0xFF);       // Fine tune
		setReg(ch * 2 + 1, (period >> 8) & 0x0F); // Coarse tune

		// Set volume (internal 0-63 → YM 0-15)
		byte ymVol = c.volume >> 2;
		if (ymVol > 15) ymVol = 15;
		setReg(8 + ch, ymVol);
	}

	setReg(7, mixer);
}

// ---------------------------------------------------------------------------
// Main tick update — called at 50 Hz
// ---------------------------------------------------------------------------

void EclipseAtariMusicStream::tickUpdate() {
	if (!_musicActive)
		return;

	// Tick speed gating: the 68000 code uses subq.b #1; bpl; reset which
	// gives a period of (tickSpeed + 1) ticks before the sequencer advances.
	// Counter counts: tickSpeed → tickSpeed-1 → ... → 0 → -1(wrap&reset)
	_tickCounter++;
	bool sequencerTick = false;
	if (_tickCounter > _tickSpeed) {
		_tickCounter = 0;
		sequencerTick = true;
	}

	if (sequencerTick) {
		for (int ch = 0; ch < kTENumChannels; ch++) {
			if (!_channels[ch].active)
				continue;

			if (_channels[ch].durationCounter > 0) {
				_channels[ch].durationCounter--;
			}

			if (_channels[ch].durationCounter == 0) {
				if (_channels[ch].envelopePhase < 3)
					_channels[ch].envelopePhase = 3;
				readPatternCommands(ch);
			}
		}
	}

	// Every tick: process effects and envelope
	for (int ch = 0; ch < kTENumChannels; ch++) {
		if (!_channels[ch].active)
			continue;

		processEffects(ch);
		processEnvelope(ch);
	}

	writeYMRegisters();
}

// ---------------------------------------------------------------------------
// Audio stream readBuffer — tick at 50 Hz, generate AY samples
// ---------------------------------------------------------------------------

int EclipseAtariMusicStream::readBuffer(int16 *buffer, const int numSamples) {
	if (!_musicActive)
		return 0;

	int samplesGenerated = 0;
	// AY8912Stream is stereo: 2 int16 values per frame
	int samplesPerTick = (getRate() / 50) * 2;

	while (samplesGenerated < numSamples && _musicActive) {
		int remaining = samplesPerTick - _tickSampleCount;
		int toGenerate = MIN(numSamples - samplesGenerated, remaining);

		if (toGenerate > 0) {
			generateSamples(buffer + samplesGenerated, toGenerate);
			samplesGenerated += toGenerate;
			_tickSampleCount += toGenerate;
		}

		if (_tickSampleCount >= samplesPerTick) {
			_tickSampleCount -= samplesPerTick;
			tickUpdate();
		}
	}

	return samplesGenerated;
}

// ---------------------------------------------------------------------------
// Factory function
// ---------------------------------------------------------------------------

Audio::AudioStream *makeEclipseAtariMusicStream(const byte *data, uint32 dataSize,
                                                  int songNum, int rate) {
	if (!data || dataSize < 0x1000) {
		warning("TE-Atari music: invalid data (size %u)", dataSize);
		return nullptr;
	}

	EclipseAtariMusicStream *stream = new EclipseAtariMusicStream(data, dataSize, songNum, rate);
	return stream->toAudioStream();
}

} // End of namespace Freescape
