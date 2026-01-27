#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_COUNTER2 = 0
};

// Native global variables

extern int32_t get_var_counter();
extern void set_var_counter(int32_t value);
extern bool get_var_flag();
extern void set_var_flag(bool value);
extern int32_t get_var_light();
extern void set_var_light(int32_t value);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/