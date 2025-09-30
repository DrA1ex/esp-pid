import {PacketType} from "./cmd.js";

function fix_float(value) {
    const fixed = value.toFixed(4);

    let i;
    for (i = fixed.length - 1; i >= 0; --i) {
        if(fixed[i] !== '0') break;
    }

    if (fixed[i] === '.') i--;

    if (i < 0) return "0";
    return fixed.slice(0, i + 1);
}

/**@type {PropertiesConfig} */
export const PropertyConfig = [{
    key: "status", section: "Status", props: [
        {
            key: "status.sensor_value", type: "label", kind: "Float32",
            cmd: PacketType.SENSOR_VALUE,
            displayConverter: (value) => [
                "Sensor:",
                `${value.toFixed(2)}`
            ]
        }, {
            key: "status.control_value", type: "label", kind: "Float32",
            cmd: PacketType.CONTROL_VALUE,
            displayConverter: (value) => [
                "Control:",
                `${(value * 100).toFixed(0)}%`
            ]
        },

        {key: "status.history", type: "chart", kind: "Binary", cmd: PacketType.HISTORY_DATA},
    ]
}, {
    key: "general", section: "General", props: [
        {key: "power", type: "trigger", title: "Power", kind: "Boolean", cmd: PacketType.POWER},
    ],
}, {
    key: "night_mode", section: "Night Mode", collapse: true, props: [
        {key: "nightMode.enabled", title: "Enabled", type: "trigger", kind: "Boolean", cmd: PacketType.NIGHT_MODE_ENABLED},
        {key: "nightMode.startTime", title: "Start Time", type: "time", kind: "Uint32", cmd: PacketType.NIGHT_MODE_START},
        {key: "nightMode.endTime", title: "End Time", type: "time", kind: "Uint32", cmd: PacketType.NIGHT_MODE_END},
    ]
}, {
    key: "sensor", section: "Sensor", collapse: true, props: [
        {key: "sensor.type", title: "Type", type: "select", kind: "Uint8", list: "sensorType", cmd: PacketType.SENSOR_TYPE},

        // ANALOG
        {key: "sensor.parsed.analog", type: "skip"},
        {key: "sensor.parsed.analog.pin", title: "Pin", type: "int", kind: "Uint8", cmd: PacketType.ANALOG_SENSOR_PIN, visibleIf: "sensor.parsed.analog"},
        {key: "sensor.parsed.analog.resolution", title: "Resolution", type: "int", kind: "Uint8", min: 8, limit: 14, cmd: PacketType.ANALOG_SENSOR_RESOLUTION, visibleIf: "sensor.parsed.analog"},

        // DSX18X
        {key: "sensor.parsed.dsx18x", type: "skip"},
        {key: "sensor.parsed.dsx18x.pin", title: "Pin", type: "int", kind: "Uint8", cmd: PacketType.DSX18_SENSOR_PIN, visibleIf: "sensor.parsed.dsx18x"},
        {key: "sensor.parsed.dsx18x.resolution", title: "Resolution", type: "int", kind: "Uint8", min: 9, limit: 12, cmd: PacketType.DSX18_SENSOR_RESOLUTION, visibleIf: "sensor.parsed.dsx18x"},
        {key: "sensor.parsed.dsx18x.parasite", title: "Parasite power", type: "trigger", kind: "Boolean", cmd: PacketType.DSX18_SENSOR_PARASITE, visibleIf: "sensor.parsed.dsx18x"},

        {type: "title", label: "Actions", extra: {m_top: true}},
        {key: "apply_control_config", type: "button", label: "Apply"},
    ]
}, {
    key: "control", section: "Control", collapse: true, props: [
        {key: "control.type", title: "Type", type: "select", kind: "Uint8", list: "controlType", cmd: PacketType.CONTROL_TYPE},

        // PWM
        {key: "control.parsed.pwm", type: "skip"},
        {key: "control.parsed.pwm.pin", title: "Pin", type: "int", kind: "Uint8", cmd: PacketType.PWM_CONTROL_PIN, visibleIf: "control.parsed.pwm"},
        {key: "control.parsed.pwm.period", title: "Period", type: "int", kind: "Uint16", cmd: PacketType.PWM_CONTROL_PERIOD, visibleIf: "control.parsed.pwm"},

        {type: "title", label: "Actions", extra: {m_top: true}},
        {key: "apply_sensor_config", type: "button", label: "Apply"},
    ]
}, {
    key: "pid", section: "PID-R", collapse: true, props: [
        {key: "pid.target", title: "Target value", type: "float", kind: "Float32", cmd: PacketType.PID_TARGET},
        {key: "pid.interval", title: "Refresh interval", type: "int", kind: "Uint16", cmd: PacketType.PID_INTERVAL},
        {type: "title", label: "Coefficients"},
        {key: "pid.p", title: "kP", type: "float", kind: "Float32", cmd: PacketType.PID_P, transform: fix_float},
        {key: "pid.i", title: "kI", type: "float", kind: "Float32", cmd: PacketType.PID_I, transform: fix_float},
        {key: "pid.d", title: "kD", type: "float", kind: "Float32", cmd: PacketType.PID_D, transform: fix_float},
        {type: "title", label: "Advanced"},
        {key: "pid.reverse", title: "Reverse", type: "trigger", kind: "Boolean", cmd: PacketType.PID_REVERSE}
    ]
}, {
    key: "system", section: "System Settings", collapse: true, props: [
        {key: "sysConfig.mdnsName", title: "mDNS Name", type: "text", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_MDNS_NAME},

        {type: "title", label: "WiFi"},
        {key: "sysConfig.wifiMode", title: "Mode", type: "select", kind: "Uint8", cmd: PacketType.SYS_CONFIG_WIFI_MODE, list: "wifiMode"},
        {key: "sysConfig.wifiSsid", title: "SSID", type: "text", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_WIFI_SSID},
        {key: "sysConfig.wifiPassword", title: "Password", type: "password", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_WIFI_PASSWORD},

        {type: "title", label: "WiFi Advanced"},
        {key: "sysConfig.wifiConnectionCheckInterval", title: "Connection Check Interval", type: "int", kind: "Uint32", cmd: PacketType.SYS_CONFIG_WIFI_CONNECTION_CHECK_INTERVAL},
        {key: "sysConfig.wifiMaxConnectionAttemptInterval", title: "Max Connection Attempt Interval", type: "int", kind: "Uint32", cmd: PacketType.SYS_CONFIG_WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL},

        {type: "title", label: "Time"},
        {key: "sysConfig.timeZone", title: "Time Zone", type: "float", kind: "Float32", cmd: PacketType.SYS_CONFIG_TIME_ZONE},

        {type: "title", label: "MQTT"},
        {key: "sysConfig.mqtt", title: "Enabled", type: "trigger", kind: "Boolean", cmd: PacketType.SYS_CONFIG_MQTT_ENABLED},
        {key: "sysConfig.mqttHost", title: "Host", type: "text", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_MQTT_HOST},
        {key: "sysConfig.mqttPort", title: "Port", type: "int", kind: "Uint16", cmd: PacketType.SYS_CONFIG_MQTT_PORT},
        {key: "sysConfig.mqttUser", title: "User", type: "text", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_MQTT_USER},
        {key: "sysConfig.mqttPassword", title: "Password", type: "password", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_MQTT_PASSWORD},

        {type: "title", label: "Actions", extra: {m_top: true}},
        {key: "apply_sys_config", type: "button", label: "Apply"},
    ]
}];
