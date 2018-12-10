#ifndef _UTIL_H
#define _UTIL_H

#if defined (__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Enum Generation */

// New style scoped enum declaration
#define CPP_GEN_ENUM(ITEM) ITEM,
#define CPP_GEN_STRING(STRING) #STRING,
#define CPP_GEN_ENUM_AND_STRING(TYPE_NAME, TYPE_STRING_NAME, FOREACH_ITEM) \
    enum class TYPE_NAME { \
        FOREACH_ITEM(CPP_GEN_ENUM) \
    }; \
    static const char * TYPE_STRING_NAME[] = { \
        FOREACH_ITEM(C_GEN_STRING) \
    }; \
    inline const char * TYPE_NAME##ToCStr(const TYPE_NAME & val) { \
        return TYPE_STRING_NAME[static_cast<int>(val)]; \
    } \


// Old style unscoped enum declaration
#define C_GEN_ENUM(ITEM) ITEM,
#define C_GEN_STRING(STRING) #STRING,
#define C_GEN_ENUM_AND_STRING(TYPE_NAME, TYPE_STRING_NAME, FOREACH_ITEM) \
    typedef enum { \
        FOREACH_ITEM(C_GEN_ENUM) \
    } TYPE_NAME; \
    static const char *TYPE_STRING_NAME[] = { \
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
    static const char * TYPE_STRING_NAME(TYPE_NAME e) { \
        FOREACH_ITEM_VAL(GEN_STRING_VAL) \
    };


//enum-string example:
//instead of:
/*
typedef enum {
    ADD,
    SUB,
    MUL,
    DIV,
    CMP
} ALU_Flag;
*/

//do:
/*
#define FOREACH_ALU_FLAG(MACRO) \
    MACRO(ADD) \
    MACRO(SUB) \
    MACRO(MUL) \
    MACRO(DIV) \
    MACRO(CMP) \

GEN_ENUM_AND_STRING(ALU_Flag, ALU_Flag_String, FOREACH_ALU_FLAG);
*/

/* Logging */
#define FOREACH_LOG_LEVEL(MACRO) \
    MACRO(LOG_PROOF) \
    MACRO(LOG_DEBUG) \
    MACRO(LOG_WARN) \
    MACRO(LOG_MESSAGE) \
    MACRO(LOG_RAW) \
    MACRO(LOG_ERROR) \
    MACRO(LOG_DEATH)

C_GEN_ENUM_AND_STRING(Log_Level, Log_Level_String, FOREACH_LOG_LEVEL);

#define UTIL_DEFAULT_LOG_LEVEL LOG_DEBUG

int get_leading_spaces();
void reset_leading_spaces();
void set_leading_spaces(int n);
const char *get_error_string();
void print_arg_title(const char *title);
void _log(const char *filename, const int line, const Log_Level lvl, const char *fmt, ...);

#ifdef _SILENT
#define prf(FMT, ...)
#define dbg(FMT, ...)
#define wrn(FMT, ...)
#define err(FMT, ...)
#define msg(FMT, ...)
#define raw(FMT, ...)
#define raw_at(LVL, FMT, ...)
#define die(FMT, ...)
#define panic_if(COND_EXPR, FMT, ...)
#else
#define prf(FMT, ...) do { if (LOG_PROOF >= get_log_level()) _log(__FILE__, __LINE__, LOG_PROOF, FMT, ##__VA_ARGS__); } while(0)
#define dbg(FMT, ...) do { if (LOG_DEBUG >= get_log_level()) _log(__FILE__, __LINE__, LOG_DEBUG, FMT, ##__VA_ARGS__); } while(0)
#define wrn(FMT, ...) do { if (LOG_WARN >= get_log_level()) _log(__FILE__, __LINE__, LOG_WARN, FMT, ##__VA_ARGS__); } while(0)
#define err(FMT, ...) _log(__FILE__, __LINE__, LOG_ERROR, FMT, ##__VA_ARGS__)
#define msg(FMT, ...) do { if (LOG_MESSAGE >= get_log_level()) _log(__FILE__, __LINE__, LOG_MESSAGE, FMT, ##__VA_ARGS__); } while(0)
#define raw(FMT, ...) do { if (LOG_RAW >= get_log_level()) _log(__FILE__, __LINE__, LOG_RAW, FMT, ##__VA_ARGS__); } while(0)
#define raw_at(LVL, FMT, ...) do{ if (LVL >= get_log_level()) raw(FMT, ##__VA_ARGS__); } while(0)
#define die(FMT, ...) do { _log(__FILE__, __LINE__, LOG_DEATH, FMT, ##__VA_ARGS__); } while(0)
#define panic_if(COND_EXPR, FMT, ...) do { if(COND_EXPR) _log(__FILE__, __LINE__, LOG_DEATH, FMT, ##__VA_ARGS__); } while(0)
#define panic_when(COND_EXPR) do { if(COND_EXPR) _log(__FILE__, __LINE__, LOG_DEATH, "Panic!\n%s\nCondition met:\n\t" #COND_EXPR "\n", __PRETTY_FUNCTION__); } while(0)
#endif //_SILENT

Log_Level get_log_level();
void set_log_level(Log_Level lvl);

/* Argument Passing */
#define DECL_VOID_FUNC_STR(name) void name(const char* arg_in)
#define DECL_ARG_CALLBACK(name) DECL_VOID_FUNC_STR(name)
#define DECL_ARG_IN_FAIL_CALLBACK(name) DECL_VOID_FUNC_STR(name)

typedef struct argument_bundle
{
    const char *const short_form;
    const char *const long_form;
    const char *const description;
    const bool exp_val;
    void (*const call_back)(const char *arg_in);
} argument_bundle;

void _parse_args(const int argc,
                 char const *argv[],
                 const int argbc,
                 const argument_bundle *argbv,
                 void (*const failure_callback) (const char *failure_arg_in));
#define parse_args(argc, argv, argb, fail_callback) \
    _parse_args(argc, \
                argv, \
                sizeof(argb)/sizeof(argb[0]), \
                argb, \
                fail_callback)

void _print_arg_bundles(const argument_bundle *argbv, const size_t n);
#define print_arg_bundles(argb) \
    _print_arg_bundles(argb, sizeof(argb)/sizeof(argb[0]))

/* IO */
void mkdir_ifn_exists(const char * dirname);
bool file_exists(const char * filename);
void write_binary(
    const char* filename,
    const char* ptr,
    size_t write_size);

/* Maths */
bool float_approximates(
    const float actual,
    const float ref); // default error=1e-5
bool float_approximates_err(
    const float actual,
    const float ref,
    const float error);
float parse_float(const char *str);
long parse_long(const char *str);
#define jmin(a, b) (a < b ? a : b)

/* Terminal Printing */
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

/* Timing */
void nsleep(long ns);
double get_timestamp_us();

#if defined (__cplusplus)
}
#endif

#endif /* include guard */
