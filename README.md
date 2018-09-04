# jutil
A C helper library that does trivial stuff.

I've made some effort to test, but some parts or all might still be buggy/badly written. Use at your own risk, but feel free to open an issue/contribute.

## Logging

There are 7 log types, in decreasing verbosity: `PROOF`, `DEBUG`, `WARN`, `MESSAGE`, `RAW`, `ERROR`, `DEATH`. Each printing macro acts just like `printf()`, but with a log level check. If log level passes, they print a color-coded tag prefix (unfortunately can't show colors in Github markdown) followed by the formatted string.

```c
#define prf(fmt, ...)
#define dbg(fmt, ...)
#define wrn(fmt, ...)
#define err(fmt, ...)
#define msg(fmt, ...)
#define raw(fmt, ...)
#define raw_at(lvl, fmt, ...)
#define die(fmt, ...)
```

`raw()` is an exception in that it intentionally does not print a tag. `raw_at()` prints at an arbitrary log level (`lvl`) without tag.

## Argument Parsing
Instead of parsing arguments manually, you could:
```c

typedef struct {
    const char * inputFile;
    /* Many other options */
} Options;
Options options;

DECL_ARG_CALLBACK(helpAndExit);
DECL_ARG_CALLBACK(setInputFile);

const argument_bundle argb[] = {
    /* {short_form, long_form, description, exp_val, call_back} */
    {"h", "help", "Print this help text and exit", false, helpAndExit},
    {"i", "inputFile", "Set input file (dummy argument)", true, setInputFile}
    /* Many many more option arguments */
};

DECL_ARG_CALLBACK(helpAndExit) {
    print_arg_bundles(argb);
    exit(1);
}

DECL_ARG_CALLBACK(setInputFile) { options.inputFile = arg_in; }

int main(int argc, const char ** argv){
	/* When parse_args() fails, it calls helpAndExit() */
    parse_args(argc, argv, argb, helpAndExit);
    /* Do stuff using options */
    return 0;
}
```

It might not look much better at first glance, but this method scales well. When adding a new argument, just add an `argument_bundle` to `argb` with a callback, and declare & define that callback with the macro:

```c
#define DECL_ARG_CALLBACK(name) DECL_VOID_FUNC_STR(name)
```

Which just expands to:

```c
void name(const char* arg_in);
```

When either the short form or the long form flag of an argument is detected, the corresponding callback is called. The `exp_val` flag indicates whether a value is expected to follow the argument flag. If so, it will be passed to `call_back`. Otherwise, `call_back` is passed the argument string that triggered its call.

Jutil then prints clear errors when user passes something unexpected:

```shell
$  ./jutil -i
[ERR] Argument -i expects a pairing value
        h, help
            Print this help text and exit
        i, inputFile
            Set input file (dummy argument)
$  ./jutil -ii
[ERR] Unknown argument: -ii
        h, help
            Print this help text and exit
        i, inputFile
            Set input file (dummy argument)
```


## Enum Definition:
Instead of:
```c
typedef enum {
    ADD,
    SUB,
    MUL,
    DIV,
    CMP
} ALU_Flag;

static const char *ALU_Flag_String[] {
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "CMP"
};
```

You can shorten it to:
```c
#define FOREACH_ALU_FLAG(MACRO) \
    MACRO(ADD) \
    MACRO(SUB) \
    MACRO(MUL) \
    MACRO(DIV) \
    MACRO(CMP) \

GEN_ENUM_AND_STRING(ALU_Flag, ALU_Flag_String, FOREACH_ALU_FLAG);
```

Alternatviely if custom consants are needed:

```c
#define FOREACH_ALU_FLAG(MACRO) \
    MACRO(ADD, 0xf0) \
    MACRO(SUB, 0xf1) \
    MACRO(MUL, 0xf2) \
    MACRO(DIV, 0xf3) \
    MACRO(CMP, 0xf4) \

GEN_ENUM_VAL_AND_STRING(ALU_Flag, ALU_Flag_String, FOREACH_ALU_FLAG);
```

## Miscellaneous

Some other useful small utility functions.

Macros:
```c
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
```