#ifndef TYPES_H_
#define TYPES_H_

#include <stdbool.h>
#include <stddef.h>

#define FOREACH_LOGLVL(MACRO) \
    MACRO(LOGLVL_MIN) \
    MACRO(LOGLVL_QUIET) \
    MACRO(LOGLVL_WARNING) \
    MACRO(LOGLVL_INFO) \
    MACRO(LOGLVL_DEBUG) \
    MACRO(LOGLVL_MAX)

GEN_ENUM_AND_STRING(JUtilLogLvl, JUtilLogLvlNames, FOREACH_LOGLVL);

typedef struct
{
    char const* const short_form;
    char const* const long_form;
    char const* const description;
    bool const exp_val;
    void (*const call_back)(char const* arg_in);
} JUtilArgBundle;

#endif  /* end of include guard: TYPES_H_ */