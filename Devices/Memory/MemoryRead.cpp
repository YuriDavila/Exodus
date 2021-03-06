#include "MemoryRead.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
MemoryRead::MemoryRead(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:Device(implementationName, instanceName, moduleID), _memoryEntryCount(0)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int MemoryRead::GetIMemoryVersion() const
{
	return ThisIMemoryVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Initialization functions
//----------------------------------------------------------------------------------------------------------------------
bool MemoryRead::Construct(IHierarchicalStorageNode& node)
{
	// Read interface size
	IHierarchicalStorageAttribute* memoryEntryCountAttribute = node.GetAttribute(L"MemoryEntryCount");
	if (memoryEntryCountAttribute != 0)
	{
		SetMemoryEntryCount(memoryEntryCountAttribute->ExtractHexValue<unsigned int>());
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Memory size functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int MemoryRead::GetMemoryEntryCount() const
{
	return _memoryEntryCount;
}

//----------------------------------------------------------------------------------------------------------------------
void MemoryRead::SetMemoryEntryCount(unsigned int memoryEntryCount)
{
	_memoryEntryCount = memoryEntryCount;
}

//----------------------------------------------------------------------------------------------------------------------
// Memory locking functions
//----------------------------------------------------------------------------------------------------------------------
bool MemoryRead::IsMemoryLockingSupported() const
{
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
void MemoryRead::LockMemoryBlock(unsigned int location, unsigned int size, bool state)
{ }

//----------------------------------------------------------------------------------------------------------------------
bool MemoryRead::IsAddressLocked(unsigned int location) const
{
	return false;
}
