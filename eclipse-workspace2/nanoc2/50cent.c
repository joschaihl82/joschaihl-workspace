#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool is_prime(int n){ if(n<=1) return false; if(n%2==0) return n==2; for(int i=3;i*i<=n;i+=2) if(n%i==0) return false; return true; }
long fact(int n){ if(n<0) return -1; long r=1; for(int i=2;i<=n;i++) r*=i; return r; }
void reverse_inplace(char *s){ int i=0,j=strlen(s)-1; while(i<j){ char t=s[i]; s[i]=s[j]; s[j]=t; i++; j--; } }
int sum_array(int *a,int n){ int s=0; for(int i=0;i<n;i++) s+=a[i]; return s; }

void expect_int(const char *name,long got,long want){ printf("%-20s : %s (got=%ld, want=%ld)\n",name, got==want?"PASS":"FAIL", got,want); }
void expect_bool(const char *name,bool got,bool want){ printf("%-20s : %s (got=%s, want=%s)\n",name, got==want?"PASS":"FAIL", got?"true":"false", want?"true":"false"); }
void expect_str(const char *name,const char *got,const char *want){ printf("%-20s : %s (got=\"%s\", want=\"%s\")\n",name, strcmp(got,want)==0?"PASS":"FAIL", got,want); }

int main(void){
    expect_bool("prime 2", is_prime(2), true);
    expect_bool("prime 3", is_prime(3), true);
    expect_bool("prime 4", is_prime(4), false);
    expect_bool("prime 17", is_prime(17), true);

    expect_int("fact 0", fact(0), 1);
    expect_int("fact 5", fact(5), 120);
    expect_int("fact -1", fact(-1), -1);

    char s1[]="hello"; reverse_inplace(s1); expect_str("reverse hello", s1, "olleh");
    char s2[]=""; reverse_inplace(s2); expect_str("reverse empty", s2, "");

    int a1[]={1,2,3,4}; expect_int("sum 1..4", sum_array(a1,4), 10);
    int a2[]={-1,1,-2,2}; expect_int("sum mixed", sum_array(a2,4), 0);

    // edge cases
    expect_bool("prime 0", is_prime(0), false);
    expect_bool("prime 1", is_prime(1), false);
    expect_int("fact 10", fact(10), 3628800);
    char s3[]="a"; reverse_inplace(s3); expect_str("reverse a", s3, "a");

    int a3[]={0}; expect_int("sum single", sum_array(a3,1), 0);
    int a4[]={100,-50}; expect_int("sum two", sum_array(a4,2), 50);

    expect_bool("prime 19", is_prime(19), true);
    expect_bool("prime 20", is_prime(20), false);
    expect_int("fact 12", fact(12), 479001600);
    char s4[]="racecar"; reverse_inplace(s4); expect_str("reverse racecar", s4, "racecar");
    char s5[]="a b"; reverse_inplace(s5); expect_str("reverse a b", s5, "b a");
    expect_int("sum empty", sum_array(NULL,0), 0);

    printf("All tests executed.\n");
    return 0;
}
