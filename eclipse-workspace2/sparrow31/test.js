var g_total = 0;
var arr = [10, 20];
var obj = {x: 1, y: 5};

// KEYWORDS: function, var, return, if, else
function calculate(a, b) {
  var temp = a + b;
  if (temp > 5) {
    return temp * 2;
  } else {
    return temp * 3;
  }
}

// FUNCTION CALL, ARRAY ACCESS, OBJECT ACCESS, ASSIGNMENT
// arr[0] = 10, obj.x = 1. calculate(10, 1) -> 11. 11 > 5 -> return 11 * 2 = 22.
g_total = calculate(arr[0], obj.x);
printf("%ld\n", g_total); // Expected: 22

// KEYWORD: while
// Runs 5 times (i=0 to 4). obj.y = 5.
var i = 0;
while (i < obj.y) {
  // g_total = 22 + (0+1+2+3+4) = 32
  g_total = g_total + i;
  i = i + 1;
}
printf("%ld\n", g_total); // Expected: 32

// KEYWORD: for
// Runs 3 times (k=0 to 2).
var sum_for = 0;
for (var k = 0; k < 3; k = k + 1) {
  // sum_for = 0 + 1 + 2 = 3
  sum_for = sum_for + k;
}
printf("%ld\n", sum_for); // Expected: 3