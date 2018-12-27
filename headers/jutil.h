#ifndef JUTIL_H
#define JUTIL_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#if defined (__cplusplus)
extern "C" {
#endif

#include <stdio.h>

#include "enum_macros.h"
#include "terminal_escapes.h"
#include "types.h"

//
// Library interface structure
//
typedef struct {

    /* Logging */
#define LOG_FUNC_PTR_DECL(FUNC_NAME) \
    void (*const FUNC_NAME)(char const* const fmt, ...)

    LOG_FUNC_PTR_DECL(panic);
    void (*const panic_if)(
        bool const predicate,
        char const* const fmt,
        ...);
    LOG_FUNC_PTR_DECL(error);
    LOG_FUNC_PTR_DECL(warn);
    LOG_FUNC_PTR_DECL(info);
    LOG_FUNC_PTR_DECL(debug);

#undef LOG_FUNC_PTR_DECL

    JUtilLogLvl (*const get_log_level)();
    void (*const set_log_level)(JUtilLogLvl);
    size_t (*const get_leading_spaces)();
    void (*const set_leading_spaces)(size_t const);
    void (*const reset_leading_spaces)();

    /* Argument Parsing */
    bool (*const arg_matches)(
        char const* const arg_in,
        JUtilArgBundle const* const ab);
    void (*const parse_args)(
        int const argc,
        char const* const argv[],
        size_t const argbc,
        JUtilArgBundle const* const argbv,
        void (*const failure_callback) (char const* failure_arg_in));
    void (*print_arg_title)(char const* const title);
    void (*print_arg_bundles)(
        JUtilArgBundle const* const argbv,
        size_t const n);
    float (*parse_float)(char const* const str);
    long (*parse_long)(char const* const str);

#define JUTIL_ARG_CALLBACK_DECL(NAME) \
    void NAME(char const* arg_in)
#define JUTIL_COUNT_ARG_BUNDLES(ARG_BUNDLES) \
    sizeof(ARG_BUNDLES) / sizeof(JUtilArgBundle)

    /* IO */
    bool (*file_exists)(char const* const filename);
    void (*mkdir_ifn_exists)(char const* const dirname);
    void (*write_binary)(
        char const* const filename,
        char const* const ptr,
        size_t write_size);

    /* Maths */
    bool (*float_approximates)(
        float const actual,
        float const ref,
        float const tolerance);
    bool (*float_approximates_1e5)(
        float const actual,
        float const ref);

    /* Timing */
    void (*nsleep)(long ns);
    double (*get_timestamp_us)();
} JUtilLibraryStruct;

/* Globals */
extern JUtilLogLvl _JUTIL_LOGLVL;
extern size_t _JUTIL_PRINT_LEADING_SPACES;
extern JUtilLibraryStruct const JUtil;

#if defined (__cplusplus)
}
#endif

#endif  /* end of include guard: JUTIL_H */