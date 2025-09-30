#pragma once

#include "sys_constants.h"

#define WIFI_MODE                               (WIFI_AP_MODE)

#define WEB_AUTH                                (1)                     // Use basic auth for non-local connections

#define WIFI_CONNECTION_CHECK_INTERVAL          (5000u)                 // Interval (ms) between Wi-Fi connection check
#define WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL    (120000u)               // Max time (ms) to wait for Wi-Fi connection before switch to AP mode
                                                                        // 0 - Newer switch to AP mode

#define MDNS_NAME                               "esp_pid"

#define TIME_ZONE                               (5.f)                   // GMT +5:00

#define DSX18_PIN                               (2u)
#define ANALOG_PIN                              (2u)
#define PWM_PIN                                 (0u)

#define HISTORY_COUNT                           (128u)

#define MQTT                                    (0)                     // Enable MQTT server

#define MQTT_CONNECTION_TIMEOUT                 (15000u)                // Connection attempt timeout to MQTT server
#define MQTT_RECONNECT_TIMEOUT                  (5000u)                 // Time before new reconnection attempt to MQTT server

#define MQTT_INVERT_POSITION                    (true)

#define MQTT_PREFIX                             ""
#define MQTT_TOPIC_POWER                        MQTT_PREFIX "/power"
#define MQTT_TOPIC_NIGHT_MODE                   MQTT_PREFIX "/night_mode"

#define MQTT_OUT_PREFIX                         MQTT_PREFIX "/out"
#define MQTT_OUT_TOPIC_POWER                    MQTT_OUT_PREFIX "/power"
#define MQTT_OUT_TOPIC_SENSOR                   MQTT_OUT_PREFIX "/sensor"
#define MQTT_OUT_TOPIC_CONTROL                  MQTT_OUT_PREFIX "/control"
#define MQTT_OUT_TOPIC_NIGHT_MODE               MQTT_OUT_PREFIX "/night_mode"

#include "./_override/credentials.h"