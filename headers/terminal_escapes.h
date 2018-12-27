#ifndef TERMINAL_ESCAPES_H_
#define TERMINAL_ESCAPES_H_

#define CLEAR_TERM(fd) fprintf(fd, "\033[H\033[J")
#define CURSOR_UP(fd, lines) fprintf(fd, "\033[%dA", lines)
#define ERASE_LINE(fd) fprintf(fd, "\r\33[2K")
#define COLOR_CLEAR "\x1B[0m"
#define COLOR_RED "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_BLUE "\x1B[34m"
#define COLOR_MAGENTA "\x1B[35m"
#define COLOR_CYAN "\x1B[36m"
#define COLOR_WHITE "\x1B[37m"

#endif  /* end of include guard: TERMINAL_ESCAPES_H_ */