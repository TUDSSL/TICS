#include <stdlib.h>

#include "nvm.h"
#include "checkpoint_ctrl.h"

volatile reg_t *RegisterCtrl_dst = NULL;
volatile reg_t *RegisterCtrl_pc_save = NULL;
volatile reg_t *RegisterCtrl_sp_save = NULL;
