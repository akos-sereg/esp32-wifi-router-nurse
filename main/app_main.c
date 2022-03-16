#include "include/app_main.h"
#include "uconfy.h"

void main_task()
{
    int tick_rate_ms = CHECK_ITERATION_MS;
    int disconnected_since_ms = 0;
    int relaxing_ms = -1;
    int router_restart_report_required = 0;
    int router_restart_report_completed = 0;
    int uptime_seconds = 0;

    while(1) {
        printf("disconnected_since_ms: %d, relaxing_ms: %d, UCONFIG_IS_WIFI_CONNECTED: %d\n", disconnected_since_ms, relaxing_ms, UCONFIG_IS_WIFI_CONNECTED);
        vTaskDelay(tick_rate_ms / portTICK_RATE_MS);
        uptime_seconds += tick_rate_ms / 1000;

        if (uptime_seconds > 18000) {
            uconfy_log("5 hour check-in");
            uptime_seconds = 0;
        }

        // we have just restarted the router; relaxing, waiting for wifi router to start
        if (relaxing_ms > -1) {
            relaxing_ms += tick_rate_ms;
            if (relaxing_ms > RELAX_TIME_AFTER_ROUTER_RESTART_MS) {
                relaxing_ms = -1;
                printf("Relaxing period ended.\n");
            }
            continue;
        }

        // measure downtime
        if (!UCONFIG_IS_WIFI_CONNECTED) {
            disconnected_since_ms += tick_rate_ms;
        } else {
            if (router_restart_report_required && !router_restart_report_completed) {
                router_restart_report_required = 0;
                router_restart_report_completed = 1;
                uconfy_log("Router restarted, and just connected to WIFI");
            }
            disconnected_since_ms = 0;
        }

        // act once downtime exceeds threshold
        if (disconnected_since_ms >= RESTART_WIFI_ROUTER_AFTER_DOWNTIME_MS) {
            printf("Wifi is disconnected for a while. Restarting router ...\n");
            gpio_set_level(GPIO_RELAY, 1);
            vTaskDelay(RESTART_WIFI_ROUTER_KEEP_HOLDING_UNPLUGGED_MS / portTICK_RATE_MS);
            gpio_set_level(GPIO_RELAY, 0);
            printf("Router restarted.\n");
            disconnected_since_ms = 0;
            relaxing_ms = 0;
            printf("Starting relaxing period.\n");

            router_restart_report_required = 1;
            router_restart_report_completed = 0;
        }
    }
}

void wifi_connected() {
    uconfy_fetch_configs(NULL);
    uconfy_log("Device started, and connected to WIFI");
}

void app_main()
{
    // initialize relay
    gpio_pad_select_gpio(GPIO_RELAY);
    gpio_set_direction(GPIO_RELAY, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_RELAY, 0);

    // wifi
    uconfy_load_from_nvs();
    uconfy_initialize_wifi(EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS, 1, &wifi_connected);
    uconfy_init(UCONFY_DEVICE_ID, UCONFY_API_KEY);

    main_task();
}