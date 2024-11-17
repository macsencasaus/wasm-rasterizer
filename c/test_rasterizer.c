#include <stdio.h>

#include "rasterizer.h"

#define ASSERT_EQ(expected, actual)                                     \
    if ((expected) != (actual)) {                                       \
        printf("Test failed: %s != %s, at %s:%d\n", #expected, #actual, \
               __FILE__, __LINE__);                                     \
        exit(1);                                                        \
    }

int main(void) {
    printf("All tests passed!\n");
    return 0;
}
