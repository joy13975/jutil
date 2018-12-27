#include "jutil.h"

#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <sys/stat.h>
#include <omp.h>

/* Local Free Functions */
#define PASS_VA_ARGS(TO_FUNC, LAST_ARG, ...) \
    do {\
        va_list args;\
        va_start(args, LAST_ARG);\
        TO_FUNC(__VA_ARGS__, args);\
        va_end(args);\
    } while(0)\

/* Logging */
void _log(
    FILE * const fd,
    char const* const tag,
    char const* const fmt,
    va_list args)
{
    char space_buffer[_JUTIL_PRINT_LEADING_SPACES + 1];
    memset(space_buffer, ' ', _JUTIL_PRINT_LEADING_SPACES);
    space_buffer[_JUTIL_PRINT_LEADING_SPACES] = '\0';

    char *new_format;
    int bytes_printed =
        asprintf(&new_format, "%s %s%s", tag, space_buffer, fmt);

    if (bytes_printed == -1) {
        fprintf(stderr, "Error: asprintf() failed");
    }
    else {
        vfprintf(fd, new_format, args);
        fflush(fd);
    }

    free(new_format);
}

void log_wrapper(
    FILE * const fd,
    char const* const tag,
    char const* const fmt,
    ...)
{
    PASS_VA_ARGS(_log, fmt, fd, tag, fmt);
}

#define LOG_FUNC_DEF(FUNC_NAME) \
    void FUNC_NAME(char const* const fmt, ...)
LOG_FUNC_DEF(error); // Forward decl

LOG_FUNC_DEF(panic) {
    #pragma omp single
    {
        int const errno_cpy = errno;
        if (errno_cpy) {
            fprintf(
                stderr,
                "\n"
                "---------------------------------------\n"
                "---------------- PANIC ---------------\n"
                "---------------------------------------\n"
                "Error before death: code %d (%s)\n",
                errno_cpy,
                strerror(errno_cpy));
        }

        PASS_VA_ARGS(error, fmt, fmt);

        exit(1);
    }
}

void panic_if(
    bool const predicate,
    char const* const fmt,
    ...) {
    if (predicate) {
        PASS_VA_ARGS(panic, fmt, fmt);
    }
}

void gated_log(
    JUtilLogLvl const loglvl,
    FILE * const fd,
    char const* const tag,
    char const* const fmt,
    va_list args)
{
    if (_JUTIL_LOGLVL >= loglvl) {
        _log(fd, tag, fmt, args);
    }
}

void gated_log_wrapper(
    JUtilLogLvl const loglvl,
    FILE * const fd,
    char const* const tag,
    char const* const fmt,
    ...)
{
    PASS_VA_ARGS(gated_log, fmt, loglvl, fd, tag, fmt);
}

LOG_FUNC_DEF(error) {
    PASS_VA_ARGS(
        _log,
        fmt,
        stderr,
        COLOR_RED"[ ERROR ]"COLOR_CLEAR,
        fmt);
}

LOG_FUNC_DEF(warn) {
    PASS_VA_ARGS(
        gated_log,
        fmt,
        LOGLVL_WARNING,
        stderr,
        COLOR_YELLOW"[WARNING]"COLOR_CLEAR,
        fmt);
}

LOG_FUNC_DEF(info) {
    PASS_VA_ARGS(
        gated_log,
        fmt,
        LOGLVL_INFO,
        stdout,
        COLOR_MAGENTA"[ INFO  ]"COLOR_CLEAR,
        fmt);
}

LOG_FUNC_DEF(debug) {
    PASS_VA_ARGS(
        gated_log,
        fmt,
        LOGLVL_DEBUG,
        stdout,
        COLOR_BLUE"[ DEBUG ]"COLOR_CLEAR,
        fmt);
}

#undef LOG_FUNC_DEF

JUtilLogLvl get_log_level() {
    return _JUTIL_LOGLVL;
}

void set_log_level(JUtilLogLvl lvl) {
    if (lvl <= LOGLVL_MIN || lvl >= LOGLVL_MAX) {
        error(
            "Invalid log level: %d\nMust be between %d and %d\n",
            lvl,
            LOGLVL_MIN + 1,
            LOGLVL_MAX - 1);

        for (int i = LOGLVL_MIN + 1; i < LOGLVL_MAX; ++i) {
            log_wrapper(
                stderr,
                /*tag=*/"  ",
                /*fmt=*/"%s=%d\n",
                JUtilLogLvlNames[i],
                i);
        }

        exit(1);
    }

    _JUTIL_LOGLVL = lvl;
    debug("Log level set to %s\n", JUtilLogLvlNames[lvl]);
}

size_t get_leading_spaces() {
    return _JUTIL_PRINT_LEADING_SPACES;
}

void set_leading_spaces(size_t const n) {
    _JUTIL_PRINT_LEADING_SPACES = n;
}

void reset_leading_spaces() {
    _JUTIL_PRINT_LEADING_SPACES = 0;
}



/* Argument Parsing */
bool arg_matches(
    char const* const arg_in,
    JUtilArgBundle const* const ab) {
    // anything shorter than 2 chars cannot match
    if (arg_in[0] == '\0' || arg_in[1] == '\0')
        return false;

    if (arg_in[0] == '-') {
        // +1 for short form is to skip "-"
        if (arg_in[1] == '-' &&
                0 == strcmp((char *) (arg_in + 2), ab->long_form)) {
            return true;
        }
        else {
            // +2 for long form is to skip "--"
            if (0 == strcmp((char *) (arg_in + 1), ab->short_form))
                return true;
        }
    }

    return false;
}

void parse_args(
    int const argc,
    char const* const argv[],
    size_t const argbc,
    JUtilArgBundle const* const argbv,
    void (*const failure_callback) (char const* failure_arg_in)) {
    for (size_t i = 1; i < (size_t) argc; ++i) {
        bool found_arg = false;

        // Iterate through argument bundle to match argument call callback
        // function if matched.
        for (size_t j = 0; j < (size_t) argbc; ++j) {
            if ((found_arg = arg_matches(argv[i], &argbv[j]))) {
                if (argbv[j].call_back != NULL) {
                    if (argbv[j].exp_val) {
                        if (i + 1 > argc - 1) {
                            error("Argument %s expects a pairing value\n",
                                  argv[i]);
                            failure_callback(argv[i]);
                            break;
                        }
                        else {
                            argbv[j].call_back(argv[++i]);
                        }
                    }
                    else {
                        argbv[j].call_back(argv[i]);
                    }
                    break;
                }
            }
        }

        if (!found_arg) {
            error("Unknown argument: %s\n", argv[i]);
            failure_callback(argv[i]);
        }
    }
}

void print_arg_title(char const* const title) {
    set_leading_spaces(4);
    log_wrapper(stdout, "", "%s\n", title);
    reset_leading_spaces();
}

void print_arg_bundles(
    JUtilArgBundle const* const argbv,
    size_t const n) {
    for (size_t i = 0; i < n; i++) {
        JUtilArgBundle const* ab = &(argbv[i]);
        set_leading_spaces(8);
        log_wrapper(stdout, "", "-%s, --%s\n", ab->short_form, ab->long_form);
        set_leading_spaces(12);
        log_wrapper(stdout, "", "%s\n", ab->description);
    }
    reset_leading_spaces();
}

float parse_float(char const* const str) {
    char *next;
    float val = strtof(str, &next);
    if (strlen(next))
        panic("Failed to parse float from \"%s\"\n", str);
    return val;
}

long parse_long(char const* const str) {
    char *next;
    long val = strtol(str, &next, 0); // base 0 allows format detection
    if (strlen(next))
        panic("Failed to parse long integer from \"%s\"\n", str);

    return val;
}


/* IO */
bool file_exists(char const* const filename) {
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}

void mkdir_ifn_exists(char const* const dirname) {
    if (!file_exists(dirname)) {
        mkdir(dirname, 0700);
    }
}

void write_binary(
    char const* const filename,
    char const* const ptr,
    size_t write_size) {
    FILE *outFile = fopen(filename, "wb");

    if (!outFile)
        panic("Could not open output file \"%s\"\n", filename);

    while (write_size > 0) {
        const size_t wrote = fwrite(ptr, write_size, 1, outFile);

        if (wrote == 0)
            break;

        write_size -= (write_size * wrote);
    }

    fclose(outFile);
}

/* Maths */
bool float_approximates(
    float const actual,
    float const ref,
    float const tolerance) {
    return fabs(actual - ref) <= tolerance;
}

bool float_approximates_1e5(
    float const actual,
    float const ref) {
    return float_approximates(actual, ref, 1e-5);
}

/* Timing */
void nsleep(long ns) {
    struct timespec sleep_time;
    sleep_time.tv_sec = ns / 1000000000;
    sleep_time.tv_nsec = ns % 1000000000;
    nanosleep(&sleep_time, NULL);
}

double get_timestamp_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec + tv.tv_sec * 1e6;
}


/* Globals Definitions */
JUtilLogLvl _JUTIL_LOGLVL = LOGLVL_DEBUG;
size_t _JUTIL_PRINT_LEADING_SPACES = 0;
JUtilLibraryStruct const JUtil = {
#define ASSIGN(NAME) \
    .NAME = NAME

    /* Logging */
    .gated_log = gated_log_wrapper,
    ASSIGN(panic),
    ASSIGN(panic_if),

    ASSIGN(error),
    ASSIGN(warn),
    ASSIGN(info),
    ASSIGN(debug),

    ASSIGN(get_log_level),
    ASSIGN(set_log_level),
    ASSIGN(get_leading_spaces),
    ASSIGN(set_leading_spaces),
    ASSIGN(reset_leading_spaces),

    /* Argument Parsing */
    ASSIGN(arg_matches),
    ASSIGN(parse_args),
    ASSIGN(print_arg_title),
    ASSIGN(print_arg_bundles),
    ASSIGN(parse_float),
    ASSIGN(parse_long),

    /* IO */
    ASSIGN(file_exists),
    ASSIGN(mkdir_ifn_exists),
    ASSIGN(write_binary),

    /* Maths */
    ASSIGN(float_approximates),
    ASSIGN(float_approximates_1e5),

    /* Timing */
    ASSIGN(nsleep),
    ASSIGN(get_timestamp_us),

#undef ASSIGN
};