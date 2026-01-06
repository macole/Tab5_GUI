#include "lvgl.h"
#include "actions.h"
#include "screens.h"
#include "ui.h"

extern int32_t counter;

void action_screen_change(lv_event_t *e) {
    // TODO: Implement action screen_change here
    counter++;
}

void action_screen_back(lv_event_t *e) {
    // TODO: Implement action screen_back here
    counter--;
}
