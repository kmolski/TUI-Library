#ifndef SPLIT
#define SPLIT

#include <cmath>
#include <unordered_map>
#include <vector>

#include "base_classes.h"
#include "misc.h"

class Split : public Widget
{
    Direction dir;
    double fraction;

    std::shared_ptr<Widget> first, second;
    enum class Active
    {
        None,
        First,
        Second
    } current_active;

    std::unordered_map<std::string, std::function<ActionResult(Split*)>> actions;

  public:
    Split(Position _pos,
          Size _size,
          Direction _dir,
          double _fraction,
          const std::shared_ptr<Widget>& _first,
          const std::shared_ptr<Widget>& _second);

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
        switch (current_active) {
            case Active::First:
                return first.get();
            case Active::Second:
                return second.get();
            default:
                return nullptr;
        }
    }
};

#endif // SPLIT
