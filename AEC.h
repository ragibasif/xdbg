// AEC.h
// ANSI Escape Codes

#ifndef __AEC_H__
#define __AEC_H__

// Reset all attributes
#define AEC_RESET "\x1b[0m"

// Text colors
#define AEC_BLACK "\x1b[30m"
#define AEC_RED "\x1b[31m"
#define AEC_GREEN "\x1b[32m"
#define AEC_YELLOW "\x1b[33m"
#define AEC_BLUE "\x1b[34m"
#define AEC_MAGENTA "\x1b[35m"
#define AEC_CYAN "\x1b[36m"
#define AEC_WHITE "\x1b[37m"
#define AEC_DEFAULT "\x1b[39m"

// Background colors
#define AEC_BG_BLACK "\x1b[40m"
#define AEC_BG_RED "\x1b[41m"
#define AEC_BG_GREEN "\x1b[42m"
#define AEC_BG_YELLOW "\x1b[43m"
#define AEC_BG_BLUE "\x1b[44m"
#define AEC_BG_MAGENTA "\x1b[45m"
#define AEC_BG_CYAN "\x1b[46m"
#define AEC_BG_WHITE "\x1b[47m"
#define AEC_BG_DEFAULT "\x1b[49m"

// Bright text colors
#define AEC_BRIGHT_BLACK "\x1b[90m"
#define AEC_BRIGHT_RED "\x1b[91m"
#define AEC_BRIGHT_GREEN "\x1b[92m"
#define AEC_BRIGHT_YELLOW "\x1b[93m"
#define AEC_BRIGHT_BLUE "\x1b[94m"
#define AEC_BRIGHT_MAGENTA "\x1b[95m"
#define AEC_BRIGHT_CYAN "\x1b[96m"
#define AEC_BRIGHT_WHITE "\x1b[97m"

// Bright background colors
#define AEC_BG_BRIGHT_BLACK "\x1b[100m"
#define AEC_BG_BRIGHT_RED "\x1b[101m"
#define AEC_BG_BRIGHT_GREEN "\x1b[102m"
#define AEC_BG_BRIGHT_YELLOW "\x1b[103m"
#define AEC_BG_BRIGHT_BLUE "\x1b[104m"
#define AEC_BG_BRIGHT_MAGENTA "\x1b[105m"
#define AEC_BG_BRIGHT_CYAN "\x1b[106m"
#define AEC_BG_BRIGHT_WHITE "\x1b[107m"

// Text attributes
#define AEC_BOLD "\x1b[1m"
#define AEC_DIM "\x1b[2m"
#define AEC_ITALIC "\x1b[3m"
#define AEC_UNDERLINE "\x1b[4m"
#define AEC_BLINK "\x1b[5m"
#define AEC_REVERSE "\x1b[7m"
#define AEC_HIDDEN "\x1b[8m"
#define AEC_STRIKETHROUGH "\x1b[9m"

// Reset specific attributes
#define AEC_NO_BOLD "\x1b[21m" // or "\x1b[22m" (both work)
#define AEC_NO_ITALIC "\x1b[23m"
#define AEC_NO_UNDERLINE "\x1b[24m"
#define AEC_NO_BLINK "\x1b[25m"
#define AEC_NO_REVERSE "\x1b[27m"
#define AEC_NO_HIDDEN "\x1b[28m"
#define AEC_NO_STRIKETHROUGH "\x1b[29m"

// Cursor movement
#define AEC_CURSOR_UP(n) "\x1b[" #n "A"
#define AEC_CURSOR_DOWN(n) "\x1b[" #n "B"
#define AEC_CURSOR_FORWARD(n) "\x1b[" #n "C"
#define AEC_CURSOR_BACK(n) "\x1b[" #n "D"
#define AEC_CURSOR_NEXT_LINE(n) "\x1b[" #n "E"
#define AEC_CURSOR_PREV_LINE(n) "\x1b[" #n "F"
#define AEC_CURSOR_COLUMN(n) "\x1b[" #n "G"
#define AEC_CURSOR_POSITION(row, col) "\x1b[" #row ";" #col "H"

// Save and restore cursor position
#define AEC_CURSOR_SAVE "\x1b[s"
#define AEC_CURSOR_RESTORE "\x1b[u"

// Cursor visibility
#define AEC_CURSOR_HIDE "\x1b[?25l"
#define AEC_CURSOR_SHOW "\x1b[?25h"

// Screen operations
#define AEC_SCREEN_CLEAR "\x1b[2J"
#define AEC_SCREEN_CLEAR_LINE "\x1b[2K"
#define AEC_SCREEN_CLEAR_TO_END "\x1b[0J"
#define AEC_SCREEN_CLEAR_TO_START "\x1b[1J"
#define AEC_LINE_CLEAR_TO_END "\x1b[0K"
#define AEC_LINE_CLEAR_TO_START "\x1b[1K"
#define AEC_SCROLL_UP(n) "\x1b[" #n "S"
#define AEC_SCROLL_DOWN(n) "\x1b[" #n "T"

// Terminal modes
#define AEC_ALTERNATE_BUFFER "\x1b[?1049h"
#define AEC_MAIN_BUFFER "\x1b[?1049l"
#define AEC_LINE_WRAP_ON "\x1b[?7h"
#define AEC_LINE_WRAP_OFF "\x1b[?7l"

#endif // __AEC_H__
