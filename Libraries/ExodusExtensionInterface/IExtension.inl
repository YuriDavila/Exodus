//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IExtension::SystemMenu
{
	SYSTEMMENU_FILE      = 1000,
	SYSTEMMENU_SYSTEM    = 2000,
	SYSTEMMENU_SETTINGS  = 3000,
	SYSTEMMENU_DEBUG     = 4000,
	SYSTEMMENU_HELP      = 5000
};

//----------------------------------------------------------------------------------------
enum IExtension::ModuleMenu
{
	MODULEMENU_SETTINGS  = 3000,
	MODULEMENU_DEBUG     = 4000
};

//----------------------------------------------------------------------------------------
enum IExtension::DeviceMenu
{
	DEVICEMENU_SETTINGS  = 3000,
	DEVICEMENU_DEBUG     = 4000
};

//----------------------------------------------------------------------------------------
enum IExtension::ExtensionMenu
{
	EXTENSIONMENU_SETTINGS  = 3000,
	EXTENSIONMENU_DEBUG     = 4000
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IExtension::ThisIExtensionVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool IExtension::AddReference(const std::wstring& referenceName, IDevice* target)
{
	return AddReferenceInternal(InteropSupport::STLObjectSource<std::wstring>(referenceName), target);
}

//----------------------------------------------------------------------------------------
bool IExtension::AddReference(const std::wstring& referenceName, IExtension* target)
{
	return AddReferenceInternal(InteropSupport::STLObjectSource<std::wstring>(referenceName), target);
}

//----------------------------------------------------------------------------------------
bool IExtension::AddReference(const std::wstring& referenceName, IBusInterface* target)
{
	return AddReferenceInternal(InteropSupport::STLObjectSource<std::wstring>(referenceName), target);
}

//----------------------------------------------------------------------------------------
bool IExtension::AddReference(const std::wstring& referenceName, IClockSource* target)
{
	return AddReferenceInternal(InteropSupport::STLObjectSource<std::wstring>(referenceName), target);
}

//----------------------------------------------------------------------------------------
std::wstring IExtension::GetExtensionClassName() const
{
	std::wstring result;
	GetExtensionClassNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IExtension::GetExtensionInstanceName() const
{
	std::wstring result;
	GetExtensionInstanceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
bool IExtension::RestoreSystemViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState)
{
	return RestoreSystemViewStateInternal(InteropSupport::STLObjectSource<std::wstring>(viewGroupName), InteropSupport::STLObjectSource<std::wstring>(viewName), viewState);
}

//----------------------------------------------------------------------------------------
bool IExtension::RestoreModuleViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID)
{
	return RestoreModuleViewStateInternal(InteropSupport::STLObjectSource<std::wstring>(viewGroupName), InteropSupport::STLObjectSource<std::wstring>(viewName), viewState, moduleID);
}

//----------------------------------------------------------------------------------------
bool IExtension::RestoreDeviceViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IDevice* targetDevice)
{
	return RestoreDeviceViewStateInternal(InteropSupport::STLObjectSource<std::wstring>(viewGroupName), InteropSupport::STLObjectSource<std::wstring>(viewName), viewState, targetDevice);
}

//----------------------------------------------------------------------------------------
bool IExtension::RestoreExtensionViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IExtension* targetExtension)
{
	return RestoreExtensionViewStateInternal(InteropSupport::STLObjectSource<std::wstring>(viewGroupName), InteropSupport::STLObjectSource<std::wstring>(viewName), viewState, targetExtension);
}

//----------------------------------------------------------------------------------------
bool IExtension::OpenSystemView(const std::wstring& viewGroupName, const std::wstring& viewName)
{
	return OpenSystemViewInternal(InteropSupport::STLObjectSource<std::wstring>(viewGroupName), InteropSupport::STLObjectSource<std::wstring>(viewName));
}

//----------------------------------------------------------------------------------------
bool IExtension::OpenModuleView(const std::wstring& viewGroupName, const std::wstring& viewName, unsigned int moduleID)
{
	return OpenModuleViewInternal(InteropSupport::STLObjectSource<std::wstring>(viewGroupName), InteropSupport::STLObjectSource<std::wstring>(viewName), moduleID);
}

//----------------------------------------------------------------------------------------
bool IExtension::OpenDeviceView(const std::wstring& viewGroupName, const std::wstring& viewName, IDevice* targetDevice)
{
	return OpenDeviceViewInternal(InteropSupport::STLObjectSource<std::wstring>(viewGroupName), InteropSupport::STLObjectSource<std::wstring>(viewName), targetDevice);
}

//----------------------------------------------------------------------------------------
bool IExtension::OpenExtensionView(const std::wstring& viewGroupName, const std::wstring& viewName, IExtension* targetExtension)
{
	return OpenExtensionViewInternal(InteropSupport::STLObjectSource<std::wstring>(viewGroupName), InteropSupport::STLObjectSource<std::wstring>(viewName), targetExtension);
}