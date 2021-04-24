module;

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <sys/ioctl.h>
#include <unistd.h>

export module libtui_base;

export const std::string ESC = "\x1B";
export const std::string CSI = "\x1B[";

export struct Position {
    unsigned int x, y;
};

export struct Size {
    unsigned int height, width;
};

export struct Color {
    enum class Foreground {
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

    enum class Background {
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

export enum class ActionResult { Success, GetShortcut, Failed };

export enum class Direction { Vertical, Horizontal };

export class Drawable {
  protected:
    Size size;
    Drawable *parent;
    Color color;
    bool visible;

  public:
    Drawable(Size _size, Color _color, bool _visible)
        : size(_size), parent(nullptr), color(_color), visible(_visible) {}

    virtual ~Drawable() = default;

    virtual void draw() = 0;

    Size get_size() const { return size; }
    void set_size(Size new_size) { size = new_size; }

    static Size get_term_size() {
        winsize term_size;
        ioctl(0, TIOCGWINSZ, &term_size);
        return Size{term_size.ws_row, term_size.ws_col};
    }

    Drawable *get_parent() const { return parent; }
    void set_parent(Drawable *new_parent) { parent = new_parent; }

    void use_color() {
        Color::Foreground fg_color = color.fg != Color::Foreground::Parent
                                         ? color.fg
                                         : (parent ? parent->color.fg : Color::Foreground::Default);

        Color::Background bg_color = color.bg != Color::Background::Parent
                                         ? color.bg
                                         : (parent ? parent->color.bg : Color::Background::Default);

        std::cout << CSI << int(fg_color) << ';' << int(bg_color) << 'm';
    }

    void reset_color() { std::cout << CSI << 0 << 'm'; }

    bool get_visible() const { return visible; }
    void set_visible(bool new_visible) { visible = new_visible; }
};

export class Actionable {
  protected:
    Actionable *action_parent;

  public:
    virtual ~Actionable() = default;

    virtual ActionResult handle_key(char) = 0;
    virtual ActionResult handle_shortcut(std::string) = 0;

    virtual Actionable *get_active_child() = 0;

    Actionable *get_action_parent() const { return action_parent; }
    void set_action_parent(Actionable *new_parent) { action_parent = new_parent; }
};

export class Widget : public Drawable, public Actionable {
  protected:
    Position pos;
    bool active;

  public:
    Widget(Position _pos, Size _size, bool _visible, Color _color)
        : Drawable(_size, _color, _visible), pos(_pos), active(false) {}

    virtual ~Widget() = default;

    Position get_pos() const { return pos; }
    void set_pos(Position new_pos) { pos = new_pos; }

    bool is_inside_box(Position box_pos, Size box_size) {
        return box_pos.x < pos.x && box_pos.y < pos.y &&
               box_pos.x + box_size.height > pos.x + size.height &&
               box_pos.y + box_size.width > pos.y + size.width;
    }

    bool get_active() const { return active; }
    void set_active(bool new_active) { active = new_active; }
};

export class Container : public Widget {
  public:
    Container(Position _pos, Size _size, bool _visible, Color _color)
        : Widget(_pos, _size, _visible, _color) {}

    virtual void attach(const std::shared_ptr<Widget> &) = 0;
    virtual void attach(const std::vector<std::shared_ptr<Widget>> &) = 0;
};
