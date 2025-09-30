import {Control} from "../lib/index.js";

export class Chart extends Control {
    #ctx;
    #canvas;
    #ratio = window.devicePixelRatio || 1;
    #data = null;

    constructor(element) {
        super(element);

        this.#canvas = element;
        this.#ctx = element.getContext("2d");
        this.addClass("chart");

        window.addEventListener("resize", this.#resize.bind(this));
        setTimeout(() => this.#resize(), 0);
    }

    #resize() {
        const rect = this.#canvas.getBoundingClientRect();
        const w = rect.width;
        const h = rect.height;

        this.#canvas.width = w * this.#ratio;
        this.#canvas.height = h * this.#ratio;

        this.#ctx.setTransform(1, 0, 0, 1, 0, 0);
        this.#ctx.scale(this.#ratio, this.#ratio);

        this.render();
    }

    setValue(value) {
        if (!value || !value.count || !value.entries) return;

        this.#data = value;
        this.render();
    }

    render() {
        if (!this.#data) return;
        const ctx = this.#ctx;
        const {count, index, entries} = this.#data;

        const rect = this.#canvas.getBoundingClientRect();
        const w = rect.width;
        const h = rect.height;

        ctx.clearRect(0, 0, w, h);

        const marginLeft = 40;
        const marginRight = 40;
        const marginTop = 10;
        const marginBottom = 10;

        const plotW = w - marginLeft - marginRight;
        const plotH = h - marginTop - marginBottom;

        const ordered = [];
        for (let i = 0; i < count; i++) {
            const pos = (index + i) % count;
            const entry = entries[pos];
            if (Number.isNaN(entry.sensor) || Number.isNaN(entry.control)) continue;

            ordered.push(entry);
        }

        const scaleX = plotW / (count - 1);
        const toX = i => marginLeft + i * scaleX;

        // === sensor ===
        const sensorValues = ordered.map(e => e.sensor);
        const sMin = Math.min(...sensorValues);
        const sMax = Math.max(...sensorValues);
        const sPad = (sMax - sMin) * 0.1 || 1;
        const sYmin = sMin - sPad;
        const sYmax = sMax + sPad;
        const sScaleY = plotH / (sYmax - sYmin);
        const sToY = v => marginTop + plotH - (v - sYmin) * sScaleY;

        // === control ===
        const controlValues = ordered.map(e => e.control);
        const cMin = Math.min(...controlValues);
        const cMax = Math.max(...controlValues);
        const cPad = (cMax - cMin) * 0.1 || 1;
        const cYmin = cMin - cPad;
        const cYmax = cMax + cPad;
        const cScaleY = plotH / (cYmax - cYmin);
        const cToY = v => marginTop + plotH - (v - cYmin) * cScaleY;

        // === sensor line ===
        ctx.strokeStyle = getComputedStyle(this.#canvas).getPropertyValue("--chart-sensor").trim();
        ctx.lineWidth = 2;
        ctx.beginPath();
        ordered.forEach((e, i) => {
            const x = toX(i);
            const y = sToY(e.sensor);
            i === 0 ? ctx.moveTo(x, y) : ctx.lineTo(x, y);
        });
        ctx.stroke();

        // === control line ===
        ctx.strokeStyle = getComputedStyle(this.#canvas).getPropertyValue("--chart-control").trim();
        ctx.lineWidth = 2;
        ctx.beginPath();
        ordered.forEach((e, i) => {
            const x = toX(i);
            const y = cToY(e.control);
            i === 0 ? ctx.moveTo(x, y) : ctx.lineTo(x, y);
        });
        ctx.stroke();

        ctx.fillStyle = getComputedStyle(this.#canvas).getPropertyValue("--chart-text").trim();
        ctx.font = "0.65rem sans-serif";
        ctx.textAlign = "left";
        ctx.textBaseline = "middle";

        // === left axis (sensor) ===
        for (let i = 0; i <= 5; i++) {
            const val = sYmax - (i / 5) * (sYmax - sYmin);
            const y = marginTop + (plotH / 5) * i;
            ctx.fillText(val.toFixed(2), 2, y);
        }

        // === right axis (control) ===
        ctx.textAlign = "right";
        for (let i = 0; i <= 5; i++) {
            const val = cYmax - (i / 5) * (cYmax - cYmin);
            const y = marginTop + (plotH / 5) * i;
            ctx.fillText(val.toFixed(2), w - 2, y);
        }
    }
}
