import libtui_app;
import libtui_base;
import libtui_button;
import libtui_grid;
import libtui_list;
import libtui_split;
import libtui_window;

#include <memory>
#include <vector>

int main() {
    auto grid = std::make_shared<Grid>(
        Position{1, 1}, Size{1, 1}, Color{Color::Foreground::Default, Color::Background::Default},
        Size{8, 8}, Size{2, 4});

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            std::shared_ptr<Widget> button = std::make_shared<Button>(
                Position{1, 1}, Size{1, 1},
                Color{Color::Foreground::Default, Color::Background::Default}, "*"s);

            grid->attach(button);
        }
    }

    auto list = std::make_shared<List>(Position{1, 1}, Size{1, 1},
                                       Color{Color::Foreground::Default, Color::Background::Blue},
                                       Direction::Vertical);

    auto b1 = std::make_shared<Button>(Position{1, 1}, Size{5, 20},
                                       Color{Color::Foreground::Brown, Color::Background::Cyan},
                                       "Sample text");

    auto b2 = std::make_shared<Button>(Position{1, 1}, Size{5, 20},
                                       Color{Color::Foreground::Blue, Color::Background::Black},
                                       "Sample text");

    auto b3 = std::make_shared<Button>(Position{1, 1}, Size{5, 20},
                                       Color{Color::Foreground::White, Color::Background::Green},
                                       "Sample text");

    list->attach({b1, b2, b3});

    auto split = std::make_shared<Split>(Position{1, 1}, Size{1, 1}, Direction::Horizontal, 0.75,
                                         grid, list);

    auto w1 = std::make_shared<Window>(Position{1, 1}, Size{30, 60},
                                       Color{Color::Foreground::Blue, Color::Background::Black},
                                       "Window 1");

    w1->attach(split);

    Application::init({w1});

    Application::run();

    return 0;
}
