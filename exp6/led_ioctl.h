#include <linux/ioctl.h>

struct   digit_cell_ctx{
    char Index;
    char Digit;
};

#define LED_APPLY _IOW(172 , 1, struct digit_cell_ctx*)


