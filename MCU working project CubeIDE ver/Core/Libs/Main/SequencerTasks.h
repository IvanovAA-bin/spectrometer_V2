#ifndef _SEQUENCER_TASKS_H_
#define _SEQUENCER_TASKS_H_

#include "main.h"
#include "System.h"
#include "ble_types.h"
#include "app_conf.h"
#include "custom_stm.h"

void ST_rf_ProcessFirstCharWrite (void);
void ST_rf_ProcessSecondCharWrite (void);
void ST_rf_UpdateCPS (void);

void ST_fs_Initialize (void);
void ST_fs_SaveSpectrPostDelayed (void);

#endif
