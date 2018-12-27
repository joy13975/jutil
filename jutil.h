#ifndef _UTIL_H
#define _UTIL_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#if defined (__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <sys/stat.h>

/* Enum Generation Macros */

// New style scoped enum declaration
#define CPP_GEN_ENUM(ITEM) ITEM,
#define CPP_GEN_STRING(STRING) #STRING,
#define CPP_GEN_ENUM_AND_STRING(TYPE_NAME, TYPE_STRING_NAME, FOREACH_ITEM) \
    enum class TYPE_NAME { \
        FOREACH_ITEM(CPP_GEN_ENUM) \
    }; \
    static char const* TYPE_STRING_NAME[] = { \
        FOREACH_ITEM(C_GEN_STRING) \
    }; \
    inline char const* TYPE_NAME##ToCStr(const TYPE_NAME val) { \
        return TYPE_STRING_NAME[static_cast<int>(val)]; \
    } \


// Old style unscoped enum declaration
#define C_GEN_ENUM(ITEM) ITEM,
#define C_GEN_STRING(STRING) #STRING,
#define C_GEN_ENUM_AND_STRING(TYPE_NAME, TYPE_STRING_NAME, FOREACH_ITEM) \
    typedef enum { \
        FOREACH_ITEM(C_GEN_ENUM) \
    } TYPE_NAME; \
    static char const* TYPE_STRING_NAME[] = { \
        FOREACH_ITEM(CPP_GEN_STRING) \
    }; \

#ifdef __cplusplus
#define GEN_ENUM_AND_STRING(...) CPP_GEN_ENUM_AND_STRING(__VA_ARGS__)
#else
#define GEN_ENUM_AND_STRING(...) C_GEN_ENUM_AND_STRING(__VA_ARGS__)
#endif  /* ifdef __cplusplus */

#define GEN_ITEM_VAL(ITEM, VAL) ITEM = VAL,
#define GEN_STRING_VAL(STRING, VAL) if(e == VAL) return #STRING;
#define GEN_ENUM_VAL_AND_STRING(TYPE_NAME, TYPE_STRING_NAME, FOREACH_ITEM_VAL) \
    typedef enum { \
        FOREACH_ITEM_VAL(GEN_ITEM_VAL) \
    } TYPE_NAME; \
    static char const* TYPE_STRING_NAME(TYPE_NAME e) { \
        FOREACH_ITEM_VAL(GEN_STRING_VAL) \
    };


//enum-string example:
//instead of:
// typedef enum {
//     ADD,
//     SUB,
//     MUL,
//     DIV,
//     CMP
// } ALUFlag;

// #define FOREACH_ALU_FLAG(MACRO) \
//     MACRO(ADD) \
//     MACRO(SUB) \
//     MACRO(MUL) \
//     MACRO(DIV) \
//     MACRO(CMP)
// GEN_ENUM_AND_STRING(ALUFlag, ALUFlagNames, FOREACH_ALU_FLAG);

/* Logging */
#define FOREACH_LOG_LEVEL(MACRO) \
    MACRO(LOG_DEBUG) \
    MACRO(LOG_INFO) \
    MACRO(LOG_WARNING) \
    MACRO(LOG_QUIET) \
    MACRO(LOG_ERROR)

C_GEN_ENUM_AND_STRING(LogLvl, LogLvlNames, FOREACH_LOG_LEVEL);

#define UTIL_DEFAULT_LOG_LEVEL LOG_DEBUG

#ifdef _SILENT
#define dbg(...)
#define info(...)
#define warn(...)
#define err(...)
#define raw(...)
#define gated_raw(...)
#define die(...)
#define panic_if(...)
#else
#define gated_log(GATE_LVL, FMT, ...) \
    do { if (get_log_level() <= GATE_LVL) _log(__FILE__, __LINE__, GATE_LVL, FMT, ##__VA_ARGS__); } while(0)
#define dbg(FMT, ...) gated_log(LOG_DEBUG, FMT, ##__VA_ARGS__)
#define info(FMT, ...) gated_log(LOG_INFO, FMT, ##__VA_ARGS__)
#define warn(FMT, ...) gated_log(LOG_WARNING, FMT, ##__VA_ARGS__)
#define err(FMT, ...) _log(__FILE__, __LINE__, LOG_ERROR, FMT, ##__VA_ARGS__)
#define raw(FMT, ...) _log(__FILE__, __LINE__, LOG_QUIET, FMT, ##__VA_ARGS__)
#define gated_raw(LVL, FMT, ...) \
    do { if (get_log_level() <= LVL) _log(__FILE__, __LINE__, LOG_QUIET, FMT, ##__VA_ARGS__); } while(0)
#define die(FMT, ...)\
    do {\
        _Pragma("omp single") {\
            int const errno_cpy = errno;\
            _log(__FILE__, __LINE__, LOG_ERROR, FMT, ##__VA_ARGS__);\
            if (errno_cpy)\
                fprintf(\
                    stderr,\
                    "\n---------------------------------------\n"\
                    "Error before death: code %d (%s)\n",\
                    errno_cpy,\
                    strerror(errno_cpy));\
            exit(1);\
        }\
    } while(0)
#define panic_if(COND_EXPR, FMT, ...) \
    do { if(COND_EXPR) die(FMT, ##__VA_ARGS__); } while(0)
#endif //_SILENT

/* Color Printing */
#define CLEAR_TERM() raw("\033[H\033[J")
#define MV_CURSOR_UP(lines) raw("\033[%dA", lines)
#define ERASE_LINE() raw("\r\33[2K")
#define CLR_NRM "\x1B[0m"
#define CLR_RED "\x1B[31m"
#define CLR_GRN "\x1B[32m"
#define CLR_YEL "\x1B[33m"
#define CLR_BLU "\x1B[34m"
#define CLR_MAG "\x1B[35m"
#define CLR_CYN "\x1B[36m"
#define CLR_WHT "\x1B[37m"


/* Log Level (1) */

extern LogLvl J_UTIL_LOG_LEVEL;

static inline LogLvl get_log_level() {
    return J_UTIL_LOG_LEVEL;
}

// The following function is way down there due to ordering.
// static inline void set_log_level(LogLvl lvl);



/* Leading Space */
static int print_leading_spaces = 0;

static inline int get_leading_spaces() {
    return print_leading_spaces;
}

static inline void set_leading_spaces(int n) {
    print_leading_spaces = n;
}

static inline void reset_leading_spaces() {
    print_leading_spaces = 0;
}


/* Logging */
static inline void _log(
    char const* filename,
    const int line,
    const LogLvl lvl,
    char const* fmt,
    ...) {
    va_list args;
    va_start(args, fmt);

    char space_buffer[print_leading_spaces + 1];
    memset(space_buffer, ' ', print_leading_spaces);
    space_buffer[print_leading_spaces] = '\0';

    char *format;
    FILE *fd;

    int bytes_printed = -1;
    switch (lvl) {
    case LOG_DEBUG:
        fd = stdout;
        bytes_printed = asprintf(&format, "%s[DBG] %s%s%s",
                                 CLR_BLU, CLR_NRM, space_buffer, fmt);
        break;
    case LOG_INFO:
        fd = stdout;
        bytes_printed = asprintf(&format, "%s[INFO] %s%s%s",
                                 CLR_MAG, CLR_NRM, space_buffer, fmt);
        break;
    case LOG_WARNING:
        fd = stdout;
        bytes_printed = asprintf(&format, "%s[WARNING] %s%s%s",
                                 CLR_YEL, CLR_NRM, space_buffer, fmt);
        break;
    case LOG_QUIET:
        fd = stdout;
        bytes_printed = asprintf(&format, "%s%s", space_buffer, fmt);
        break;
    case LOG_ERROR:
        fd = stderr;
        bytes_printed = asprintf(&format, "%s[ERROR] %s%s%s",
                                 CLR_RED, CLR_NRM, space_buffer, fmt);
        break;
    default:
        fd = stdout;
        bytes_printed = asprintf(&format, "%s[???] %s%s%s",
                                 CLR_RED, CLR_NRM, space_buffer, fmt);
    }

    if (bytes_printed == -1) {
        fprintf(stderr,
                "Error: asprintf() failed to print for log level %s",
                LogLvlNames[get_log_level()]);
    }
    else {
        vfprintf(fd, format, args);
        fflush(fd);
    }

    free(format);

    va_end(args);
}


/* Log Level (2) */
static inline void set_log_level(LogLvl lvl) {
    if (lvl < 0 || lvl >= LOG_ERROR) {
        err("Invalid log level: %d\n", lvl);

        int const least_verbose_lvl = LOG_QUIET;
        gated_raw(LOG_ERROR, "Must be between 0 and %d\n", least_verbose_lvl);
        for (int i = LOG_DEBUG; i <= least_verbose_lvl; ++i) {
            gated_raw(LOG_ERROR, "  %s=%d\n", LogLvlNames[i], i);
        }
        // raw() and err() bypass verbosity level checks.

        exit(1);
    }

    J_UTIL_LOG_LEVEL = lvl;
    dbg("Log level set to %s\n", LogLvlNames[lvl]);
}

/* Argument Parsing */
#define DECL_VOID_FUNC_STR(name) void name(char const* arg_in)
#define DECL_ARG_CALLBACK(name) DECL_VOID_FUNC_STR(name)
#define DECL_ARG_IN_FAIL_CALLBACK(name) DECL_VOID_FUNC_STR(name)

typedef struct argument_bundle
{
    char const* const short_form;
    char const* const long_form;
    char const* const description;
    const bool exp_val;
    void (*const call_back)(char const* arg_in);
} argument_bundle;

static inline bool arg_matches(char const* arg_in, argument_bundle const* ab) {
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

static inline void _parse_args(
    const int argc,
    char const *argv[],
    const int argbc,
    argument_bundle const* argbv,
    void (*const failure_callback) (char const* failure_arg_in)) {
    for (size_t i = 1; i < (size_t) argc; i++) {
        bool found_arg = false;

        // iterate through argument bundle to match argument
        // call callback function if matched
        for (size_t j = 0; j < (size_t) argbc; j++) {
            if ((found_arg = arg_matches(argv[i], &argbv[j]))) {
                if (argbv[j].call_back != NULL) {
                    if (argbv[j].exp_val) {
                        if (i + 1 > argc - 1) {
                            err("Argument %s expects a pairing value\n",
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
            err("Unknown argument: %s\n", argv[i]);
            failure_callback(argv[i]);
        }
    }
}

#define parse_args(argc, argv, argb, fail_callback) \
    _parse_args(argc, \
                argv, \
                sizeof(argb)/sizeof(argb[0]), \
                argb, \
                fail_callback)

static inline void print_arg_title(char const* title) {
    set_leading_spaces(4);
    raw("%s\n", title);
    reset_leading_spaces();
}

static inline void _print_arg_bundles(argument_bundle const* argbv, const size_t n) {
    for (size_t i = 0; i < n; i++) {
        argument_bundle const* ab = &(argbv[i]);
        set_leading_spaces(8);
        raw("-%s, --%s\n", ab->short_form, ab->long_form);
        set_leading_spaces(12);
        raw("%s\n", ab->description);
    }
    reset_leading_spaces();
}

#define print_arg_bundles(argb) \
    _print_arg_bundles(argb, sizeof(argb)/sizeof(argb[0]))


/* IO */
static inline bool file_exists(char const* filename) {
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}

static inline void mkdir_ifn_exists(char const* dirname) {
    if (!file_exists(dirname)) {
        mkdir(dirname, 0700);
    }
}

static inline void write_binary(
    char const* filename,
    char const* ptr,
    size_t write_size) {
    FILE *outFile = fopen(filename, "wb");

    if (!outFile)
        die("Could not open output file \"%s\"\n", filename);

    while (write_size > 0) {
        const size_t wrote = fwrite(ptr, write_size, 1, outFile);

        if (wrote == 0)
            break;

        write_size -= (write_size * wrote);
    }

    fclose(outFile);
}


/* Maths */
static inline bool float_approximates_err(
    const float actual,
    const float ref,
    const float tolerance) {
    const float d = fabs(actual - ref);

    const bool approx = d <= tolerance;
    return approx;
}

static inline bool float_approximates(
    const float actual,
    const float ref) {
    return float_approximates_err(actual, ref, 1e-5);
}

static inline float parse_float(char const* str) {
    char *next;
    float val = strtof(str, &next);
    if (strlen(next))
        die("Failed to parse float from \"%s\"\n", str);
    return val;
}

static inline long parse_long(char const* str) {
    char *next;
    long val = strtol(str, &next, 0); // base 0 allows format detection
    if (strlen(next))
        die("Failed to parse long integer from \"%s\"\n", str);

    return val;
}

#define jmin(a, b) (a < b ? a : b)


/* Timing */
static inline void nsleep(long ns) {
    struct timespec sleep_time;
    sleep_time.tv_sec = ns / 1000000000;
    sleep_time.tv_nsec = ns % 1000000000;
    nanosleep(&sleep_time, NULL);
}

static inline double get_timestamp_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec + tv.tv_sec * 1e6;
}

#if defined (__cplusplus)
}
#endif

#endif /* include guard */
