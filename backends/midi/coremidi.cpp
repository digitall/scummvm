/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

// Disable symbol overrides so that we can use system headers.
#define FORBIDDEN_SYMBOL_ALLOW_ALL

#include "common/scummsys.h"

#ifdef MACOSX

#include "common/config-manager.h"
#include "common/debug.h"
#include "common/error.h"
#include "common/str-array.h"
#include "common/textconsole.h"
#include "common/util.h"
#include "audio/musicplugin.h"
#include "audio/mpu401.h"

#include <CoreMIDI/CoreMIDI.h>

#include <AvailabilityMacros.h>

// kMIDIPropertyDisplayName is not defined prior to OSX 10.4 API, so use
// fallback of string concatenation of Device-Entity-Endpoint
#if !defined(USE_FALLBACK_COREMIDI_API)
	#if TARGET_CPU_PPC || TARGET_CPU_PPC64 || !defined(MAC_OS_X_VERSION_10_4)
		#define USE_FALLBACK_COREMIDI_API 1
	#else
		#define USE_FALLBACK_COREMIDI_API 0
	#endif
#endif

/*
For information on how to unify the CoreMidi and MusicDevice code:

http://lists.apple.com/archives/Coreaudio-api/2005/Jun/msg00194.html
http://lists.apple.com/archives/coreaudio-api/2003/Mar/msg00248.html
http://lists.apple.com/archives/coreaudio-api/2003/Jul/msg00137.html

*/


/* CoreMIDI MIDI driver
 * By Max Horn
 */
class MidiDriver_CoreMIDI : public MidiDriver_MPU401 {
public:
	MidiDriver_CoreMIDI(int deviceIndex);
	~MidiDriver_CoreMIDI();
	int open();
	bool isOpen() const { return mOutPort != 0 && mDest != 0; }
	void close();
	void send(uint32 b);
	void sysEx(const byte *msg, uint16 length);

private:
	int _device;

	MIDIClientRef	mClient;
	MIDIPortRef		mOutPort;
	MIDIEndpointRef	mDest;
};

MidiDriver_CoreMIDI::MidiDriver_CoreMIDI(int deviceIndex)
	: _device(deviceIndex), mClient(0), mOutPort(0), mDest(0) {

	OSStatus err;
	err = MIDIClientCreate(CFSTR("ScummVM MIDI Driver for OS X"), NULL, NULL, &mClient);
}

MidiDriver_CoreMIDI::~MidiDriver_CoreMIDI() {
	if (mClient)
		MIDIClientDispose(mClient);
	mClient = 0;
}

int MidiDriver_CoreMIDI::open() {
	if (isOpen())
		return MERR_ALREADY_OPEN;

	OSStatus err = noErr;

	mOutPort = 0;

	int dests = MIDIGetNumberOfDestinations();
	if (dests > _device && mClient) {
		mDest = MIDIGetDestination(_device);
		err = MIDIOutputPortCreate( mClient,
									CFSTR("scummvm_output_port"),
									&mOutPort);
	} else {
		return MERR_DEVICE_NOT_AVAILABLE;
	}

	if (err != noErr)
		return MERR_CANNOT_CONNECT;

	return 0;
}

void MidiDriver_CoreMIDI::close() {
	MidiDriver_MPU401::close();

	if (isOpen()) {
		MIDIPortDispose(mOutPort);
		mOutPort = 0;
		mDest = 0;
	}
}

void MidiDriver_CoreMIDI::send(uint32 b) {
	if (!isOpen()) {
		warning("MidiDriver_CoreMIDI: Got event while not open");
		return;
	}

	// Extract the MIDI data
	byte status_byte = (b & 0x000000FF);
	byte first_byte = (b & 0x0000FF00) >> 8;
	byte second_byte = (b & 0x00FF0000) >> 16;

	// Generate a single MIDI packet with that data
	MIDIPacketList packetList;
	MIDIPacket *packet = &packetList.packet[0];

	packetList.numPackets = 1;

	packet->timeStamp = 0;
	packet->data[0] = status_byte;
	packet->data[1] = first_byte;
	packet->data[2] = second_byte;

	// Compute the correct length of the MIDI command. This is important,
	// else things may screw up badly...
	switch (status_byte & 0xF0) {
	case 0x80:	// Note Off
	case 0x90:	// Note On
	case 0xA0:	// Polyphonic Aftertouch
	case 0xB0:	// Controller Change
	case 0xE0:	// Pitch Bending
		packet->length = 3;
		break;
	case 0xC0:	// Programm Change
	case 0xD0:	// Monophonic Aftertouch
		packet->length = 2;
		break;
	default:
		warning("CoreMIDI driver encountered unsupported status byte: 0x%02x", status_byte);
		packet->length = 3;
		break;
	}

	// Finally send it out to the synthesizer.
	MIDISend(mOutPort, mDest, &packetList);
}

void MidiDriver_CoreMIDI::sysEx(const byte *msg, uint16 length) {
	if (!isOpen()) {
		warning("MidiDriver_CoreMIDI: Got SysEx while not open");
		return;
	}

	byte buf[384];
	MIDIPacketList *packetList = (MIDIPacketList *)buf;
	MIDIPacket *packet = packetList->packet;

	assert(sizeof(buf) >= sizeof(UInt32) + sizeof(MIDITimeStamp) + sizeof(UInt16) + length + 2);

	packetList->numPackets = 1;

	packet->timeStamp = 0;

	// Add SysEx frame
	packet->length = length + 2;
	packet->data[0] = 0xF0;
	memcpy(packet->data + 1, msg, length);
	packet->data[length + 1] = 0xF7;

	// Send it
	MIDISend(mOutPort, mDest, packetList);
}


// Plugin interface

class CoreMIDIMusicPlugin : public MusicPluginObject {
public:
	const char *getName() const {
		return "CoreMIDI";
	}

	const char *getId() const {
		return "coremidi";
	}

	MusicDevices getDevices() const;
	Common::Error createInstance(MidiDriver **mididriver, MidiDriver::DeviceHandle = 0) const;
};

// Obtain the name of an endpoint, following connections.
// The result should be released by the caller.
static CFStringRef ConnectedEndpointName(MIDIEndpointRef endpoint)
{
	CFMutableStringRef result = CFStringCreateMutable(NULL, 0);
	CFStringRef str;
	OSStatus err;

	// Does the endpoint have connections?
	CFDataRef connections = NULL;
	int nConnected = 0;
	bool anyStrings = false;
	err = MIDIObjectGetDataProperty(endpoint, kMIDIPropertyConnectionUniqueID, &connections);
	if (connections != NULL) {
		// It has connections, follow them
		// Concatenate the names of all connected devices
		nConnected = CFDataGetLength(connections) / sizeof(MIDIUniqueID);
		if (nConnected) {
			const SInt32 *pid = reinterpret_cast<const SInt32 *>(CFDataGetBytePtr(connections));
			for (int i = 0; i < nConnected; ++i, ++pid) {
				MIDIUniqueID id = EndianS32_BtoN(*pid);
				MIDIObjectRef connObject;
				MIDIObjectType connObjectType;
				err = MIDIObjectFindByUniqueID(id, &connObject, &connObjectType);
				if (err == noErr) {
					if (connObjectType == kMIDIObjectType_ExternalSource  || connObjectType == kMIDIObjectType_ExternalDestination) {
						// Connected to an external device's endpoint (10.3 and later).
						str = EndpointName(static_cast<MIDIEndpointRef>(connObject), true);
					} else {
						// Connected to an external device (10.2) (or something else, catch-all)
						str = NULL;
						MIDIObjectGetStringProperty(connObject, kMIDIPropertyName, &str);
					}
					if (str != NULL) {
						if (anyStrings)
							CFStringAppend(result, CFSTR(", "));
						else anyStrings = true;
						CFStringAppend(result, str);
						CFRelease(str);
					}
				}
			}
		}
		CFRelease(connections);
	}
	if (anyStrings)
		return result;

	// Here, either the endpoint had no connections, or we failed to obtain names for any of them.
	return EndpointName(endpoint, false);
}

// Obtain the name of an endpoint without regard for whether it has connections.
// The result should be released by the caller.
static CFStringRef EndpointName(MIDIEndpointRef endpoint, bool isExternal)
{
	CFMutableStringRef result = CFStringCreateMutable(NULL, 0);
	CFStringRef str;

	// begin with the endpoint's name
	str = NULL;
	MIDIObjectGetStringProperty(endpoint, kMIDIPropertyName, &str);
	if (str != NULL) {
		CFStringAppend(result, str);
		CFRelease(str);
	}

	MIDIEntityRef entity = NULL;
	MIDIEndpointGetEntity(endpoint, &entity);
	if (entity == NULL)
		// probably virtual
		return result;

	if (CFStringGetLength(result) == 0) {
		// endpoint name has zero length -- try the entity
		str = NULL;
		MIDIObjectGetStringProperty(entity, kMIDIPropertyName, &str);
		if (str != NULL) {
			CFStringAppend(result, str);
			CFRelease(str);
		}
	}
	// now consider the device's name
	MIDIDeviceRef device = NULL;
	MIDIEntityGetDevice(entity, &device);
	if (device == NULL)
		return result;

	str = NULL;
	MIDIObjectGetStringProperty(device, kMIDIPropertyName, &str);
	if (str != NULL) {
		// if an external device has only one entity, throw away 
		// the endpoint name and just use the device name
		if (isExternal && MIDIDeviceGetNumberOfEntities(device) < 2) {
			CFRelease(result);
			return str;
		} else {
			// does the entity name already start with the device name? 
			// (some drivers do this though they shouldn't)
			// if so, do not prepend
			if (CFStringCompareWithOptions(str /* device name */, 
			                               result /* endpoint name */, 
			                               CFRangeMake(0, CFStringGetLength(str)), 0) != kCFCompareEqualTo) {
				// prepend the device name to the entity name
				if (CFStringGetLength(result) > 0)
					CFStringInsert(result, 0, CFSTR(" "));
				CFStringInsert(result, 0, str);
			}
			CFRelease(str);
		}
	}
	return result;
}

MusicDevices CoreMIDIMusicPlugin::getDevices() const {
	MusicDevices devices;
	int dests = MIDIGetNumberOfDestinations();
	debug(1, "CoreMIDI driver found %d destinations:", dests);

	Common::StringArray deviceNames;
	// TODO: Return a different music type depending on the configuration

	// List the available devices
	for(int i = 0; i < dests; i++) {
		MIDIEndpointRef dest = MIDIGetDestination(i);
		Common::String destname = "Unknown / Invalid";
		if (dest) {
			CFStringRef midiname = 0;
#if USE_FALLBACK_COREMIDI_API
			// TODO: kMIDIPropertyDisplayName was only added in 10.4 API, so need to use fallback solution
			//       for PPC which uses 10.2 API.
#else
			if(MIDIObjectGetStringProperty(dest, kMIDIPropertyDisplayName, &midiname) == noErr) {
				const char *s = CFStringGetCStringPtr(midiname, kCFStringEncodingMacRoman);
				if (s) {
					destname = Common::String(s);
					deviceNames.push_back(Common::String(s));
				}
			}
#endif
		}
		debug(1, "\tDestination %d: %s", i, destname.c_str());
	}

	for (Common::StringArray::iterator i = deviceNames.begin(); i != deviceNames.end(); ++i)
		// There is no way to detect the "MusicType" so I just set it to MT_GM
		// The user will have to manually select his MT32 type device and his GM type device.
		devices.push_back(MusicDevice(this, *i, MT_GM));

	return devices;
}

Common::Error CoreMIDIMusicPlugin::createInstance(MidiDriver **mididriver, MidiDriver::DeviceHandle dev) const {
	int devIndex = 0;
	bool found = false;

	if (dev) {
		MusicDevices i = getDevices();
		for (MusicDevices::iterator d = i.begin(); d != i.end(); d++) {
			if (d->getCompleteId().equals(MidiDriver::getDeviceString(dev, MidiDriver::kDeviceId))) {
				found = true;
				break;
			}
			devIndex++;
		}
	}

	*mididriver = new MidiDriver_CoreMIDI(found ? devIndex : 0);
	return Common::kNoError;
}

//#if PLUGIN_ENABLED_DYNAMIC(COREMIDI)
	//REGISTER_PLUGIN_DYNAMIC(COREMIDI, PLUGIN_TYPE_MUSIC, CoreMIDIMusicPlugin);
//#else
	REGISTER_PLUGIN_STATIC(COREMIDI, PLUGIN_TYPE_MUSIC, CoreMIDIMusicPlugin);
//#endif

#endif // MACOSX
