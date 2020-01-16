#ifndef WINDOW
#define WINDOW

#include <unordered_map>
#include <vector>

#include "base_classes.h"
#include "misc.h"

class Window : public Container
{
    std::string window_name;

    std::vector<std::shared_ptr<Widget>> children;
    size_t active_index;

    std::unordered_map<std::string, std::function<ActionResult(Window*)>> actions;

  public:
    Window(Position _pos, Size _size, Color _color, std::string _window_name);

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

    Actionable* get_active_child()
    {
        return (active_index >= children.size()) ? nullptr : children[active_index].get();
    }

    void attach(const std::shared_ptr<Widget>& new_child);
    void attach(const std::vector<std::shared_ptr<Widget>>& new_children);
};

#endif // WINDOW
