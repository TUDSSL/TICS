#ifndef CHECKPOINT_DATA_H__
#define CHECKPOINT_DATA_H__

#include <string.h>

#include "checkpoint_ctrl.h"

extern struct cpctrl_data DataCtrl;

static inline void checkpoint_data_set_dst(char *dst_data, char *dst_bss)
{
    DataCtrl.dst_data = dst_data;
    DataCtrl.dst_bss = dst_bss;
}

#define checkpoint_data_set_src checkpoint_data_set_dst

static inline void checkpoint_data(void)
{
    memcpy(DataCtrl.dst_data, DataCtrl.data_start, DataCtrl.data_size);
    memcpy(DataCtrl.dst_bss, DataCtrl.bss_start, DataCtrl.bss_size);
}

static inline void checkpoint_restore_data(void)
{
    memcpy(DataCtrl.data_start, DataCtrl.dst_data, DataCtrl.data_size);
    memcpy(DataCtrl.bss_start, DataCtrl.dst_bss, DataCtrl.bss_size);
}


#endif /* CHECKPOINT_DATA_H__ */
