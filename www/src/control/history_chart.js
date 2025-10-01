import {BinaryParser} from "../lib/index.js";

import {Chart} from "./chart.js";

export class HistoryChart extends Chart {
    constructor(element) {
        const baseConfig = {
            margins: {left: 40, right: 40, top: 10, bottom: 10},
            axes: {
                sensorAxis: {side: "right", cssVar: "--chart-text", ticks: 5},
                controlAxis: {side: "left", cssVar: "--chart-text", ticks: 10, suggestedMin: 0, suggestedMax: 1},
            },
            series: [
                {field: "target", cssVar: "--chart-target", axis: "sensorAxis", style: "dash"},
                {field: "sensor", cssVar: "--chart-sensor", axis: "sensorAxis"},
                {field: "control", cssVar: "--chart-control", axis: "controlAxis"},
                {field: "integral", cssVar: "--chart-integral", axis: "controlAxis", style: "dash"},
            ],
            data: null
        };

        super(element, baseConfig);
    }

    setValue(value) {
        if (!value) return;

        const parser = new BinaryParser(value.buffer, value.byteOffset);
        const count = parser.readUint16();

        const sensorMin = parser.readFloat32();
        const sensorMax = parser.readFloat32();

        const index = parser.readUint16();
        const entries = new Array(count);
        for (let i = 0; i < count; i++) {
            entries[i] = {
                sensor: parser.readFloat32(),
                control: parser.readFloat32(),
                integral: parser.readFloat32()
            };
        }

        // reorder entries based on index
        const ordered = [];
        for (let i = 0; i < count; i++) {
            const pos = (index + i) % count;
            const entry = entries[pos];
            if (!Number.isNaN(entry.sensor) && !Number.isNaN(entry.control)) {
                ordered.push(entry);
            }
        }

        const target = window.__app.app.config.pid?.target ?? 0;

        const values = {
            target: new Array(ordered.length).fill(target),
            sensor: ordered.map(e => e.sensor),
            control: ordered.map(e => e.control),
            integral: ordered.map(e => e.integral)
        };

        this.config.axes.sensorAxis.suggestedMin = sensorMin;
        this.config.axes.sensorAxis.suggestedMax = sensorMax;

        this.setConfig({
            ...this.config,
            data: {count: ordered.length, values}
        });
    }
}