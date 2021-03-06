#include "S315_5313Menus.h"
#include "DebugMenuHandler.h"
#include "SettingsMenuHandler.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
S315_5313Menus::S315_5313Menus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:Extension(implementationName, instanceName, moduleID)
{ }

//----------------------------------------------------------------------------------------------------------------------
S315_5313Menus::~S315_5313Menus()
{
	// Delete all menu handlers
	for (std::map<const IDevice*, DebugMenuHandler*>::const_iterator i = _debugMenuHandlers.begin(); i != _debugMenuHandlers.end(); ++i)
	{
		i->second->ClearMenuItems();
		delete i->second;
	}
	for (std::map<const IDevice*, SettingsMenuHandler*>::const_iterator i = _settingsMenuHandlers.begin(); i != _settingsMenuHandlers.end(); ++i)
	{
		i->second->ClearMenuItems();
		delete i->second;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Window functions
//----------------------------------------------------------------------------------------------------------------------
bool S315_5313Menus::RegisterDeviceMenuHandler(IDevice* targetDevice)
{
	// Attempt to cast the supplied device to the correct type
	IS315_5313* targetDeviceAsIS315_5313 = dynamic_cast<IS315_5313*>(targetDevice);
	if (targetDeviceAsIS315_5313 == 0)
	{
		return false;
	}

	// Create a new debug menu handler for the target device
	DebugMenuHandler* debugMenuHandler = new DebugMenuHandler(*this, *targetDevice, *targetDeviceAsIS315_5313);
	debugMenuHandler->LoadMenuItems();
	_debugMenuHandlers[targetDevice] = debugMenuHandler;

	// Create a new settings menu handler for the target device
	SettingsMenuHandler* settingsMenuHandler = new SettingsMenuHandler(*this, *targetDevice, *targetDeviceAsIS315_5313);
	settingsMenuHandler->LoadMenuItems();
	_settingsMenuHandlers[targetDevice] = settingsMenuHandler;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void S315_5313Menus::UnregisterDeviceMenuHandler(IDevice* targetDevice)
{
	// Delete the debug menu handler for the target device
	DebugMenuHandler* debugMenuHandler = _debugMenuHandlers[targetDevice];
	debugMenuHandler->ClearMenuItems();
	delete debugMenuHandler;
	_debugMenuHandlers.erase(targetDevice);

	// Delete the settings menu handler for the target device
	SettingsMenuHandler* settingsMenuHandler = _settingsMenuHandlers[targetDevice];
	settingsMenuHandler->ClearMenuItems();
	delete settingsMenuHandler;
	_settingsMenuHandlers.erase(targetDevice);
}

//----------------------------------------------------------------------------------------------------------------------
void S315_5313Menus::AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice)
{
	switch (deviceMenu)
	{
	case IExtension::DeviceMenu::Debug:
		_debugMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
		break;
	case IExtension::DeviceMenu::Settings:
		_settingsMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
		break;
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool S315_5313Menus::RestoreDeviceViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice)
{
	bool result = false;
	result |= _debugMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState, restoredViewPresenter);
	result |= _settingsMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState, restoredViewPresenter);
	return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool S315_5313Menus::OpenDeviceView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IDevice* targetDevice)
{
	bool result = false;
	result |= _debugMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
	result |= _settingsMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
	return result;
}
