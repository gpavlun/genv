
#include <stdio.h>
#include <stddef.h>
#include <limits.h>

/* Argument types */

typedef enum {
    FMT_CHAR,
    FMT_SCHAR,
    FMT_UCHAR,
    FMT_SHORT,
    FMT_USHORT,
    FMT_INT,
    FMT_UINT,
    FMT_LONG,
    FMT_ULONG,
    FMT_LLONG,
    FMT_ULLONG,
    FMT_FLOAT,
    FMT_DOUBLE,
    FMT_LDOUBLE,
    FMT_STRING,
    FMT_POINTER
} fmt_type_t;

/* Argument descriptor */

typedef struct {
    fmt_type_t type;

    union {
        char c;
        signed char sc;
        unsigned char uc;
        short s;
        unsigned short us;
        int i;
        unsigned int u;
        long l;
        unsigned long ul;
        long long ll;
        unsigned long long ull;
        float f;
        double d;
        long double ld;
        const char *str;
        void *ptr;
    } value;
} fmt_arg_t;

/* Constructors */

#define FMT_CONSTRUCTOR(name, ctype, tag, member) \
    static inline fmt_arg_t name(ctype value) {   \
        return (fmt_arg_t){                       \
            .type = tag,                          \
            .value.member = value                 \
        };                                        \
    }

FMT_CONSTRUCTOR(fmt_char,    char,                FMT_CHAR,    c)
FMT_CONSTRUCTOR(fmt_schar,   signed char,          FMT_SCHAR,   sc)
FMT_CONSTRUCTOR(fmt_uchar,   unsigned char,        FMT_UCHAR,   uc)
FMT_CONSTRUCTOR(fmt_short,   short,                FMT_SHORT,   s)
FMT_CONSTRUCTOR(fmt_ushort,  unsigned short,       FMT_USHORT,  us)
FMT_CONSTRUCTOR(fmt_int,     int,                  FMT_INT,     i)
FMT_CONSTRUCTOR(fmt_uint,    unsigned int,         FMT_UINT,    u)
FMT_CONSTRUCTOR(fmt_long,    long,                 FMT_LONG,    l)
FMT_CONSTRUCTOR(fmt_ulong,   unsigned long,        FMT_ULONG,   ul)
FMT_CONSTRUCTOR(fmt_llong,   long long,            FMT_LLONG,   ll)
FMT_CONSTRUCTOR(fmt_ullong,  unsigned long long,   FMT_ULLONG,  ull)
FMT_CONSTRUCTOR(fmt_float,   float,                FMT_FLOAT,   f)
FMT_CONSTRUCTOR(fmt_double,  double,               FMT_DOUBLE,  d)
FMT_CONSTRUCTOR(fmt_ldouble, long double,          FMT_LDOUBLE, ld)
FMT_CONSTRUCTOR(fmt_string,  const char *,         FMT_STRING,  str)
FMT_CONSTRUCTOR(fmt_pointer, void *,              FMT_POINTER, ptr)

/* Type inference */

#define FMT_ARG(x) _Generic((x), \
    char:               fmt_char,    \
    signed char:        fmt_schar,   \
    unsigned char:      fmt_uchar,   \
    short:              fmt_short,   \
    unsigned short:     fmt_ushort,  \
    int:                fmt_int,     \
    unsigned int:       fmt_uint,    \
    long:               fmt_long,    \
    unsigned long:      fmt_ulong,   \
    long long:          fmt_llong,   \
    unsigned long long: fmt_ullong,  \
    float:              fmt_float,   \
    double:             fmt_double,  \
    long double:        fmt_ldouble, \
    char *:             fmt_string,  \
    const char *:       fmt_string,  \
    void *:             fmt_pointer \
)(x)

/* Argument counting */

#define PP_NARG(...) PP_NARG_(__VA_ARGS__, 8,7,6,5,4,3,2,1)
#define PP_NARG_(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N

#define CAT_(a,b) a##b
#define CAT(a,b) CAT_(a,b)

/* Argument expansion */

#define FE_1(m,a) m(a)
#define FE_2(m,a,...) m(a), FE_1(m,__VA_ARGS__)
#define FE_3(m,a,...) m(a), FE_2(m,__VA_ARGS__)
#define FE_4(m,a,...) m(a), FE_3(m,__VA_ARGS__)
#define FE_5(m,a,...) m(a), FE_4(m,__VA_ARGS__)
#define FE_6(m,a,...) m(a), FE_5(m,__VA_ARGS__)
#define FE_7(m,a,...) m(a), FE_6(m,__VA_ARGS__)
#define FE_8(m,a,...) m(a), FE_7(m,__VA_ARGS__)

#define FOR_EACH(m,...) CAT(FE_,PP_NARG(__VA_ARGS__))(m,__VA_ARGS__)

/* Argument output */

static int fmt_emit_arg(fmt_arg_t arg)
{
    switch (arg.type) {
        case FMT_CHAR:    return printf("%c", arg.value.c);
        case FMT_SCHAR:   return printf("%hhd", (int)arg.value.sc);
        case FMT_UCHAR:   return printf("%hhu", (unsigned int)arg.value.uc);
        case FMT_SHORT:   return printf("%hd", (int)arg.value.s);
        case FMT_USHORT:  return printf("%hu", (unsigned int)arg.value.us);
        case FMT_INT:     return printf("%d", arg.value.i);
        case FMT_UINT:    return printf("%u", arg.value.u);
        case FMT_LONG:    return printf("%ld", arg.value.l);
        case FMT_ULONG:   return printf("%lu", arg.value.ul);
        case FMT_LLONG:   return printf("%lld", arg.value.ll);
        case FMT_ULLONG:  return printf("%llu", arg.value.ull);
        case FMT_FLOAT:   return printf("%f", (double)arg.value.f);
        case FMT_DOUBLE:  return printf("%f", arg.value.d);
        case FMT_LDOUBLE: return printf("%Lf", arg.value.ld);
        case FMT_STRING:  return printf("%s", arg.value.str ? arg.value.str : "(null)");
        case FMT_POINTER: return printf("%p", arg.value.ptr);
    }

    return -1;
}

/* Formatter */

static int fmt_print(
    const char *format,
    const fmt_arg_t *args,
    size_t count
) {
    size_t placeholders = 0;

    /* Validate argument count before producing output. */
    for (const char *p = format; *p; ++p) {
        if (p[0] == '{' && p[1] == '}') {
            ++placeholders;
            ++p;
        }
    }

    if (placeholders != count)
        return -1;

    const char *start = format;
    const char *p = format;
    int written = 0;

    while (*p) {
        if (p[0] == '{' && p[1] == '}') {
            size_t len = (size_t)(p - start);

            if (len) {
                size_t n = fwrite(start, 1, len, stdout);
                if (n != len)
                    return -1;
                written += (int)n;
            }

            int n = fmt_emit_arg(args[0]);
            if (n < 0)
                return -1;

            written += n;
            ++args;
            p += 2;
            start = p;
        } else {
            ++p;
        }
    }

    size_t len = (size_t)(p - start);

    if (len) {
        size_t n = fwrite(start, 1, len, stdout);
        if (n != len)
            return -1;
        written += (int)n;
    }

    return written;
}

/* Public interface */

#define print(fmt, ...) \
    fmt_print((fmt), \
        (fmt_arg_t[]){ FOR_EACH(FMT_ARG, __VA_ARGS__) }, \
        PP_NARG(__VA_ARGS__))

/* Example */

int main(void)
{
    const char *name = "Test";
    int health = 100;
    float speed = 12.5f;
    char rank = 'S';

    print("Name: {}\n Health: {}\n Speed: {}\n Rank: {}\n",
          name, health, speed, rank);
    
    puts("");

    printf("Name: %s\n Health: %d\n Speed: %f\n Rank: %c\n",
          name, health, speed, rank);

    return 0;
}
