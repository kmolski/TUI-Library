export module libtui_list;

#include <cmath>
#include <iostream>
#include <unordered_map>
#include <vector>

import libtui_base;

export class List : public Container {
    Direction dir;

    std::vector<std::shared_ptr<Widget>> children;
    size_t active_index;

    std::unordered_map<std::string, std::function<ActionResult(List *)>> actions;

  public:
    List(Position _pos, Size _size, Color _color, Direction _dir)
        : Container(_pos, _size, true, _color), dir(_dir) {
        actions = {{"UP",
                    [&](List *) {
                        if (dir == Direction::Vertical && active_index > 0) {
                            active_index = active_index - 1;
                            return ActionResult::Success;
                        } else {
                            return ActionResult::Failed;
                        }
                    }},
                   {"DOWN",
                    [&](List *) {
                        if (dir == Direction::Vertical && active_index < children.size() - 1) {
                            active_index = active_index + 1;
                            return ActionResult::Success;
                        } else {
                            return ActionResult::Failed;
                        }
                    }},
                   {"LEFT",
                    [&](List *) {
                        if (dir == Direction::Horizontal && active_index > 0) {
                            active_index = active_index - 1;
                            return ActionResult::Success;
                        } else {
                            return ActionResult::Failed;
                        }
                    }},
                   {"RIGHT", [&](List *) {
                        if (dir == Direction::Horizontal && active_index < children.size() - 1) {
                            active_index = active_index + 1;
                            return ActionResult::Success;
                        } else {
                            return ActionResult::Failed;
                        }
                    }}};
    }

    void draw() {
        Size screen_size = parent->get_size();

        use_color();

        for (uint x = 0; x < size.height && pos.x + x < screen_size.height + 1; ++x) {
            std::cout << CSI << pos.x + x << ';' << pos.y << 'f';
            for (uint y = 0; y < size.width && pos.y + y < screen_size.width + 1; ++y) {
                std::cout << ' ';
            }
        }

        reset_color();

        for (auto &widget : children) {
            widget->set_active(widget == children[active_index] ? active : false);
            switch (dir) {
                case Direction::Vertical:
                    widget->set_size({widget->get_size().height, size.width});
                    break;
                case Direction::Horizontal:
                    widget->set_size({size.height, widget->get_size().width});
                    break;
            }
        }

        Position current = get_pos();
        unsigned int remaining = dir == Direction::Vertical ? size.height : size.width;

        for (auto &widget : children) {
            widget->set_pos(current);
            Size widget_size = widget->get_size();
            switch (dir) {
                case Direction::Vertical:
                    widget->set_size({std::min(widget_size.height, remaining), widget_size.width});
                    break;
                case Direction::Horizontal:
                    widget->set_size({widget_size.height, std::min(widget_size.width, remaining)});
                    break;
            }
            widget->draw();
            switch (dir) {
                case Direction::Vertical:
                    current.x += widget->get_size().height;
                    if (current.x >= pos.x + size.height - 1)
                        return;
                    remaining -= widget->get_size().height;
                    break;
                case Direction::Horizontal:
                    current.y += widget->get_size().width;
                    if (current.y >= pos.y + size.width - 1)
                        return;
                    remaining -= widget->get_size().width;
                    break;
            }
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
        return (active_index >= children.size()) ? nullptr : children[active_index].get();
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
};
