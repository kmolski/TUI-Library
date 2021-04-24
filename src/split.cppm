module;

import libtui_base;

#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

export module libtui_split;

export class Split : public Widget {
    Direction dir;
    double fraction;

    std::shared_ptr<Widget> first, second;
    enum class Active { None, First, Second } current_active;

    std::unordered_map<std::string, std::function<ActionResult(Split *)>> actions;

  public:
    Split(Position _pos, Size _size, Direction _dir, double _fraction,
          const std::shared_ptr<Widget> &_first, const std::shared_ptr<Widget> &_second)
        : Widget(_pos, _size, true, {Color::Foreground::Default, Color::Background::Default}),
          dir(_dir), fraction(_fraction), first(_first), second(_second),
          current_active(Active::First) {
        first->set_action_parent(this);
        second->set_action_parent(this);
        actions = {{"UP",
                    [&](Split *) {
                        if (dir == Direction::Vertical && current_active == Active::Second) {
                            current_active = Active::First;
                            return ActionResult::Success;
                        } else {
                            return ActionResult::Failed;
                        }
                    }},
                   {"DOWN",
                    [&](Split *) {
                        if (dir == Direction::Vertical && current_active == Active::First) {
                            current_active = Active::Second;
                            return ActionResult::Success;
                        } else {
                            return ActionResult::Failed;
                        }
                    }},
                   {"LEFT",
                    [&](Split *) {
                        if (dir == Direction::Horizontal && current_active == Active::Second) {
                            current_active = Active::First;
                            return ActionResult::Success;
                        } else {
                            return ActionResult::Failed;
                        }
                    }},
                   {"RIGHT", [&](Split *) {
                        if (dir == Direction::Horizontal && current_active == Active::First) {
                            current_active = Active::Second;
                            return ActionResult::Success;
                        } else {
                            return ActionResult::Failed;
                        }
                    }}};
    }

    void draw() {
        switch (current_active) {
            case Active::None:
            case Active::First:
                first->set_active(active);
                second->set_active(false);
                break;
            case Active::Second:
                second->set_active(active);
                first->set_active(false);
                break;
        }

        switch (dir) {
            case Direction::Vertical: {
                unsigned int first_height = std::round(fraction * size.height);
                first->set_size({first_height, size.width});
                first->set_pos(pos);
                second->set_size({size.height - first_height, size.width});
                second->set_pos({pos.x + first_height, pos.y});
                break;
            }
            case Direction::Horizontal:
                unsigned int first_width = std::round(fraction * size.width);
                first->set_size({size.height, first_width});
                first->set_pos(pos);
                second->set_size({size.height, size.width - first_width});
                second->set_pos({pos.x, pos.y + first_width});
                break;
        }

        first->draw();
        second->draw();
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
