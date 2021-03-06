#include "DebugMenuHandler.h"
#include "ControlViewPresenter.h"
#include "WatchpointViewPresenter.h"
#include "CallStackViewPresenter.h"
#include "TraceViewPresenter.h"
#include "DisassemblyViewPresenter.h"
#include "ActiveDisassemblyViewPresenter.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
DebugMenuHandler::DebugMenuHandler(ProcessorMenus& owner, const IDevice& modelInstanceKey, IProcessor& model)
:MenuHandlerBase(L"ProcessorDebugMenu", owner.GetViewManager()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Management functions
//----------------------------------------------------------------------------------------------------------------------
void DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	if (_model.ActiveDisassemblySupported())
	{
		menuItems.push_back(MenuItemDefinition(MENUITEM_ACTIVEDISASSEMBLY, L"ActiveDisassembly", ActiveDisassemblyViewPresenter::GetUnqualifiedViewTitle(), true, true));
	}
	menuItems.push_back(MenuItemDefinition(MENUITEM_CALLSTACK, L"CallStack", CallStackViewPresenter::GetUnqualifiedViewTitle(), true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_DISASSEMBLY, L"Disassembly", DisassemblyViewPresenter::GetUnqualifiedViewTitle(), true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_CONTROL, L"ProcessorControl", ControlViewPresenter::GetUnqualifiedViewTitle(), true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_TRACE, L"Trace", TraceViewPresenter::GetUnqualifiedViewTitle(), true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_WATCHPOINTS, L"Watchpoints", WatchpointViewPresenter::GetUnqualifiedViewTitle(), true));
}

//----------------------------------------------------------------------------------------------------------------------
IViewPresenter* DebugMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch (menuItemID)
	{
	case MENUITEM_CONTROL:
		return new ControlViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	case MENUITEM_WATCHPOINTS:
		return new WatchpointViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	case MENUITEM_CALLSTACK:
		return new CallStackViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	case MENUITEM_TRACE:
		return new TraceViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	case MENUITEM_DISASSEMBLY:
		return new DisassemblyViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	case MENUITEM_ACTIVEDISASSEMBLY:
		return new ActiveDisassemblyViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void DebugMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
