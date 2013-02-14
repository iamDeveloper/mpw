#include <cstdio>
#include <cstdint>
#include <stdlib.h>
#include <string>

#include <cpu/defs.h>
#include <cpu/CpuModule.h>
#include <cpu/fmem.h>

#include "toolbox.h"
#include "rm.h"
#include "mm.h"
#include "os.h"
#include "mpw_time.h"


// yuck.  TST.W d0
extern "C" void cpuSetFlagsNZ00NewW(UWO res);

namespace ToolBox {





	void dispatch(uint16_t trap)
	{
		// todo -- check/remove extra bits for save a0, toolglue, etc.

		uint32_t d0 = 0;
		switch (trap)
		{
			case 0xa008:
				d0 = OS::Create(trap);
				break;

			case 0xA00C:
				d0 = OS::GetFileInfo(trap);
				break;
			case 0xa00d:
				d0 = OS::SetFileInfo(trap);
				break;

			case 0xa014:
				d0 = OS::GetVol(trap);
				break;

			// BlockMove (sourcePtr,destPtr: Ptr; byteCount: Size);
			case 0xa02e:
				d0 = MM::BlockMove(trap);
				break;


			// ReadDateTime (VAR sees: LONGINT) : OSErr;
			case 0xa039:
				d0 = Time::ReadDateTime(trap);
				break;

			// SecondsToDate (s: LongInt; VAR d: DateTimeRec);
			case 0xa9c6:
				d0 = Time::SecondsToDate(trap);
				break;
		
			// NewPtr [Sys, Clear] (logicalSize: Size): Ptr;
			case 0xa11e:
			case 0xa31e:
			case 0xa51e:
			case 0xa71e:
				// clear = bit 9, sys = bit 10
				d0 = MM::NewPtr(trap);
				break;

			// DisposPtr (p: Ptr);
			case 0xa01f:
				d0 = MM::DisposePtr(trap);
				break;

			// Get1NamedResource (theType: ResType; name: Str255) : Handle;
			case 0xa820:
				d0 = RM::Get1NamedResource(trap);
				break;

			default:
				fprintf(stderr, "Unsupported tool trap: %04x\n", trap);
				fprintf(stderr, "pc: %08x\n", cpuGetPC());
				exit(255);
		}

		cpuSetDReg(0, d0);
		cpuSetFlagsNZ00NewW(d0);
	}

	std::string ReadCString(uint32_t address)
	{
		std::string tmp;

		if (address)
		{
			tmp.assign((char *)memoryPointer(address));
		}

		return tmp;
	}
	
	std::string ReadPString(uint32_t address)
	{
		std::string tmp;

		if (address)
		{
			unsigned length = memoryReadByte(address);
		
			tmp.assign((char *)memoryPointer(address + 1), length);
		}

		return tmp;
	}

	bool WritePString(uint32_t address, const std::string &s)
	{
		int length = s.length();
		if (length > 255) return false;
		if (address == 0) return false;

		uint8_t *ptr = memoryPointer(address);
		*ptr++  = (uint8_t)length;
		for (char c : s)
		{
			*ptr++ = (uint8_t)c;
		}
		return true;
	}


}