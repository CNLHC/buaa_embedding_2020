#include <linux/ioctl.h>

struct   digit_cell_ctx{
    char Index;
    char Digit;
}digit_cell_ctx;

#define LED_APPLY _IOW(172 , 1, struct digit_cell_ctx*)


