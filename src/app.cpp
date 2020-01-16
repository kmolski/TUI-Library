#include "app.h"
#include "base_classes.h"
#include "misc.h"
#include "screen.h"

Screen Application::root = Screen();
bool Application::should_continue = true;

void
Application::update_screen_size()
{
    winsize term_size;
    ioctl(0, TIOCGWINSZ, &term_size);

    root.set_size({ term_size.ws_row, term_size.ws_col });
}

void
Application::resize_signal_handler(int signal)
{
    if (signal == SIGWINCH) {
        update_screen_size();
    }
}

inline void
Application::process_charkey(std::string& action, char key)
{
    switch (key) {
        case 9:
            action += "TAB";
            break;
        case 13:
            action += "ENTER";
            break;
        case 32:
            action += "SPACE";
            break;
        case 127:
            action += "BACKSPACE";
            break;
        default:
            if (key < 27) {
                action += "CTRL+";
                action += char(key + 64);
            } else if (isalpha(key)) {
                if (isupper(key)) {
                    action += "SHIFT+";
                    action += key;
                } else if (islower(key)) {
                    action += toupper(key);
                }
            } else {
                action += key;
            }
    }
}

inline void
Application::process_xterm_seqence(std::string& action, char key)
{
    switch (key) {
        case 'A':
            action += "UP";
            break;
        case 'B':
            action += "DOWN";
            break;
        case 'C':
            action += "RIGHT";
            break;
        case 'D':
            action += "LEFT";
            break;
        case 'F':
            action += "END";
            break;
        case 'H':
            action += "HOME";
            break;
        case 'Z':
            action += "SHIFT+TAB";
            break;
    }
}

inline void
Application::process_esc_sequence(std::string& action)
{
    const std::string key_names[] = { "HOME", "INSERT", "DELETE", "END", "PG_UP", "PG_DOWN",
                                      "HOME", "END",    "",       "F0",  "F1",    "F2",
                                      "F3",   "F4",     "F5",     "",    "F6",    "F7",
                                      "F8",   "F9",     "F10",    "",    "F11",   "F12" };

    char key = 0;
    int code = 0;

    while ((key = std::cin.get())) {
        if (isupper(key)) {
            process_xterm_seqence(action, key);
            return;
        } else if (isdigit(key)) {
            code *= 10;
            code += (key - '0');
        } else if (key == ';') {
            if ((key = std::cin.get())) {
                process_modifiers(action, key);
            }
        } else if (key == '~' && code > 0 && code < 25) {
            action += key_names[int(code) - 1];
            return;
        }
    }
}

inline void
Application::process_modifiers(std::string& action, char key)
{
    --key; // Decrement key by one to get bit mask
    if (key & 0b0010)
        action += "ALT+";
    if (key & 0b0100)
        action += "CTRL+";
    if (key & 0b0001)
        action += "SHIFT+";
}

inline void
Application::process_function_key(std::string& action)
{
    char key = 0;
    while ((key = std::cin.get())) {
        if (isdigit(key)) {
            process_modifiers(action, key);
            continue;
        }
        switch (key) {
            case 'P':
                action += "F1";
                break;
            case 'Q':
                action += "F2";
                break;
            case 'R':
                action += "F3";
                break;
            case 'S':
                action += "F4";
                break;
        }
        break;
    }
}

inline void
Application::process_keystrokes()
{
    char key = std::cin.get();
    std::string action;
    if (key != 0x1B) {
        process_charkey(action, key);
    } else if ((key = std::cin.get())) {
        if (key == '[') { // Escape sequence
            process_esc_sequence(action);
        } else if (key == 'O') { // F1-F4
            process_function_key(action);
        } else { // ALT+key
            action += "ALT+";
            process_charkey(action, key);
        }
    }

    if (key) {
        Actionable* child = root.get_deepest_active();
        ActionResult response = child->handle_key(key);

        while (response != ActionResult::Success && child != nullptr) {
            switch (response) {
                case ActionResult::Success:
                    break;
                case ActionResult::GetShortcut: {
                    response = child->handle_shortcut(action);
                    break;
                }
                case ActionResult::Failed:
                    // Make the parent handle the action
                    child = child->get_action_parent();
                    if (child) {
                        response = child->handle_key(key);
                    } else {
                        return;
                    }
            }
        }
    }
}

void
Application::init(const std::vector<std::shared_ptr<Window>>& widgets)
{
    std::signal(SIGWINCH, resize_signal_handler);

    update_screen_size();

    root.attach(widgets);
}

void
Application::run()
{
    size_t keys_available = 0;

    while (should_continue) {
        if (ioctl(0, FIONREAD, &keys_available), keys_available) {
            process_keystrokes();
        }
        root.draw();
        std::this_thread::sleep_for(frame_delay);
    }
}

Size
Application::get_screen_size()
{
    return root.get_size();
}
