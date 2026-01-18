#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;

static void event_handler_cb_main_obj0(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        action_btn_click(e);
    }
}

static void event_handler_cb_main_obj1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        action_sw_change(e);
    }
}

static void event_handler_cb_main_slider_01(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_slider_get_value(ta);
            assignIntegerProperty(flowState, 9, 3, value, "Failed to assign Value in Slider widget");
        }
    }
}

void create_screen_main() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1280, 720);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 71, 24);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Hello, world!");
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 66, 111);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, event_handler_cb_main_obj0, LV_EVENT_ALL, flowState);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Button");
                }
            }
        }
        {
            lv_obj_t *obj = lv_arc_create(parent_obj);
            lv_obj_set_pos(obj, 71, 202);
            lv_obj_set_size(obj, 150, 150);
            lv_arc_set_value(obj, 25);
        }
        {
            lv_obj_t *obj = lv_switch_create(parent_obj);
            objects.obj1 = obj;
            lv_obj_set_pos(obj, 304, 111);
            lv_obj_set_size(obj, 110, 50);
            lv_obj_add_event_cb(obj, event_handler_cb_main_obj1, LV_EVENT_ALL, flowState);
        }
        {
            lv_obj_t *obj = lv_checkbox_create(parent_obj);
            lv_obj_set_pos(obj, 499, 111);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_checkbox_set_text(obj, "Checkbox");
        }
        {
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.obj2 = obj;
            lv_obj_set_pos(obj, 45, 360);
            lv_obj_set_size(obj, 555, 335);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // textarea_01
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.textarea_01 = obj;
            lv_obj_set_pos(obj, 264, 202);
            lv_obj_set_size(obj, 324, 150);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
        }
        {
            // Slider_01
            lv_obj_t *obj = lv_slider_create(parent_obj);
            objects.slider_01 = obj;
            lv_obj_set_pos(obj, 1197, 40);
            lv_obj_set_size(obj, 50, 625);
            lv_slider_set_range(obj, 0, 255);
            lv_obj_add_event_cb(obj, event_handler_cb_main_slider_01, LV_EVENT_ALL, flowState);
        }
        {
            // Label_ Slider01
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label__slider01 = obj;
            lv_obj_set_pos(obj, 1174, 16);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
        }
        {
            // Textarea_02
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.textarea_02 = obj;
            lv_obj_set_pos(obj, 985, 16);
            lv_obj_set_size(obj, 150, 70);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
        }
        {
            // Label_01
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_01 = obj;
            lv_obj_set_pos(obj, 708, 111);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
        }
        {
            // Label_02
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_02 = obj;
            lv_obj_set_pos(obj, 708, 202);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Text");
        }
    }
    lv_keyboard_set_textarea(objects.obj2, objects.textarea_01);
    
    tick_screen_main();
}

void tick_screen_main() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
    {
        int32_t new_val = evalIntegerProperty(flowState, 9, 3, "Failed to evaluate Value in Slider widget");
        int32_t cur_val = lv_slider_get_value(objects.slider_01);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.slider_01;
            lv_slider_set_value(objects.slider_01, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 10, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.label__slider01);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label__slider01;
            lv_label_set_text(objects.label__slider01, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 12, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.label_01);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_01;
            lv_label_set_text(objects.label_01, new_val);
            tick_value_change_obj = NULL;
        }
    }
}


static const char *screen_names[] = { "Main" };
static const char *object_names[] = { "main", "obj0", "obj1", "textarea_01", "slider_01", "label__slider01", "textarea_02", "label_01", "label_02", "obj2" };


typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    eez_flow_init_screen_names(screen_names, sizeof(screen_names) / sizeof(const char *));
    eez_flow_init_object_names(object_names, sizeof(object_names) / sizeof(const char *));
    
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}
