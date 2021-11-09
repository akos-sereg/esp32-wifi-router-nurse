#include "include/app_main.h"

void main_task()
{
    int tick_rate_ms = CHECK_ITERATION_MS;
    int disconnected_since_ms = 0;
    int relaxing_ms = -1;

    while(1) {
        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);

        // we have just restarted the router; relaxing, waiting for wifi router to start
        if (relaxing_ms > -1) {
            relaxing_ms += tick_rate_ms;
            if (relaxing_ms > RELAX_TIME_AFTER_ROUTER_RESTART_MS) {
                relaxing_ms = -1;
            }
            continue;
        }

        // measure downtime
        if (!IS_WIFI_CONNECTED) {
            disconnected_since_ms += tick_rate_ms;
        } else {
            disconnected_since_ms = 0;
        }

        // act once downtime exceeds threshold
        if (disconnected_since_ms >= RESTART_WIFI_ROUTER_AFTER_DOWNTIME_MS) {
            gpio_set_level(GPIO_RELAY, 1);
            vTaskDelay(RESTART_WIFI_ROUTER_KEEP_HOLDING_UNPLUGGED_MS / portTICK_RATE_MS);
            gpio_set_level(GPIO_RELAY, 0);
            disconnected_since_ms = 0;
            relaxing_ms = 0;
        }
    }
}

void app_main()
{
    // initialize relay
    gpio_pad_select_gpio(GPIO_RELAY);
    gpio_set_direction(GPIO_RELAY, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_RELAY, 0);

    // wifi
    networking_initialize_wifi();

    main_task();
}