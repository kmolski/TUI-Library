module;

import libtui_base;

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

export module libtui_grid;

export class Grid : public Container {
    std::vector<std::shared_ptr<Widget>> children;
    Size grid_size, element_size;
    size_t active_x, active_y;

    std::unordered_map<std::string, std::function<ActionResult(Grid *)>> actions;

  public:
    Grid(Position _pos, Size _size, Color _color, Size _grid, Size _elem)
        : Container(_pos, _size, true, _color), grid_size(_grid), element_size(_elem) {
        actions = {{"UP",
                    [&](Grid *) {
                        if (active_x > 0) {
                            --active_x;
                            return ActionResult::Success;
                        } else {
                            return ActionResult::Failed;
                        }
                    }},
                   {"DOWN",
                    [&](Grid *) {
                        if (active_x < grid_size.height - 1) {
                            ++active_x;
                            return ActionResult::Success;
                        } else {
                            return ActionResult::Failed;
                        }
                    }},
                   {"LEFT",
                    [&](Grid *) {
                        if (active_y > 0) {
                            --active_y;
                            return ActionResult::Success;
                        } else {
                            return ActionResult::Failed;
                        }
                    }},
                   {"RIGHT", [&](Grid *) {
                        if (active_y < grid_size.width - 1) {
                            ++active_y;
                            return ActionResult::Success;
                        } else {
                            return ActionResult::Failed;
                        }
                    }}};
    }

    void draw() {
        Size screen_size = get_term_size();

        use_color();

        for (uint x = 0; x < size.height && pos.x + x < screen_size.height + 1; ++x) {
            std::cout << CSI << pos.x + x << ';' << pos.y << 'f';
            for (uint y = 0; y < size.width && pos.y + y < screen_size.width + 1; ++y) {
                std::cout << ' ';
            }
        }

        reset_color();

        for (auto &widget : children) {
            widget->set_active(widget == children[active_x * grid_size.width + active_y] ? active
                                                                                         : false);
            widget->set_size({element_size.height, element_size.width});
        }

        Position line_pos = get_pos();
        unsigned int remaining_x = size.height;

        for (uint x = 0; x < grid_size.height && line_pos.x < screen_size.height + 1; ++x) {
            if (line_pos.x >= pos.x + size.height - 1)
                break;

            Position current_pos = line_pos;
            unsigned int remaining_y = size.width;

            for (uint y = 0; y < grid_size.width && current_pos.y < screen_size.width + 1; ++y) {
                if (current_pos.y >= pos.y + size.width - 1)
                    break;

                Widget *child = children[x * grid_size.width + y].get();
                if (child) {
                    child->set_size({std::min(element_size.height, remaining_x),
                                     std::min(element_size.width, remaining_y)});
                    child->set_pos(current_pos);
                    child->draw();
                }
                current_pos.y += element_size.width;
                remaining_y -= element_size.width;
            }

            line_pos.x += element_size.height;
            remaining_x -= element_size.height;
        }
    }

    ActionResult handle_key(char) { return ActionResult::GetShortcut; }
    ActionResult handle_shortcut(std::string shortcut) {
        if (actions.find(shortcut) != actions.end()) {
            return actions[shortcut](this);
        } else {
            return ActionResult::Failed;
        }
    }

    Actionable *get_active_child() {
        return ((active_x * grid_size.width + active_y) >= (grid_size.height * grid_size.width))
                   ? nullptr
                   : children[active_x * grid_size.width + active_y].get();
    }

    void attach(const std::shared_ptr<Widget> &new_child) {
        children.push_back(new_child);
        new_child->set_parent(this);
        new_child->set_action_parent(this);
    }

    void attach(const std::vector<std::shared_ptr<Widget>> &new_children) {
        std::for_each(new_children.cbegin(), new_children.cend(),
                      [&](auto &new_child) { attach(new_child); });
    }

    void attach_x_y(const std::shared_ptr<Widget> &new_child, size_t x, size_t y) {
        children.insert(children.begin() + (x * grid_size.width + y), new_child);
        new_child->set_parent(this);
        new_child->set_action_parent(this);
    }
};
