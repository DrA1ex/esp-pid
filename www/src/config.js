import {AppConfigBase, BinaryParser} from "./lib/index.js";

import {PropertyConfig} from "./props.js";
import {PacketType} from "./cmd.js";


export class Config extends AppConfigBase {
    power;

    sensor;
    control;
    pid;

    sysConfig;

    status;

    constructor() {
        super(PropertyConfig);

        this.lists["wifiMode"] = [
            {code: 0, name: "AP"},
            {code: 1, name: "STA"},
        ];

        this.lists["sensorType"] = [
            {code: 0, name: "Analog"},
            {code: 1, name: "DSX18X"},
        ]

        this.lists["controlType"] = [
            {code: 0, name: "PWM"},
        ]
    }

    get cmd() {return PacketType.GET_CONFIG;}

    async load(ws) {
        const statePacket = await ws.request(PacketType.GET_STATE)
        this.status = this.#parseState(statePacket.parser());

        await super.load(ws);
    }

    parse(parser) {
        this.power = parser.readBoolean();

        this.sensor = {
            type: parser.readUint8(),
            data: parser.readBinary(1024)
        };

        this.#parseSensor();

        this.control = {
            type: parser.readUint8(),
            data: parser.readBinary(1024)
        };

        this.#parseControl();

        this.pid = {
            target: parser.readFloat32(),

            p: parser.readFloat32(),
            i: parser.readFloat32(),
            d: parser.readFloat32(),

            interval: parser.readUint16(),
            reverse: parser.readBoolean()
        };

        this.nightMode = {
            enabled: parser.readBoolean(),
            startTime: parser.readUint32(),
            endTime: parser.readUint32()
        };


        this.sysConfig = {
            mdnsName: parser.readFixedString(32),

            wifiMode: parser.readUint8(),
            wifiSsid: parser.readFixedString(32),
            wifiPassword: parser.readFixedString(32),

            wifiConnectionCheckInterval: parser.readUint32(),
            wifiMaxConnectionAttemptInterval: parser.readUint32(),

            timeZone: parser.readFloat32(),

            mqtt: parser.readBoolean(),
            mqttHost: parser.readFixedString(32),
            mqttPort: parser.readUint16(),
            mqttUser: parser.readFixedString(32),
            mqttPassword: parser.readFixedString(32)
        };
    }

    parseHistory(parser) {
        const result = {
            count: parser.readUint16(),
            index: parser.readUint16(),
        }

        result.entries = new Array(result.count)
        for (let i = 0; i < result.count; i++) {
            result.entries[i] = {
                sensor: parser.readFloat32(),
                control: parser.readFloat32(),
            }
        }

        return result;
    }

    #parseSensor() {
        const parser = new BinaryParser(this.sensor.data.buffer, this.sensor.data.byteOffset);

        this.sensor.parsed = {};
        if (this.sensor.type === 0) { // ANALOG
            this.sensor.parsed["analog"] = {
                pin: parser.readUint8(),
                resolution: parser.readUint8(),
            }
        } else if (this.sensor.type === 1) { // DSX18X
            this.sensor.parsed["dsx18x"] = {
                pin: parser.readUint8(),
                resolution: parser.readUint8(),
                parasite: parser.readBoolean(),
            }
        }
    }

    #parseControl() {
        const parser = new BinaryParser(this.control.data.buffer, this.control.data.byteOffset);

        this.control.parsed = {};
        if (this.control.type === 0) { // PWM
            this.control.parsed["pwm"] = {
                pin: parser.readUint8(),
                period: parser.readUint16(),
            }
        }
    }

    #parseState(parser) {
        return {
            sensor_value: parser.readFloat32(),
            control_value: parser.readFloat32(),

            history: this.parseHistory(parser)
        };
    }
}