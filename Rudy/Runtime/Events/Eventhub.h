#pragma once
#include "RudyPCH.h"

#include <cstdint>

#include "KeyCodes.h"
#include "MouseCodes.h"

namespace Rudy
{

struct WEWindowResize
{
    uint32_t width, height;
};

struct WEWindowClose
{
};

struct IEKeyPressed
{
    KeyCode key;
};

struct IEKeyReleased
{
    KeyCode key;
};

struct IEKeyRepeat
{
    KeyCode key;
};

struct IEMouseButtonPressed
{
    MouseButtonCode button;

    float x, y;
};

struct IEMouseButtonReleased
{
    MouseButtonCode button;

    float x, y;
};

struct IEMouseSroll
{
    float xOffset, yOffset;
};

struct IEMouseCursorMoved
{
    float xPos, yPos;
};

using WindowEvent = std::variant<WEWindowResize, WEWindowClose>;
using InputEvent  = std::variant<IEKeyPressed, IEKeyReleased, IEKeyRepeat, IEMouseButtonPressed, IEMouseButtonReleased, IEMouseSroll, IEMouseCursorMoved>;

} // namespace Rudy
