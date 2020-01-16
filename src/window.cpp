#include <cmath>

#include "app.h"
#include "list.h"
#include "window.h"

Window::Window(Position _pos, Size _size, Color _color, std::string _window_name)
  : Container(_pos, _size, true, _color)
  , window_name(_window_name)
{
    actions = {
        { "CTRL+UP",
          [&](Window*) {
              pos.x = std::max(1u, pos.x - 1);
              return ActionResult::Success;
          } },
        { "CTRL+DOWN",
          [&](Window*) {
              pos.x = std::min(Application::get_screen_size().height, pos.x + 1);
              return ActionResult::Success;
          } },
        { "CTRL+LEFT",
          [&](Window*) {
              pos.y = std::max(1u, pos.y - 1);
              return ActionResult::Success;
          } },
        { "CTRL+RIGHT",
          [&](Window*) {
              pos.y = std::min(Application::get_screen_size().width, pos.y + 1);
              return ActionResult::Success;
          } },
    };
}

void
Window::draw()
{
    use_color();

    if (active) {
        std::cout << CSI "1m";
    }

    Size screen_size = Application::get_screen_size();

    // Move the cursor to the beginning of the widget
    std::cout << CSI << pos.x << ';' << pos.y << 'f';

    // Format the title bar, truncate the window name if it's too long
    std::string title_text =
      "+ " + (window_name.length() < size.width - 3
                ? window_name + " "
                : window_name.substr(0, std::max(0, int(size.width - 7))) + "... ");

    // Fill the rest of the title bar with dashes and a corner
    for (uint i = 0; i < (size.width - 4 - window_name.length()); ++i) {
        title_text += '-';
    }

    title_text += '+';

    // Draw the title bar without spilling into the next line
    if (pos.x <= screen_size.height) {
        std::cout << title_text.substr(0, std::max(0, int(screen_size.width - pos.y + 1)));
    }

    // Draw the background and side borders
    for (uint x = 1; x < size.height - 1 && pos.x + x < screen_size.height + 1; ++x) {
        std::cout << CSI << pos.x + x << ';' << pos.y << 'f';
        for (uint y = 0; y < size.width && pos.y + y < screen_size.width + 1; ++y) {
            std::cout << (y == 0 || y == size.width - 1 ? '|' : ' ');
        }
    }

    // Draw the bottom bar
    if (pos.x + size.height - 1 <= screen_size.height) {
        std::cout << CSI << pos.x + size.height - 1 << ';' << pos.y << 'f';
        std::string bottom_text = "+";
        bottom_text += std::string(std::max(0U, size.width - 2), '-');
        bottom_text += '+';
        std::cout << bottom_text.substr(0, std::max(0, int(screen_size.width - pos.y + 1)));
    }

    reset_color();

    Position current = get_pos();
    current.x += 1;
    current.y += 1;

    for (auto& widget : children) {
        widget->set_active(widget == children[active_index] ? true : false);
        widget->set_pos(current);
        widget->set_size({ size.height - 2, size.width - 2 });
        widget->draw();
    }
}

void
Window::attach(const std::shared_ptr<Widget>& new_child)
{
    children.push_back(new_child);
    new_child->set_parent(this);
    new_child->set_action_parent(this);
}

void
Window::attach(const std::vector<std::shared_ptr<Widget>>& new_children)
{
    std::for_each(
      new_children.cbegin(), new_children.cend(), [&](auto& new_child) { attach(new_child); });
}
