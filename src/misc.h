#ifndef MISC
#define MISC

#include <string>

#define ESC "\x1B"
#define CSI "\x1B["

struct Position
{
    unsigned int x, y;
};

struct Size
{
    unsigned int height, width;
};

struct Color
{
    enum class Foreground
    {
        Parent = 0,
        Black = 30,
        Red = 31,
        Green = 32,
        Brown = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 37,
        Default = 39
    } fg;

    enum class Background
    {
        Parent = 0,
        Black = 40,
        Red = 41,
        Green = 42,
        Brown = 43,
        Blue = 44,
        Magenta = 45,
        Cyan = 46,
        White = 47,
        Default = 49
    } bg;
};

enum class ActionResult
{
    Success,
    GetShortcut,
    Failed
};

enum class Direction
{
    Vertical,
    Horizontal
};

#endif // MISC
