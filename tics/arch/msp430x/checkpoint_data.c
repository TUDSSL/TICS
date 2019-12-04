#include <string.h>

#include "nvm.h"
#include "checkpoint_ctrl.h"

extern char __datastart;
extern char __dataend;
extern size_t __datasize;

extern char __bssstart;
extern char __bssend;
extern size_t __bsssize;

NVM struct cpctrl_data DataCtrl = {
    .dst_data = NULL,
    .data_start = &__datastart,
    .data_size = (size_t)&__datasize,

    .dst_bss = NULL,
    .bss_start = &__bssstart,
    .bss_size = (size_t)&__bsssize,
};
