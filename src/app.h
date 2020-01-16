#ifndef APPLICATION
#define APPLICATION

#include <chrono>
#include <csignal>
#include <thread>

#include <sys/ioctl.h>
#include <unistd.h>

#include "screen.h"

class Application
{
    static Screen root;
    static bool should_continue;

    static constexpr auto frame_delay = std::chrono::milliseconds{ 1000 / 60 };

    static void update_screen_size();
    static void resize_signal_handler(int signal);

    static void process_charkey(std::string& action, char key);
    static void process_xterm_seqence(std::string& action, char key);
    static void process_esc_sequence(std::string& action);
    static void process_modifiers(std::string& action, char key);
    static void process_function_key(std::string& action);
    static void process_keystrokes();

  public:
    static void init(const std::vector<std::shared_ptr<Window>>& widgets);
    static void run();
    static Size get_screen_size();
};

#endif // APPLICATION
