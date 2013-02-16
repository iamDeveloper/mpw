#include "defs.h"
#include "fmem.h"
#include "CpuModule.h"


/*============================================================================*/
/* Illegal read / write fault information                                     */
/*============================================================================*/

BOOLE memory_fault_read = FALSE;                       /* TRUE - read / FALSE - write */
ULO memory_fault_address = 0;

/*==============================================================================
Raises exception 3 when a word or long is accessing an odd address
and the CPU is < 020
==============================================================================*/

static void memoryOddRead(ULO address)
{
	if (address & 1)
	{
		if (cpuGetModelMajor() < 2)
		{
			memory_fault_read = TRUE;
			memory_fault_address = address;
			cpuThrowAddressErrorException();
		}
	}
}

static void memoryOddWrite(ULO address)
{
	if (address & 1)
	{
		if (cpuGetModelMajor() < 2)
		{
			memory_fault_read = FALSE;
			memory_fault_address = address;
			cpuThrowAddressErrorException();
		}
	}
}


// new memory functions.

static uint8_t *Memory = NULL;
static uint32_t MemorySize = 0;
static uint32_t MemoryGlobalLog = 0;

void memorySetMemory(uint8_t *memory, uint32_t size)
{
	Memory = memory;
	MemorySize = size;
}

void memorySetGlobalLog(uint32_t globalLog)
{
	MemoryGlobalLog = globalLog;
}


uint8_t *memoryPointer(uint32_t address)
{
	return Memory + address;
}

// memory read of 0xffffffff not handled correctly
// since the unsigned compare overflows.
UBY memoryReadByte(ULO address)
{

	if (address < MemoryGlobalLog)
	{
		fprintf(stderr, "memoryReadByte(%08x)\n", address);
	}

	// hmmm... 32-bit clean addresses?
	if (address < MemorySize) 
		return Memory[address];
	return 0;
}

UWO memoryReadWord(ULO address)
{

	if (address < MemoryGlobalLog)
	{
		fprintf(stderr, "memoryReadWord(%08x)\n", address);
	}
	

	if (address & 0x01) memoryOddRead(address);

	if (address + 1 < MemorySize)
		return (Memory[address++] << 8) 
			| (Memory[address++] << 0); 

	return 0;
}

ULO memoryReadLong(ULO address)
{
	if (address < MemoryGlobalLog)
	{
		fprintf(stderr, "memoryReadLong(%08x)\n", address);
	}

	if (address & 0x01) memoryOddRead(address);

	if (address + 1 < MemorySize)
		return (Memory[address++] << 24) 
			| (Memory[address++] << 16)
			| (Memory[address++] << 8)
			| (Memory[address++] << 0); 

	return 0;

}
void memoryWriteByte(UBY data, ULO address)
{
	if (address < MemoryGlobalLog)
	{
		fprintf(stderr, "memoryWriteByte(%02x, %08x)\n", data, address);
	}

	if (address < MemorySize)
	{
		Memory[address] = data;
	}
}

void memoryWriteWord(UWO data, ULO address)
{

	if (address < MemoryGlobalLog)
	{
		fprintf(stderr, "memoryWriteWord(%04x, %08x)\n", data, address);
	}

	if (address & 0x01) memoryOddWrite(address);

	if (address + 1 < MemorySize)
	{
		Memory[address++] = data >> 8;
		Memory[address++] = data >> 0;
	}
}

void memoryWriteLong(ULO data, ULO address)
{

	if (address < MemoryGlobalLog)
	{
		fprintf(stderr, "memoryWriteLong(%08x, %08x)\n", data, address);
	}


	if (address & 0x01) memoryOddWrite(address);

	if (address + 3 < MemorySize)
	{
		Memory[address++] = data >> 24;
		Memory[address++] = data >> 16;
		Memory[address++] = data >> 8;
		Memory[address++] = data >> 0;
	}
}


