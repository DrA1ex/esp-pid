#pragma once

#include "sys_constants.h"

#include <uPID.h>

#include <lib/bootstrap.h>
#include <lib/misc/button.h>
#include <lib/misc/ntp_time.h>
#include <lib/async/promise.h>

#include "config.h"
#include "metadata.h"
#include "cmd.h"
#include "controls/pwm_control.h"
#include "misc/night_mode.h"
#include "sensors/dsx18_sensor.h"

class Application {
    std::unique_ptr<Bootstrap<Config, PacketType>> _bootstrap = nullptr;
    std::unique_ptr<ConfigMetadata> _metadata = nullptr;
    std::unique_ptr<NightModeManager> _night_mode_manager = nullptr;
    std::unique_ptr<NtpTime> _ntp_time = nullptr;

    std::unique_ptr<DSx18Sensor> _sensor = nullptr;
    std::unique_ptr<PwmControl> _control = nullptr;
    std::unique_ptr<uPID> _pid = nullptr;

    RuntimeInfo _runtime_info{};

    bool _initialized = false;
    uint32_t _last_pid_compute = 0;

    unsigned long _state_change_time = 0;
    AppState _state = AppState::UNINITIALIZED;

    std::map<const AbstractParameter *, PacketType> _parameter_to_packet{};

public:
    [[nodiscard]] Config &config() const { return _bootstrap->config(); }
    [[nodiscard]] SysConfig &sys_config() const { return config().sys_config; }

    void begin();
    void event_loop();

    void update();

    void restart() { _bootstrap->restart(); }

protected:
    void change_state(AppState s);

private:
    void _setup();
    void _load();

    void _notify_periodic_status();

    void _on_bootstrap_ready();
    void _bootstrap_state_changed(void *sender, BootstrapState state, void *arg);
    void _night_mode_state_changed(void *sender, NightModeState state, void *arg);

    void _service_loop();
    void _bootstrap_service_loop();

    void _handle_property_change(const AbstractParameter *param);
};
