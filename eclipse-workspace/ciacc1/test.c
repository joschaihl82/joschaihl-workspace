/* * test.c
 * A comprehensive test suite for the 5cc compiler.
 */

// 1. External declarations (since no preprocessor/headers)
int printf(char *fmt, ...);
int exit(int code);

// 2. Global Variable Tests
int global_val;
int global_arr[5];
char *global_str;

// 3. Typedef Tests
typedef int MyInt;
typedef struct MyStruct MyStruct;

// 4. Struct and Union Definitions
struct MyStruct {
    int a;
    char b;
    long c;
};

union MyUnion {
    int x;
    char y;
};

// Helper function for assertions
void assert(int expected, int actual, char *msg) {
    if (expected == actual) {
        printf("OK: %s => %d\n", msg, actual);
        return;
    }
    printf("FAIL: %s. Expected %d, but got %d\n", msg, expected, actual);
    exit(1);
}

// Function with many arguments to test register passing (rdi, rsi, rdx, rcx, r8, r9)
int add6(int a, int b, int c, int d, int e, int f) {
    return a + b + c + d + e + f;
}

// Recursive function (fibonacci) to test stack depth and calls
int fib(int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}

int main() {
    printf("Starting 5cc capability tests...\n");

    // ==========================================
    // 1. Integers, Arithmetic and Precedence
    // ==========================================
    assert(42, 42, "Integer literal");
    assert(5, 2 + 3, "Addition");
    assert(2, 5 - 3, "Subtraction");
    assert(12, 3 * 4, "Multiplication");
    assert(3, 12 / 4, "Division");
    assert(1, 5 % 2, "Modulo");
    assert(7, 1 + 2 * 3, "Precedence (* over +)");
    assert(9, (1 + 2) * 3, "Grouping ()");
    assert(10, -5 + 15, "Unary negation");
    assert(1, +1, "Unary plus");

    // ==========================================
    // 2. Comparisons
    // ==========================================
    assert(1, 1 == 1, "Equality");
    assert(1, 1 != 2, "Inequality");
    assert(1, 1 < 2, "Less than");
    assert(1, 2 <= 2, "Less equal");
    assert(1, 2 > 1, "Greater than");
    assert(1, 2 >= 2, "Greater equal");
    assert(0, 1 == 2, "False equality");

    // ==========================================
    // 3. Variables (Local) and Assignment
    // ==========================================
    int a;
    a = 10;
    assert(10, a, "Local var assignment");
    int b = 20;
    assert(20, b, "Local var init");
    assert(30, a + b, "Var usage");

    // Comma operator
    assert(50, (a=20, b=30), "Comma operator");

    // ==========================================
    // 4. Control Flow
    // ==========================================

    // IF / ELSE
    int ret = 0;
    if (1) ret = 1; else ret = 0;
    assert(1, ret, "If statement true");

    if (0) ret = 1; else ret = 2;
    assert(2, ret, "If else statement false");

    // WHILE
    int i = 0;
    while (i < 5) i = i + 1;
    assert(5, i, "While loop");

    // FOR
    int j = 0;
    int sum = 0;
    for (j = 0; j <= 5; j = j + 1) {
        sum = sum + j;
    }
    assert(15, sum, "For loop");

    // BLOCK Scope
    {
        int a = 99; // Shadowing outer 'a'
        assert(99, a, "Block scope variable shadowing");
    }
    assert(20, a, "Outer variable check"); // Outer 'a' was 20

    // ==========================================
    // 5. Pointers and Addressing
    // ==========================================
    int x = 3;
    int *y = &x;
    assert(3, *y, "Pointer dereference");
    *y = 4;
    assert(4, x, "Pointer write");

    // Pointer Arithmetic
    // Integers are 4 bytes. Adding 1 to pointer adds 4.
    char *p_char;
    int *p_int;
    assert(8, sizeof(p_int), "Size of pointer");

    // Array access
    int arr[3];
    arr[0] = 100;
    arr[1] = 200;
    *(arr + 2) = 300; // Pointer math syntax for array
    assert(100, arr[0], "Array index 0");
    assert(200, 1[arr], "Array index 1 (swapped)");
    assert(300, arr[2], "Array pointer math");

    // ==========================================
    // 6. Types (char, short, long, void*)
    // ==========================================
    char c = 1;
    short s = 2;
    long l = 3;
    assert(1, sizeof(c), "Sizeof char");
    assert(2, sizeof(s), "Sizeof short");
    assert(4, sizeof(int), "Sizeof int");
    assert(8, sizeof(l), "Sizeof long");

    // ==========================================
    // 7. String Literals
    // ==========================================
    char *str = "Hello";
    assert(72, str[0], "String literal char 'H'"); // 72 is ASCII 'H'
    assert(101, str[1], "String literal char 'e'"); // 101 is ASCII 'e'

    // Escape characters
    char *esc = "A\nB";
    assert(10, esc[1], "Escape sequence newline");

    // ==========================================
    // 8. Structs and Unions
    // ==========================================
    struct MyStruct st;
    st.a = 10;
    st.c = 50;
    assert(10, st.a, "Struct member access");
    assert(50, st.c, "Struct member offset access");
    assert(16, sizeof(st), "Struct size (aligned 4+1+pad+8)");

    struct MyStruct *stp = &st;
    stp->a = 20;
    assert(20, st.a, "Struct arrow operator");

    union MyUnion u;
    u.x = 0;
    u.y = 1; // overwrites lowest byte of x
    assert(1, u.x, "Union shared memory");

    // ==========================================
    // 9. Functions and Recursion
    // ==========================================
    assert(21, add6(1, 2, 3, 4, 5, 6), "Func call 6 args");
    assert(55, fib(10), "Recursion (fib 10)");

    // ==========================================
    // 10. Typedef usage
    // ==========================================
    MyInt val = 77;
    assert(77, val, "Typedef usage");

    // ==========================================
    // 11. Statement Expressions (GNU Extension)
    // ==========================================
    int stmt_res = ({
        int val1 = 5;
        int val2 = 10;
        val1 + val2;
    });
    assert(15, stmt_res, "Statement expression");

    // ==========================================
    // 12. Global Variables
    // ==========================================
    global_val = 123;
    assert(123, global_val, "Global int access");

    global_arr[4] = 99;
    assert(99, global_arr[4], "Global array access");

    // ==========================================
    // 13. Comments (Tokenizer check)
    // ==========================================
    // This is a single line comment
    /* This is a
       multi-line comment */
    int /* comment inside */ z = 1;
    assert(1, z, "Comments ignored correctly");

    printf("All tests passed successfully.\n");
    return 0;
}
