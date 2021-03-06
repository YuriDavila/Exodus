#ifndef __SN76489MENUS_H__
#define __SN76489MENUS_H__
#include "Extension/Extension.pkg"
#include "SN76489/ISN76489.h"
class DebugMenuHandler;

class SN76489Menus :public Extension
{
public:
	// Constructors
	SN76489Menus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);
	~SN76489Menus();

	// Window functions
	virtual bool RegisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void UnregisterDeviceMenuHandler(IDevice* targetDevice);
	virtual void AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice);
	virtual bool RestoreDeviceViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice);
	virtual bool OpenDeviceView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IDevice* targetDevice);

private:
	std::map<IDevice*, DebugMenuHandler*> _debugMenuHandlers;
};

#endif
