import {AppConfigBase} from "./lib/index.js";

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

        this.control = {
            type: parser.readUint8(),
            data: parser.readBinary(1024)
        };

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

    #parseState(parser) {
        return {
            sensor_value: parser.readFloat32(),
            control_value: parser.readFloat32(),
        }
    }
}