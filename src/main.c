#include <pebble.h>
#include "colors.h"
#include "vibes.c"

#define YEAR 0
#define MONTH 1
#define DAY 2
#define HOUR 3
#define MINUTE 4
#define SECOND 5

static Window* s_main_window;
static Layer* main_layer;
static Layer* year_layer;
static Layer* month_layer;
static Layer* day_layer;
static Layer* hour_layer;
static Layer* minute_layer;
static Layer* second_layer;

static bool bluetooth_connected = true;
static TimeUnits to_change = 0;
static bool started = false;

static void tick_handler (struct tm *tick_time, TimeUnits units_changed) {
    uint8_t ones = 0;
    uint8_t tens = 0;
    to_change = units_changed;
    if (units_changed & SECOND_UNIT) {
        ones = tick_time->tm_sec % 10;
        tens = ((tick_time->tm_sec % 100) - ones) / 10;
        *(uint8_t*)(layer_get_data (second_layer)) = (ones << 4) + tens;
        layer_mark_dirty (second_layer);
    } 
    if (units_changed & MINUTE_UNIT) {
        ones = tick_time->tm_min % 10;
        tens = ((tick_time->tm_min % 100) - ones) / 10;
        *(uint8_t*)(layer_get_data (minute_layer)) = (ones << 4) + tens;
        layer_mark_dirty (minute_layer);
        if (started) {
            if (ones == 5 && tens == 1) {
                vibes_enqueue_custom_pattern (vibe_quarter);
            } else if (ones == 0 && tens == 3) {
                vibes_enqueue_custom_pattern (vibe_half);
            } else if (ones == 5 && tens == 4) {
                vibes_enqueue_custom_pattern (vibe_threequarter);
            }
        }
    } 
    if (units_changed & HOUR_UNIT) {
        ones = tick_time->tm_hour % 10;
        tens = ((tick_time->tm_hour % 100) - ones) / 10;
        *(uint8_t*)(layer_get_data (hour_layer)) = (ones << 4) + tens;
        layer_mark_dirty (hour_layer);
        
        if (started) {
            uint8_t h = ones + (tens * 10);
            if (h > 12) {
                h -= 12;
            }
            if ((h & 1) != 0) {
                vibe_hour_data[VIBE_HOUR_ONE] = VIBE_HOUR_LONG;
            } else {
                vibe_hour_data[VIBE_HOUR_ONE] = VIBE_HOUR_SHORT;
            }
            if ((h & 2) != 0) {
                vibe_hour_data[VIBE_HOUR_TWO] = VIBE_HOUR_LONG;
            } else {
                vibe_hour_data[VIBE_HOUR_TWO] = VIBE_HOUR_SHORT;
            }
            if ((h & 4) != 0) {
                vibe_hour_data[VIBE_HOUR_FOUR] = VIBE_HOUR_LONG;
            } else {
                vibe_hour_data[VIBE_HOUR_FOUR] = VIBE_HOUR_SHORT;
            }
            if ((h & 8) != 0) {
                vibe_hour_data[VIBE_HOUR_EIGHT] = VIBE_HOUR_LONG;
            } else {
                vibe_hour_data[VIBE_HOUR_EIGHT] = VIBE_HOUR_SHORT;
            }
            vibes_enqueue_custom_pattern (vibe_hour);
        }
    }
    if (units_changed & DAY_UNIT) {
        ones = tick_time->tm_mday % 10;
        tens = ((tick_time->tm_mday % 100) - ones) / 10;
        *(uint8_t*)(layer_get_data (day_layer)) = (ones << 4) + tens;
        layer_mark_dirty (day_layer);
    } 
    if (units_changed & MONTH_UNIT) {
        ones = (tick_time->tm_mon + 1) % 10;
        tens = (((tick_time->tm_mon + 1) % 100) - ones) / 10;
        *(uint8_t*)(layer_get_data (month_layer)) = (ones << 4) + tens;
        layer_mark_dirty (month_layer);
    }
    if (units_changed & YEAR_UNIT) {
        ones = tick_time->tm_year % 10;
        tens = ((tick_time->tm_year % 100) - ones) / 10;
        *(uint8_t*)(layer_get_data (year_layer)) = (ones << 4) + tens;
        layer_mark_dirty (year_layer);
    }
}

static void canvas_update_proc (Layer* layer, GContext* ctx) {
    GRect bounds = layer_get_bounds (layer);
    uint8_t y = 0;
    uint8_t dX = bounds.size.w / 2;
    uint8_t dY = bounds.size.h / 4;
    int8_t dYr = dY;
    
    uint8_t* layer_type = layer_get_data (layer) + 1;
    
    GColor color_on;
    GColor color_off;
    
    if (*layer_type == YEAR) {
        y = dY * 3;
        dYr *= -1;
        if (bluetooth_connected) {
            color_on = COLOR_X1;
            color_off = COLOR_OFF_X1;
        } else {
            color_on = COLOR_Y1;
            color_off = COLOR_OFF_Y1;
        }
    } else if (*layer_type == MONTH) {
        y = dY * 3;
        dYr *= -1;
        if (bluetooth_connected) {
            color_on = COLOR_X2;
            color_off = COLOR_OFF_X2;
        } else {
            color_on = COLOR_Y2;
            color_off = COLOR_OFF_Y2;
        }
    } else if (*layer_type == DAY) {
        y = dY * 3;
        dYr *= -1;
        if (bluetooth_connected) {
            color_on = COLOR_X3;
            color_off = COLOR_OFF_X3;
        } else {
            color_on = COLOR_Y3;
            color_off = COLOR_OFF_Y3;
        }
    } else if (*layer_type == HOUR) {
        if (bluetooth_connected) {
            color_on = COLOR_Y1;
            color_off = COLOR_OFF_Y1;
        } else {
            color_on = COLOR_X1;
            color_off = COLOR_OFF_X1;
        }
    } else if (*layer_type == MINUTE) {
        if (bluetooth_connected) {
            color_on = COLOR_Y2;
            color_off = COLOR_OFF_Y2;
        } else {
            color_on = COLOR_X2;
            color_off = COLOR_OFF_X2;
        }
    } else {
        if (bluetooth_connected) {
            color_on = COLOR_Y3;
            color_off = COLOR_OFF_Y3;
        } else {
            color_on = COLOR_X3;
            color_off = COLOR_OFF_X3;
        }
    }
    
    graphics_context_set_fill_color (ctx, color_off);
    graphics_fill_rect (ctx, bounds, 0, GCornersAll);
    graphics_context_set_fill_color (ctx, color_on);
    
    uint8_t ones = *(layer_type - 1) >> 4;
    uint8_t tens = *(layer_type - 1) & 15;
    
    if ((tens & 1) != 0) {
        graphics_fill_rect (ctx, GRect (
        0, y, dX, dY
        ), 0, GCornersAll);
    }
    
    if ((tens & 2) != 0) {
        graphics_fill_rect (ctx, GRect (
        0, y + dYr, dX, dY
        ), 0, GCornersAll);
    }
    
    if ((tens & 4) != 0) {
        graphics_fill_rect (ctx, GRect (
        0, y + dYr * 2, dX, dY
        ), 0, GCornersAll);
    }
    
    if ((ones & 1) != 0) {
        graphics_fill_rect (ctx, GRect (
        dX, y, dX, dY
        ), 0, GCornersAll);
    }
    
    if ((ones & 2) != 0) {
        graphics_fill_rect (ctx, GRect (
        dX, y + dYr, dX, dY
        ), 0, GCornersAll);
    }
    
    if ((ones & 4) != 0) {
        graphics_fill_rect (ctx, GRect (
        dX, y + dYr * 2, dX, dY
        ), 0, GCornersAll);
    }
    
    if ((ones & 8) != 0) {
        graphics_fill_rect (ctx, GRect (
        dX, y + dYr * 3, dX, dY
        ), 0, GCornersAll);
    }
}

static void bluetooth_changed (bool connected) {
    bluetooth_connected = connected;
    if (!started) {
        return;
    }
    if (connected) {
        vibes_enqueue_custom_pattern (vibe_connect);
    } else {
        vibes_enqueue_custom_pattern (vibe_disconnect);
    }
}

static void main_window_load (Window* window) {
    main_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds (main_layer);
    
    uint8_t* layer_type;
    
    year_layer = layer_create_with_data (GRect (0, 0, bounds.size.w / 3, bounds.size.h / 2), 2);
    layer_type = layer_get_data (year_layer) + 1;
    *layer_type = YEAR;
    layer_add_child (main_layer, year_layer);
    
    month_layer = layer_create_with_data (GRect (bounds.size.w / 3, 0, bounds.size.w / 3, bounds.size.h / 2), 2);
    layer_type = layer_get_data (month_layer) + 1;
    *layer_type = MONTH;
    layer_add_child (main_layer, month_layer);
    
    day_layer = layer_create_with_data (GRect (bounds.size.w * 2 / 3, 0, bounds.size.w / 3, bounds.size.h / 2), 2);
    layer_type = layer_get_data (day_layer) + 1;
    *layer_type = DAY;
    layer_add_child (main_layer, day_layer);
    
    hour_layer = layer_create_with_data (GRect (0, bounds.size.h / 2, bounds.size.w / 3, bounds.size.h / 2), 2);
    layer_type = layer_get_data (hour_layer) + 1;
    *layer_type = HOUR;
    layer_add_child (main_layer, hour_layer);
    
    minute_layer = layer_create_with_data (GRect (bounds.size.w / 3, bounds.size.h / 2, bounds.size.w / 3, bounds.size.h / 2), 2);
    layer_type = layer_get_data (minute_layer) + 1;
    *layer_type = MINUTE;
    layer_add_child (main_layer, minute_layer);
    
    second_layer = layer_create_with_data (GRect (bounds.size.w * 2 / 3, bounds.size.h / 2, bounds.size.w / 3, bounds.size.h / 2), 2);
    layer_type = layer_get_data (second_layer) + 1;
    *layer_type = SECOND;
    layer_add_child (main_layer, second_layer);
    
    layer_set_update_proc (year_layer, canvas_update_proc);
    layer_set_update_proc (month_layer, canvas_update_proc);
    layer_set_update_proc (day_layer, canvas_update_proc);
    layer_set_update_proc (hour_layer, canvas_update_proc);
    layer_set_update_proc (minute_layer, canvas_update_proc);
    layer_set_update_proc (second_layer, canvas_update_proc);
}

static void main_window_unload (Window* window) {
    layer_destroy (second_layer);
    layer_destroy (minute_layer);
    layer_destroy (hour_layer);
    layer_destroy (day_layer);
    layer_destroy (month_layer);
    layer_destroy (year_layer);
}

static void init () {
    s_main_window = window_create ();
    window_set_window_handlers (s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    window_stack_push (s_main_window, true);
    
    tick_timer_service_subscribe (SECOND_UNIT, tick_handler);
    connection_service_subscribe ((ConnectionHandlers) {
        .pebble_app_connection_handler = bluetooth_changed
    });
    
    time_t rawtime;
    struct tm* timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    tick_handler (timeinfo, SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT | MONTH_UNIT | YEAR_UNIT);
    bluetooth_changed (connection_service_peek_pebble_app_connection ());
    started = true;
}

static void deinit () {
    connection_service_unsubscribe ();
    tick_timer_service_unsubscribe ();
    
    window_destroy (s_main_window);
}

int main () {
    init ();
    app_event_loop ();
    deinit ();
    return 0;
}