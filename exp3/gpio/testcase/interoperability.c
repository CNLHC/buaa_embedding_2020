#include "stdio.h"
#include "util.h"

int main(){
    bind_key_led(555,333);
    int a = key_to_led(555);
    printf("%d",a);
}
