#include <stdio.h>
#include "todo.h"

int main(void) {
    Test t;
    t.x = 5;
    printf("Hello <todo>! (%d)\n", t.x);
    return 0;
}
