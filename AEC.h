/*
 * File: AEC.h
 * Author: Ragib Asif
 * Email: ragib.asif30@myhunter.cuny.edu
 * GitHub: https://github.com/ragibasif
 * LinkedIn: https://www.linkedin.com/in/ragibasif/
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2025 Ragib Asif
 * Version 1.0.0
 *
 * ANSI Escape Codes
 *
 */

#ifndef __AEC_H__
#define __AEC_H__

// Reset all attributes
#define RESET "\x1b[0m"

// Text colors
#define BLACK "\x1b[30m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[37m"
#define DEFAULT "\x1b[39m"

// Background colors
#define BG_BLACK "\x1b[40m"
#define BG_RED "\x1b[41m"
#define BG_GREEN "\x1b[42m"
#define BG_YELLOW "\x1b[43m"
#define BG_BLUE "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN "\x1b[46m"
#define BG_WHITE "\x1b[47m"
#define BG_DEFAULT "\x1b[49m"

// Bright text colors
#define BRIGHT_BLACK "\x1b[90m"
#define BRIGHT_RED "\x1b[91m"
#define BRIGHT_GREEN "\x1b[92m"
#define BRIGHT_YELLOW "\x1b[93m"
#define BRIGHT_BLUE "\x1b[94m"
#define BRIGHT_MAGENTA "\x1b[95m"
#define BRIGHT_CYAN "\x1b[96m"
#define BRIGHT_WHITE "\x1b[97m"

// Bright background colors
#define BG_BRIGHT_BLACK "\x1b[100m"
#define BG_BRIGHT_RED "\x1b[101m"
#define BG_BRIGHT_GREEN "\x1b[102m"
#define BG_BRIGHT_YELLOW "\x1b[103m"
#define BG_BRIGHT_BLUE "\x1b[104m"
#define BG_BRIGHT_MAGENTA "\x1b[105m"
#define BG_BRIGHT_CYAN "\x1b[106m"
#define BG_BRIGHT_WHITE "\x1b[107m"

// Text attributes
#define BOLD "\x1b[1m"
#define DIM "\x1b[2m"
#define ITALIC "\x1b[3m"
#define UNDERLINE "\x1b[4m"
#define BLINK "\x1b[5m"
#define REVERSE "\x1b[7m"
#define HIDDEN "\x1b[8m"
#define STRIKETHROUGH "\x1b[9m"

// Reset specific attributes
#define NO_BOLD "\x1b[21m" // or "\x1b[22m" (both work)
#define NO_ITALIC "\x1b[23m"
#define NO_UNDERLINE "\x1b[24m"
#define NO_BLINK "\x1b[25m"
#define NO_REVERSE "\x1b[27m"
#define NO_HIDDEN "\x1b[28m"
#define NO_STRIKETHROUGH "\x1b[29m"

// Cursor movement
#define CURSOR_UP(n) "\x1b[" #n "A"
#define CURSOR_DOWN(n) "\x1b[" #n "B"
#define CURSOR_FORWARD(n) "\x1b[" #n "C"
#define CURSOR_BACK(n) "\x1b[" #n "D"
#define CURSOR_NEXT_LINE(n) "\x1b[" #n "E"
#define CURSOR_PREV_LINE(n) "\x1b[" #n "F"
#define CURSOR_COLUMN(n) "\x1b[" #n "G"
#define CURSOR_POSITION(row, col) "\x1b[" #row ";" #col "H"

// Save and restore cursor position
#define CURSOR_SAVE "\x1b[s"
#define CURSOR_RESTORE "\x1b[u"

// Cursor visibility
#define CURSOR_HIDE "\x1b[?25l"
#define CURSOR_SHOW "\x1b[?25h"

// Screen operations
#define SCREEN_CLEAR "\x1b[2J"
#define SCREEN_CLEAR_LINE "\x1b[2K"
#define SCREEN_CLEAR_TO_END "\x1b[0J"
#define SCREEN_CLEAR_TO_START "\x1b[1J"
#define LINE_CLEAR_TO_END "\x1b[0K"
#define LINE_CLEAR_TO_START "\x1b[1K"
#define SCROLL_UP(n) "\x1b[" #n "S"
#define SCROLL_DOWN(n) "\x1b[" #n "T"

// Terminal modes
#define ALTERNATE_BUFFER "\x1b[?1049h"
#define MAIN_BUFFER "\x1b[?1049l"
#define LINE_WRAP_ON "\x1b[?7h"
#define LINE_WRAP_OFF "\x1b[?7l"

#endif // __AEC_H__
