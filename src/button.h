#ifndef BUTTON
#define BUTTON

#include <cmath>
#include <iostream>
#include <unordered_map>

#include "app.h"
#include "base_classes.h"
#include "misc.h"

class Button : public Widget
{
    std::string label;
    std::unordered_map<std::string, std::function<ActionResult(Button*)>> actions;

  public:
    Button(Position _pos, Size _size, Color _color, std::string _label);

    void draw();

    ActionResult handle_key(char) { return ActionResult::GetShortcut; }
    ActionResult handle_shortcut(std::string shortcut)
    {
        if (actions.find(shortcut) != actions.end()) {
            return actions[shortcut](this);
        } else {
            return ActionResult::Failed;
        }
    }

    Actionable* get_active_child() { return nullptr; }
};

#endif // BUTTON
