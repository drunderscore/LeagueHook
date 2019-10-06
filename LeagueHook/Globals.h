#pragma once

#include "NetworkDebug.h"
#include "Logger.h"
#include "InputManager.h"
#include "DebugOverlay.h"

namespace Globals
{
    extern Logger *Log;
    extern InputManager *Input;
    extern bool ShowMenu;
    extern NetworkDebug *NetDebug;
    extern DebugOverlay *Overlay;
}