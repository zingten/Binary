#include <pebble.h>

static const uint32_t vibe_connect_data[] = {50, 50, 50};
static VibePattern vibe_connect = {
    .durations = vibe_connect_data,
    .num_segments = 3
};
static const uint32_t vibe_disconnect_data[] = {50, 50, 50, 50, 50};
static VibePattern vibe_disconnect = {
    .durations = vibe_disconnect_data,
    .num_segments = 5
};
static const uint32_t vibe_quarter_data[] = {500, 250, 100};
static VibePattern vibe_quarter = {
    .durations = vibe_quarter_data,
    .num_segments = 3
};
static const uint32_t vibe_half_data[] = {100, 250, 500};
static VibePattern vibe_half = {
    .durations = vibe_half_data,
    .num_segments = 3
};
static const uint32_t vibe_threequarter_data[] = {500, 250, 500};
static VibePattern vibe_threequarter = {
    .durations = vibe_threequarter_data,
    .num_segments = 3
};

#define VIBE_HOUR_ONE 2
#define VIBE_HOUR_TWO 4
#define VIBE_HOUR_FOUR 6
#define VIBE_HOUR_EIGHT 8
#define VIBE_HOUR_LONG 500
#define VIBE_HOUR_SHORT 100
static uint32_t vibe_hour_data[] = {1000, 1000, VIBE_HOUR_SHORT, 350, VIBE_HOUR_SHORT, 350, VIBE_HOUR_SHORT, 350, VIBE_HOUR_SHORT};
static VibePattern vibe_hour = {
    .durations = vibe_hour_data,
    .num_segments = 9
};