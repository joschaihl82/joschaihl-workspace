/* test.c
 * Minimal test program exercising the compiler's recognized keywords.
 * Keeps size small while touching typedef, struct, union, extern, static,
 * int, char, long, short, unsigned, if, else, for, while, return.
 */

typedef unsigned long u_long_t;
typedef short s_short_t;

struct S {
    int a;
    char b;
};

union U {
    int x;
    char y;
};

extern int ext_symbol;
static int static_counter = 1;

int add(int x, int y)
{
    return x + y;
}

int main(void)
{
    struct S s;
    struct S *ps;
    union U u;
    int arr[2];
    int i;
    int sum = 0;

    /* initialize */
    s.a = 2;
    s.b = 'A';
    ps = &s;
    ps->a = ps->a * 2;        /* pointer + arrow */
    u.x = 5;
    arr[0] = 7;
    arr[1] = 3;

    /* for loop and if/else */
    for (i = 0; i < 2; i++) {
        if (arr[i] % 2 == 0)
            sum += add(arr[i], u.x);
        else
            sum += arr[i];
    }

    /* while loop */
    while (static_counter > 0) {
        static_counter--;
        if (static_counter == 0)
            break;
    }


    return sum;
}
