/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-03-21     Tristan       the first version
 */

#include "mc_user_definitions.h"


#include "mc_svm.h"
#include "mc_current.h"
#include "mc_control.h"
#include "mc_transform.h"

typedef enum mc_foc_enable_state_s
{
    MC_FOC_ENABLE,
    MC_FOC_DISABLE
} mc_foc_enable_state_s;

typedef enum mc_foc_control_state_s
{
    MC_FOC_MODE_OPEN_LOOP,
    MC_FOC_MODE_TORQUE_CNTRL,
    MC_FOC_MODE_SPEED_CNTRL
} mc_foc_control_state_s;

typedef struct mc_foc_context_t
{
    mc_foc_enable_state_s state;
    rt_uint32_t control_hook;
} mc_foc_context_t;

void mc_foc_init(void);
