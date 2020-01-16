#ifndef GRID
#define GRID

#include <memory>
#include <unordered_map>
#include <vector>

#include "base_classes.h"
#include "misc.h"

class Grid : public Container
{
    std::vector<std::shared_ptr<Widget>> children;
    Size grid_size, element_size;
    size_t active_x, active_y;

    std::unordered_map<std::string, std::function<ActionResult(Grid*)>> actions;

  public:
    Grid(Position _pos, Size _size, Color _color, Size _grid, Size _elem);

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
        return ((active_x * grid_size.width + active_y) >= (grid_size.height * grid_size.width))
                 ? nullptr
                 : children[active_x * grid_size.width + active_y].get();
    }

    void attach(const std::shared_ptr<Widget>& new_child);
    void attach(const std::vector<std::shared_ptr<Widget>>& new_children);
    void attach_x_y(const std::shared_ptr<Widget>& new_child, size_t x, size_t y);
};

#endif // GRID
