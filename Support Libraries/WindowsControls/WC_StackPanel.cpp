#include "WC_StackPanel.h"
#include <set>
#include <cmath>

//----------------------------------------------------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------------------------------------------------
const wchar_t* WC_StackPanel::WindowClassName = L"EX_StackPanel";

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
WC_StackPanel::WC_StackPanel(HINSTANCE moduleHandle, HWND hwnd)
:_hwnd(hwnd)
{
	_currentControlWidth = 0;
	_currentControlHeight = 0;
	_stackDirection = StackDirection::Horizontal;
	_verticalContentAlignmentDisplacement = 0.0;
	_horizontalContentAlignmentDisplacement = 0.0;
	_sizeMode = SizeMode::StackRowSize;
	_wrappingEnabled = false;
	_updatingControlSizeInternally = false;
	_verticalPadding = 0;
	_horizontalPadding = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Class registration
//----------------------------------------------------------------------------------------------------------------------
bool WC_StackPanel::RegisterWindowClass(HINSTANCE moduleHandle)
{
	// Attempt to register the window class for this control, and return the result to the
	// caller.
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = moduleHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = WindowClassName;
	wc.hIconSm       = NULL;
	ATOM registerClassExReturn = RegisterClassEx(&wc);
	return (registerClassExReturn != 0);
}

//----------------------------------------------------------------------------------------------------------------------
bool WC_StackPanel::UnregisterWindowClass(HINSTANCE moduleHandle)
{
	// Attempt to unregister the window class for this control, and return the result to
	// the caller.
	BOOL unregisterClassReturn = UnregisterClass(WindowClassName, moduleHandle);
	return (unregisterClassReturn != 0);
}

//----------------------------------------------------------------------------------------------------------------------
// Message handlers
//----------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WC_StackPanel::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	default:{
		WC_StackPanel* object = (WC_StackPanel*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (object != 0)
		{
			return object->WndProcPrivate(message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		WC_StackPanel* object = new WC_StackPanel((HINSTANCE)createStruct->lpCreateParams, hwnd);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->WndProcPrivate(message, wParam, lParam);}
	case WM_DESTROY:{
		WC_StackPanel* object = (WC_StackPanel*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (object != 0)
		{
			LPARAM result = object->WndProcPrivate(message, wParam, lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
			delete object;
			return result;
		}
		break;}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		return msgWM_CREATE(wParam, lParam);
	case WM_SIZE:
		return msgWM_SIZE(wParam, lParam);
	case WM_ERASEBKGND:
		return msgWM_ERASEBKGND(wParam, lParam);
	case WM_NCHITTEST:
		return msgWM_NCHITTEST(wParam, lParam);
	case WM_COMMAND:
		return msgWM_COMMAND(wParam, lParam);
	case WM_NOTIFY:
		return msgWM_NOTIFY(wParam, lParam);
	case WM_BOUNCE:
		return msgWM_BOUNCE(wParam, lParam);
	case (unsigned int)WindowMessages::AddWindow:
		return msgSTACK_ADDWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::InsertWindow:
		return msgSTACK_INSERTWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::RemoveWindow:
		return msgSTACK_REMOVEWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::SetDirection:
		return msgSTACK_SETDIRECTION(wParam, lParam);
	case (unsigned int)WindowMessages::SetWrappingEnabled:
		return msgSTACK_SETWRAPPINGENABLED(wParam, lParam);
	case (unsigned int)WindowMessages::SetAlignment:
		return msgSTACK_SETALIGNMENT(wParam, lParam);
	case (unsigned int)WindowMessages::SetSizeMode:
		return msgSTACK_SETSIZEMODE(wParam, lParam);
	case (unsigned int)WindowMessages::SetVerticalPadding:
		return msgSTACK_SETVERTICALPADDING(wParam, lParam);
	case (unsigned int)WindowMessages::SetHorizontalPadding:
		return msgSTACK_SETHORIZONTALPADDING(wParam, lParam);
	}

	return DefWindowProc(_hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgWM_CREATE(WPARAM wParam, LPARAM lParam)
{
	// Calculate the dimensions of the client region of this control
	RECT rect;
	GetClientRect(_hwnd, &rect);
	int newClientWidth = (int)(rect.right - rect.left);
	int newClientHeight = (int)(rect.bottom - rect.top);

	// Process the initial size of the window
	HandleSizeChanged(newClientWidth, newClientHeight);
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgWM_SIZE(WPARAM wParam, LPARAM lParam)
{
	// Calculate the dimensions of the client region of this control
	RECT rect;
	GetClientRect(_hwnd, &rect);
	int newClientWidth = (int)(rect.right - rect.left);
	int newClientHeight = (int)(rect.bottom - rect.top);

	// Handle this size changed event
	if ((_currentControlWidth != newClientWidth) || (_currentControlHeight != newClientHeight))
	{
		HandleSizeChanged(newClientWidth, newClientHeight);
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam)
{
	// If the WS_EX_TRANSPARENT window style has been applied to our control, since we want
	// this control to essentially have a transparent background, we don't perform any
	// operation when a background erase is requested. Note that this requires the
	// containing window to use the WS_EX_COMPOSITED style in order to achieve the desired
	// effect. If the WS_EX_TRANSPARENT style has not been specified, we pass this message
	// on to DefWindowProc.
	if (((unsigned int)GetWindowLongPtr(_hwnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT) != 0)
	{
		return TRUE;
	}
	return DefWindowProc(_hwnd, WM_ERASEBKGND, wParam, lParam);
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgWM_NCHITTEST(WPARAM wParam, LPARAM lParam)
{
	// Make this control transparent in the client area for hit testing
	LRESULT result = DefWindowProc(_hwnd, WM_NCHITTEST, wParam, lParam);
	return (result == HTCLIENT)? HTTRANSPARENT: result;
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgWM_COMMAND(WPARAM wParam, LPARAM lParam)
{
	// Forward this message directly to the parent window
	return SendMessage(GetAncestor(_hwnd, GA_PARENT), WM_COMMAND, wParam, lParam);
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgWM_NOTIFY(WPARAM wParam, LPARAM lParam)
{
	// Forward this message directly to the parent window
	return SendMessage(GetAncestor(_hwnd, GA_PARENT), WM_NOTIFY, wParam, lParam);
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgWM_BOUNCE(WPARAM wParam, LPARAM lParam)
{
	BounceMessage* bounceMessage = (BounceMessage*)lParam;
	if (bounceMessage->uMsg == WM_SIZE)
	{
		// If the size of a hosted window has changed, update the size of all child
		// windows.
		std::map<HWND, unsigned int>::iterator hostedWindowsIterator = _hostedWindowsSet.find(bounceMessage->hwnd);
		if (hostedWindowsIterator != _hostedWindowsSet.end())
		{
			HostedWindowInfo& windowInfo = _hostedWindows[hostedWindowsIterator->second];
			RECT rect;
			GetClientRect(windowInfo.windowHandle, &rect);
			if ((windowInfo.currentSizeX  != rect.right) || (windowInfo.currentSizeY != rect.bottom))
			{
				windowInfo.currentSizeX = rect.right;
				windowInfo.currentSizeY = rect.bottom;
				UpdateChildWindowPositions();
			}
		}
	}
	return SendMessage(GetAncestor(_hwnd, GA_PARENT), WM_BOUNCE, wParam, lParam);
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgSTACK_ADDWINDOW(WPARAM wParam, LPARAM lParam)
{
	return msgSTACK_INSERTWINDOW((unsigned int)_hostedWindows.size(), lParam);
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgSTACK_INSERTWINDOW(WPARAM wParam, LPARAM lParam)
{
	// Calculate the actual width and height of the target window
	HWND targetWindow = (HWND)lParam;
	RECT rect;
	GetClientRect(targetWindow, &rect);
	int actualWindowWidth = (int)(rect.right - rect.left);
	int actualWindowHeight = (int)(rect.bottom - rect.top);

	// Build a HostedWindowInfo object to record information on this new hosted window
	HostedWindowInfo hostedWindowInfo;
	hostedWindowInfo.windowHandle = targetWindow;
	hostedWindowInfo.currentSizeX = actualWindowWidth;
	hostedWindowInfo.currentSizeY = actualWindowHeight;

	// Retrieve the position to insert the new window within the window list
	unsigned int insertPos = (unsigned int)wParam;

	// Add the target window to the control
	return (LRESULT)InsertWindow(hostedWindowInfo, insertPos);
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgSTACK_REMOVEWINDOW(WPARAM wParam, LPARAM lParam)
{
	// Remove the target window from the control
	HWND windowHandle = (HWND)lParam;
	RemoveWindow(windowHandle);
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgSTACK_SETDIRECTION(WPARAM wParam, LPARAM lParam)
{
	// Set the new stack direction state
	_stackDirection = (StackDirection)wParam;

	// Refresh the position of each hosted control
	UpdateChildWindowPositions();
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgSTACK_SETWRAPPINGENABLED(WPARAM wParam, LPARAM lParam)
{
	// Set the new wrapping enable state
	_wrappingEnabled = (wParam != 0);

	// Refresh the position of each hosted control
	UpdateChildWindowPositions();
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgSTACK_SETALIGNMENT(WPARAM wParam, LPARAM lParam)
{
	// Set the new content alignment values
	const SetAlignmentParams& params = *((const SetAlignmentParams*)lParam);
	_horizontalContentAlignmentDisplacement = params.horizontalAlignmentDisplacement;
	_verticalContentAlignmentDisplacement = params.verticalAlignmentDisplacement;

	// Refresh the position of each hosted control
	UpdateChildWindowPositions();
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgSTACK_SETSIZEMODE(WPARAM wParam, LPARAM lParam)
{
	// Set the new size mode for the control
	_sizeMode = (SizeMode)wParam;

	// Refresh the position of each hosted control
	UpdateChildWindowPositions();
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgSTACK_SETVERTICALPADDING(WPARAM wParam, LPARAM lParam)
{
	// Set the new vertical padding for the control
	_verticalPadding = (unsigned int)wParam;

	// Refresh the position of each hosted control
	UpdateChildWindowPositions();
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT WC_StackPanel::msgSTACK_SETHORIZONTALPADDING(WPARAM wParam, LPARAM lParam)
{
	// Set the new horizontal padding for the control
	_horizontalPadding = (unsigned int)wParam;

	// Refresh the position of each hosted control
	UpdateChildWindowPositions();
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Hosted window methods
//----------------------------------------------------------------------------------------------------------------------
bool WC_StackPanel::InsertWindow(const HostedWindowInfo& windowInfo, unsigned int insertPos)
{
	// Ensure this window isn't currently hosted in our control
	std::map<HWND, unsigned int>::iterator hostedWindowsIterator = _hostedWindowsSet.find(windowInfo.windowHandle);
	if (hostedWindowsIterator != _hostedWindowsSet.end())
	{
		return false;
	}

	// Ensure the target insert position is valid
	if (insertPos > (unsigned int)_hostedWindows.size())
	{
		return false;
	}

	// Adjust the index values for the existing hosted windows
	for (std::map<HWND, unsigned int>::iterator i = _hostedWindowsSet.begin(); i != _hostedWindowsSet.end(); ++i)
	{
		if (i->second >= insertPos)
		{
			++i->second;
		}
	}

	// Record information on this new hosted window
	_hostedWindows.insert(_hostedWindows.begin() + insertPos, windowInfo);
	_hostedWindowsSet.insert(std::pair<HWND, unsigned int>(windowInfo.windowHandle, insertPos));

	// Set the target window as a child window of our control
	SetWindowParent(windowInfo.windowHandle, _hwnd);

	// Subclass the target window so we can intercept size change events
	SetWindowSubclass(windowInfo.windowHandle, BounceBackSubclassProc, 0, 0);

	// Refresh the position of each hosted control
	UpdateChildWindowPositions();
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void WC_StackPanel::RemoveWindow(HWND windowHandle)
{
	// Attempt to retrieve info on the target hosted window
	std::map<HWND, unsigned int>::iterator hostedWindowsIterator = _hostedWindowsSet.find(windowHandle);
	if (hostedWindowsIterator == _hostedWindowsSet.end())
	{
		return;
	}
	unsigned int hostedWindowsIndex = hostedWindowsIterator->second;
	const HostedWindowInfo& hostedWindowInfo = _hostedWindows[hostedWindowsIndex];

	// Remove the target window as a child window of our control
	SetWindowParent(hostedWindowInfo.windowHandle, NULL);

	// Remove the window from the list of hosted windows
	_hostedWindows.erase(_hostedWindows.begin() + hostedWindowsIndex);
	_hostedWindowsSet.erase(hostedWindowsIterator);

	// Adjust the index values for the remaining hosted windows
	for (std::map<HWND, unsigned int>::iterator i = _hostedWindowsSet.begin(); i != _hostedWindowsSet.end(); ++i)
	{
		if (i->second > hostedWindowsIndex)
		{
			--i->second;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Sizing methods
//----------------------------------------------------------------------------------------------------------------------
void WC_StackPanel::HandleSizeChanged(int newWidth, int newHeight)
{
	// If we're currently in the process of changing the size of the control internally,
	// abort any further processing.
	if (_updatingControlSizeInternally)
	{
		return;
	}

	// Save the new client window dimensions
	_currentControlWidth = newWidth;
	_currentControlHeight = newHeight;

	// Update all child window sizes now that the layout has been altered
	UpdateChildWindowPositions();
}

//----------------------------------------------------------------------------------------------------------------------
void WC_StackPanel::UpdateChildWindowPositions()
{
	// Begin a session for processing this batch of window size changes. Processing all the
	// window size and position changes in a single operation in this manner gives the best
	// performance and appearance.
	HDWP deferWindowPosSession = BeginDeferWindowPos((int)_hostedWindows.size());

	// Determine the padding values to use between stacked windows and rows
	int paddingStackDirection = (_stackDirection == StackDirection::Horizontal)? (int)_horizontalPadding: (int)_verticalPadding;
	int paddingNonStackDirection = (_stackDirection == StackDirection::Horizontal)? (int)_verticalPadding: (int)_horizontalPadding;

	// Split our list of windows into a set of rows with sizes
	unsigned int currentRowNo = 0;
	std::vector<RowInfo> rows;
	rows.push_back(RowInfo());
	int largestRowStackDirectionSize = 0;
	for (unsigned int i = 0; i < (unsigned int)_hostedWindows.size(); ++i)
	{
		// Retrieve the size of the target window
		const HostedWindowInfo& windowInfo = _hostedWindows[i];
		int windowStackDirectionSize = (_stackDirection == StackDirection::Horizontal)? windowInfo.currentSizeX: windowInfo.currentSizeY;
		int windowNonStackDirectionSize = (_stackDirection == StackDirection::Horizontal)? windowInfo.currentSizeY: windowInfo.currentSizeX;

		// If wrapping is enabled, and the stack direction isn't being sized based on the
		// content, advance to a new row if there are other windows in the current row, and
		// this window would exceed the maximum size of the row.
		if (_wrappingEnabled && ((_sizeMode == SizeMode::Fixed) || (_sizeMode == SizeMode::StackRowSize) || ((_stackDirection == StackDirection::Horizontal) && (_sizeMode == SizeMode::ContentHeight)) || ((_stackDirection == StackDirection::Vertical) && (_sizeMode == SizeMode::ContentWidth))))
		{
			int windowAvailableStackDirectionSize = (_stackDirection == StackDirection::Horizontal)? _currentControlWidth: _currentControlHeight;
			if (!rows[currentRowNo].hostedWindows.empty() && ((rows[currentRowNo].rowStackDirectionSize + paddingStackDirection + windowStackDirectionSize) > windowAvailableStackDirectionSize))
			{
				rows.push_back(RowInfo());
				++currentRowNo;
			}
		}

		// Add this window to the list of windows in the current row
		rows[currentRowNo].hostedWindows.push_back(i);

		// Update the required size of this row
		rows[currentRowNo].rowStackDirectionSize += ((!rows[currentRowNo].hostedWindows.empty())? paddingStackDirection: 0) + windowStackDirectionSize;
		rows[currentRowNo].rowNonStackDirectionSize = (rows[currentRowNo].rowNonStackDirectionSize > windowNonStackDirectionSize)? rows[currentRowNo].rowNonStackDirectionSize: windowNonStackDirectionSize;

		// Update the largest row size based on the size of the current row
		largestRowStackDirectionSize = (rows[currentRowNo].rowStackDirectionSize > largestRowStackDirectionSize)? rows[currentRowNo].rowStackDirectionSize: largestRowStackDirectionSize;
	}

	// Update the position of each window based on the calculated row sizes
	int rowNonStackDirectionCurrentDisplacement = 0;
	for (unsigned int rowNo = 0; rowNo < (unsigned int)rows.size(); ++rowNo)
	{
		// Insert padding into the start position for this row if required
		rowNonStackDirectionCurrentDisplacement += (rowNo > 0)? paddingNonStackDirection: 0;

		// Calculate the position of the first window in this row
		const RowInfo& rowInfo = rows[rowNo];
		int rowStackDirectionInitialDisplacement = (int)((double)(largestRowStackDirectionSize - rowInfo.rowStackDirectionSize) * ((_stackDirection == StackDirection::Horizontal)? _horizontalContentAlignmentDisplacement: _verticalContentAlignmentDisplacement));
		int nextWindowPosX = (_stackDirection == StackDirection::Horizontal)? rowStackDirectionInitialDisplacement: rowNonStackDirectionCurrentDisplacement;
		int nextWindowPosY = (_stackDirection == StackDirection::Horizontal)? rowNonStackDirectionCurrentDisplacement: rowStackDirectionInitialDisplacement;

		// Position each window in the row
		for (std::list<unsigned int>::const_iterator i = rowInfo.hostedWindows.begin(); i != rowInfo.hostedWindows.end(); ++i)
		{
			// Position this window
			const HostedWindowInfo& windowInfo = _hostedWindows[*i];
			DeferWindowPos(deferWindowPosSession, windowInfo.windowHandle, NULL, nextWindowPosX, nextWindowPosY, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE);

			// Calculate the position of the next window in this row
			nextWindowPosX = (_stackDirection == StackDirection::Horizontal)? nextWindowPosX + paddingStackDirection + windowInfo.currentSizeX: nextWindowPosX;
			nextWindowPosY = (_stackDirection == StackDirection::Horizontal)? nextWindowPosY: nextWindowPosY + paddingStackDirection + windowInfo.currentSizeY;
		}

		// Set the starting position for the next row
		rowNonStackDirectionCurrentDisplacement += rowInfo.rowNonStackDirectionSize;
	}

	// Process all the window size and position changes involved in this update
	EndDeferWindowPos(deferWindowPosSession);

	// Calculate the new required size of the stack panels
	int newControlWidth = _currentControlWidth;
	int newControlHeight = _currentControlHeight;
	if ((_sizeMode == SizeMode::ContentWidth) || (_sizeMode == SizeMode::ContentWidthAndHeight) || ((_sizeMode == SizeMode::StackRowSize) && (_stackDirection == StackDirection::Vertical)))
	{
		newControlWidth = (_stackDirection == StackDirection::Horizontal)? largestRowStackDirectionSize: rowNonStackDirectionCurrentDisplacement;
	}
	if ((_sizeMode == SizeMode::ContentHeight) || (_sizeMode == SizeMode::ContentWidthAndHeight) || ((_sizeMode == SizeMode::StackRowSize) && (_stackDirection == StackDirection::Horizontal)))
	{
		newControlHeight = (_stackDirection == StackDirection::Horizontal)? rowNonStackDirectionCurrentDisplacement: largestRowStackDirectionSize;
	}

	// If the new required size of the stack panel is different to the current size, resize
	// the stack panel to the required size.
	if ((newControlWidth != _currentControlWidth) || (newControlHeight != _currentControlHeight))
	{
		_updatingControlSizeInternally = true;
		SetWindowPos(_hwnd, NULL, 0, 0, newControlWidth, newControlHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
		_updatingControlSizeInternally = false;
	}
}
