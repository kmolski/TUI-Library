#ifndef SCREEN
#define SCREEN

#include <algorithm>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>

#include "base_classes.h"
#include "misc.h"
#include "window.h"

class Screen
  : public Drawable
  , public Actionable
{
    // This list contains the children Windows
    // with the last being the one in focus.
    std::list<std::shared_ptr<Window>> children;

    std::unordered_map<std::string, std::function<ActionResult(Screen*)>> actions;

    struct Layer
    {
        Position pos;
        Size size;
    };

    std::vector<Layer> layers;

  public:
    Screen();

    ~Screen();

    void draw();

    void attach(const std::shared_ptr<Window>& new_child);

    void attach(const std::vector<std::shared_ptr<Window>>& new_children);

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
        return (children.size() == 0) ? nullptr : children.back().get();
    }

    Actionable* get_deepest_active()
    {
        Actionable *deepest = get_active_child(), *next = nullptr;

        while (deepest && (next = deepest->get_active_child())) {
            deepest = next;
        }

        return deepest;
    }
};

#endif // SCREEN
