#ifndef LIST
#define LIST

#include <cmath>

#include "app.h"
#include "base_classes.h"
#include "misc.h"

class List : public Container
{
    Direction dir;

    std::vector<std::shared_ptr<Widget>> children;
    size_t active_index;

    std::unordered_map<std::string, std::function<ActionResult(List*)>> actions;

  public:
    List(Position _pos, Size _size, Color _color, Direction _dir);

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

#endif // LIST
