#include "logging.h"

#include <iostream>
#include <string>

#define LOGGING_DEBUG true
#define RAW_TUI

#ifdef RAW_TUI
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#ifdef NCURSES_TUI
#include <ncurses.h>
#endif

Logger::Logger() {
#ifdef NCURSES_TUI
    initscr();
#endif
#ifdef RAW_TUI
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    my = size.ws_row;
    mx = size.ws_col;
#endif
}

Logger::~Logger() {
    //endwin();
}
void Logger::warn(std::string msg) {
        std::cout <<
#ifdef RAW_TUI
                    "\x1b[2K\x1b[1G" << 
#endif
                    "[WARN ] " << msg << 
#ifdef NCURSES_TUI
                    "\r" <<
#endif
                    "\n";
    draw();
}

void Logger::error(std::string msg) {
    std::cerr << "[ERROR] " << msg << "\n";
    draw();
}

void Logger::info(std::string msg) {
        std::cout <<
#ifdef RAW_TUI
                    "\x1b[2K\x1b[1G" << 
#endif
                    "[INFO ] " << msg << 
#ifdef NCURSES_TUI
                    "\r" <<
#endif
                    "\n";
    draw();
}

void Logger::debug(std::string msg) {
    if (LOGGING_DEBUG) {
        std::cout <<
#ifdef RAW_TUI
                    "\x1b[2K\x1b[1G" << 
#endif
                    "[DEBUG] " << msg << 
#ifdef NCURSES_TUI
                    "\r" <<
#endif
                    "\n";
    }
    draw();
}

void Logger::tps(double ms, double tps) {
    this->ms = ms;
    this->tpss = tps;
    draw();
}

void Logger::num_surfaces(uint32_t num) {
    this->num = num;
    draw();
}
void Logger::draw() {
#ifdef RAW_TUI
    std::cout << "\x1b[" << mx << ";0H\x1b[2K\x1b[1G";
    std::cout << tpss << "tps (" << ms << "mspt) ticking " << num << " surfaces"<< std::flush;
#endif
#ifdef NCURSES_TUI
    int mx, my;
    getmaxyx(stdscr, my, mx);
    int x, y;
    getyx(stdscr, y, x);
    wmove(stdscr, my - 1, 0);
    printw(std::to_string(tpss).c_str());
    wmove(stdscr, y, x);
    refresh();
#endif
}