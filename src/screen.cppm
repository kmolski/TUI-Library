module;

import libtui_base;
import libtui_window;

#include <algorithm>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

export module libtui_screen;

export class Screen : public Drawable, public Actionable {
    // This list contains the children windows with the last being the one in focus.
    std::list<std::shared_ptr<Window>> children;
    std::unordered_map<std::string, std::function<ActionResult(Screen *)>> actions;

    struct Layer {
        Position pos;
        Size size;
    };

    std::vector<Layer> layers;

  public:
    Screen() : Drawable({0, 0}, {Color::Foreground::Default, Color::Background::Default}, true) {
        system("stty raw -echo");

        actions = {{"CTRL+Q",
                    [](Screen *) {
                        std::exit(0);
                        return ActionResult::Success;
                    }},
                   {"CTRL+F", [&](Screen *) {
                        auto elem = children.front();
                        children.pop_front();
                        children.push_back(elem);
                        return ActionResult::Success;
                    }}};
    }

    ~Screen() {
        std::cout << CSI << "2J"; // Clear the whole screen
        system("stty -raw");
    }

    void draw() {
        // Clear the whole screen and hide the cursor
        std::cout << CSI << "2J" << CSI << "?25l";

        // First, widgets that are completely covered by
        // other widgets should be set to invisible
        layers.clear();

        std::for_each(children.rbegin(), children.rend(), [&](auto &child) {
            child->set_visible(std::none_of(layers.cbegin(), layers.cend(), [&](auto layer) {
                return child->is_inside_box(layer.pos, layer.size);
            }));

            if (child->get_visible()) {
                layers.push_back({child->get_pos(), child->get_size()});
            }

            child->set_active(child == children.back() ? true : false);
        });

        // Next, all the elements should be drawn from top to bottom
        for (auto &child : children) {
            if (child->get_visible()) {
                child->draw();
            }
        }

        // Make the cursor visible again
        std::cout << CSI << "?25h";
        std::cout.flush();
    }

    void attach(const std::shared_ptr<Window> &new_child) {
        children.push_back(new_child);
        new_child->set_parent(this);
        new_child->set_action_parent(this);
    }

    void attach(const std::vector<std::shared_ptr<Window>> &new_children) {
        std::for_each(new_children.cbegin(), new_children.cend(),
                      [&](auto &new_child) { attach(new_child); });
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
        return (children.size() == 0) ? nullptr : children.back().get();
    }

    Actionable *get_deepest_active() {
        Actionable *deepest = get_active_child(), *next = nullptr;

        while (deepest && (next = deepest->get_active_child())) {
            deepest = next;
        }

        return deepest;
    }
};
