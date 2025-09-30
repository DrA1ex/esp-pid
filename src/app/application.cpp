#include "application.h"

#include "poly_meta.h"

void Application::begin() {
    D_PRINT("Starting application...");

    if (!LittleFS.begin()) {
        D_PRINT("Unable to initialize FS");
    }

    _bootstrap = std::make_unique<Bootstrap<Config, PacketType>>(&LittleFS);

    auto &sys_config = _bootstrap->config().sys_config;
    _bootstrap->begin({
        .mdns_name = sys_config.mdns_name,
        .wifi_mode = sys_config.wifi_mode,
        .wifi_ssid = sys_config.wifi_ssid,
        .wifi_password = sys_config.wifi_password,
        .wifi_connection_timeout = sys_config.wifi_max_connection_attempt_interval,
        .mqtt_enabled = sys_config.mqtt,
        .mqtt_host = sys_config.mqtt_host,
        .mqtt_port = sys_config.mqtt_port,
        .mqtt_user = sys_config.mqtt_user,
        .mqtt_password = sys_config.mqtt_password,
    });

    _ntp_time = std::make_unique<NtpTime>();
    _night_mode_manager = std::make_unique<NightModeManager>(*_ntp_time, _bootstrap->timer(), _bootstrap->config());

    _night_mode_manager->event_night_mode().subscribe(this, [this](auto sender, auto state, auto arg) {
        _load();
    });

    if (config().regulator.sensor.type == SensorType::DSX18X) {
        _sensor = std::make_unique<DSx18Sensor>(_bootstrap->timer(), config().regulator.sensor.data);
        _sensor_meta = std::make_unique<MetaHolder<DSx18SensorConfigMeta>>(
            build_dsx18_sensor_metadata(*(DSx18SensorConfig *) config().regulator.sensor.data)
        );
    } else {
        _sensor = std::make_unique<AnalogSensor>(_bootstrap->timer(), config().regulator.sensor.data);
        _sensor_meta = std::make_unique<MetaHolder<AnalogSensorConfigMeta>>(
            build_analog_sensor_metadata(*(AnalogSensorConfig *) config().regulator.sensor.data)
        );
    }

    _sensor->begin();

    _control = std::make_unique<PwmControl>(_bootstrap->timer(), config().regulator.control.data);
    _control_meta = std::make_unique<MetaHolder<PwmControlConfigMeta>>(
        build_pwm_control_metadata(*(PwmControlConfig *) config().regulator.control.data)
    );

    _control->begin();

    _pid = std::make_unique<uPID>();

    _bootstrap->event_state_changed().subscribe(this, [this](auto sender, auto state, auto arg) {
        _bootstrap_state_changed(sender, state, arg);
    });

    _bootstrap->timer().add_interval([this](auto) { _notify_periodic_status(); }, APP_STATE_NOTIFICATION_INTERVAL);

    _bootstrap->event_state_changed().subscribe(this, BootstrapState::READY, [this](auto, auto, auto) {
        _on_bootstrap_ready();
    });

    _bootstrap->timer().add_interval([this](auto) { _service_loop(); }, APP_SERVICE_LOOP_INTERVAL);

    _setup();
    change_state(AppState::INITIALIZATION);
}

void Application::_setup() {
    NotificationBus::get().subscribe([this](auto sender, auto param) {
        if (sender != this) _handle_property_change(param);
    });

    auto &ws_server = _bootstrap->ws_server();
    auto &mqtt_server = _bootstrap->mqtt_server();

    auto visit_fn = [this, &ws_server, &mqtt_server](AbstractPropertyMeta *meta) {
        auto binary_protocol = (BinaryProtocolMeta<PacketType> *) meta->get_binary_protocol();
        if (binary_protocol->packet_type.has_value()) {
            ws_server->register_parameter(*binary_protocol->packet_type, meta->get_parameter());
            VERBOSE(D_PRINTF("WebSocket: Register property %s\r\n", __debug_enum_str(*binary_protocol->packet_type)));
        }

        auto mqtt_protocol = meta->get_mqtt_protocol();
        if (mqtt_protocol->topic_in && mqtt_protocol->topic_out) {
            mqtt_server->register_parameter(mqtt_protocol->topic_in, mqtt_protocol->topic_out, meta->get_parameter());
            VERBOSE(D_PRINTF("MQTT: Register property %s <-> %s\r\n", mqtt_protocol->topic_in, mqtt_protocol->topic_out));
        } else if (mqtt_protocol->topic_out) {
            mqtt_server->register_notification(mqtt_protocol->topic_out, meta->get_parameter());
            VERBOSE(D_PRINTF("MQTT: Register notification -> %s\r\n", mqtt_protocol->topic_out));
        }

        if (binary_protocol->packet_type.has_value()) {
            _parameter_to_packet[meta->get_parameter()] = binary_protocol->packet_type.value();
        }
    };

    _metadata = std::make_unique<ConfigMetadata>(build_metadata(config(), _runtime_info));
    _metadata->visit(visit_fn);

    _sensor_meta->visit(visit_fn);
    _control_meta->visit(visit_fn);

    ws_server->register_notification(PacketType::SENSOR_VALUE, _metadata->data.sensor_value);
    ws_server->register_notification(PacketType::CONTROL_VALUE, _metadata->data.control_value);
    ws_server->register_notification(PacketType::HISTORY_DATA, _metadata->data.history);

    ws_server->register_data_request(PacketType::GET_CONFIG, _metadata->data.config);
    ws_server->register_data_request(PacketType::GET_STATE, _metadata->data.state);

    ws_server->register_command(PacketType::RESTART, [this] { _bootstrap->restart(); });

    mqtt_server->register_notification(MQTT_OUT_TOPIC_SENSOR, _metadata->data.sensor_value);
    mqtt_server->register_notification(MQTT_OUT_TOPIC_CONTROL, _metadata->data.control_value);
}

void Application::_load() {
    bool active = config().power && !_night_mode_manager->active();
    if (!active) _pid->integral = 0;

    change_state(active ? AppState::ACTIVE : AppState::INACTIVE);

    auto &pid_cfg = config().regulator.pid;
    _pid->setpoint = pid_cfg.target;

    _pid->setKp(pid_cfg.p);
    _pid->setKi(pid_cfg.i);
    _pid->setKd(pid_cfg.d);
    _pid->setDt(pid_cfg.interval);

    if (pid_cfg.reverse) _pid->setMode(PID_REVERSE);
    else _pid->clearMode(PID_REVERSE);
}

void Application::_notify_periodic_status() {
    _bootstrap->mqtt_server()->send_notification(MQTT_OUT_TOPIC_SENSOR);
    _bootstrap->mqtt_server()->send_notification(MQTT_OUT_TOPIC_CONTROL);
}

void Application::_on_bootstrap_ready() {
    _load();

    _ntp_time->begin(config().sys_config.time_zone);

    _ntp_time->update();
    _night_mode_manager->update();

    _bootstrap->timer().add_interval([this](auto) {
        _bootstrap_service_loop();
    }, BOOTSTRAP_SERVICE_LOOP_INTERVAL);
}

void Application::event_loop() {
    _bootstrap->event_loop();
}

void Application::_handle_property_change(const AbstractParameter *parameter) {
    auto it = _parameter_to_packet.find(parameter);
    if (it == _parameter_to_packet.end()) return;

    _load();

    auto type = it->second;
    if (type >= PacketType::NIGHT_MODE_ENABLED && type <= PacketType::NIGHT_MODE_END) {
        _night_mode_manager->update();
    }

    update();
}


void Application::update() {
    _bootstrap->save_changes();
}

void Application::change_state(AppState s) {
    _state_change_time = millis();
    _state = s;
    D_PRINTF("Change app state: %s\r\n", __debug_enum_str(s));
}

void Application::_service_loop() {
    auto now = millis();
    if (now - _last_pid_compute < config().regulator.pid.interval) return;

    _last_pid_compute = now;
    if (!_sensor->has_value()) {
        D_PRINT("Sensor is not ready!");
        return;
    }

    auto value = _sensor->get_value();
    _runtime_info.sensor_value = value;

    float out = 0;
    if (_state == AppState::ACTIVE) {
        out = _pid->compute(value) / 255.f;
    }

    _control->set_value(out);
    _runtime_info.control_value = out;

    D_PRINTF("Sensor: %f; Control: %f\r\n", value, out);

    _runtime_info.history.entries[_runtime_info.history.index] = {
        .sensor = value,
        .control = out,
    };

    _runtime_info.history.index = (_runtime_info.history.index + 1) % HISTORY_COUNT;

    _bootstrap->ws_server()->send_notification(PacketType::SENSOR_VALUE);
    _bootstrap->ws_server()->send_notification(PacketType::CONTROL_VALUE);
    _bootstrap->ws_server()->send_notification(PacketType::HISTORY_DATA);
}

void Application::_bootstrap_service_loop() {
    if (_bootstrap->wifi_manager()->mode() == WifiMode::STA) {
        _ntp_time->update();
    }
}

void Application::_bootstrap_state_changed(void *sender, BootstrapState state, void *arg) {
    if (state == BootstrapState::INITIALIZING) {
        _ntp_time->begin(TIME_ZONE);

        change_state(AppState::INITIALIZATION);
    } else if (state == BootstrapState::READY && !_initialized) {
        _initialized = true;

        change_state(config().power ? AppState::ACTIVE : AppState::INACTIVE);
    }
}
