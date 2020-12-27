#include <linux/ioctl.h>

typedef struct {
    char Index;
    char Digit;
}digit_cell_ctx;

#define LED_APPLY _IOW(172,1, (digit_cell_ctx *))


