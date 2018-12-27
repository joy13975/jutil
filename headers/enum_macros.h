#ifndef ENUM_MACROS_H_
#define ENUM_MACROS_H_

// CPP new style scoped enum declaration
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


// C old style unscoped enum declaration
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


#endif  /* end of include guard: ENUM_MACROS_H_ */