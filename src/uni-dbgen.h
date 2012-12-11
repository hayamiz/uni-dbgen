#ifndef UNI_DBGEN_H
#define UNI_DBGEN_H

#include <stdbool.h>

typedef struct {
    int scalefactor;
    bool verbose;
    int parallel;
} option_t;


extern void generate(int argc, char **argv);

#endif
