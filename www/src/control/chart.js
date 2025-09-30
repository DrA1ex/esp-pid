import {BinaryParser, Control} from "../lib/index.js";

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

        window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', event => {
            this.render();
        });

        window.addEventListener("resize", this.#resize.bind(this));
        setTimeout(() => this.#resize(), 0);
    }

    #resize() {
        const {width, height} = this.#canvas.getBoundingClientRect();
        this.#canvas.width = width * this.#ratio;
        this.#canvas.height = height * this.#ratio;

        this.#ctx.setTransform(1, 0, 0, 1, 0, 0);
        this.#ctx.scale(this.#ratio, this.#ratio);

        this.render();
    }

    setValue(value) {
        if (!value) return;
        
        const parser = new BinaryParser(value.buffer, value.byteOffset);
        const result = {
            count: parser.readUint16(),
            index: parser.readUint16(),
        }

        result.entries = new Array(result.count)
        for (let i = 0; i < result.count; i++) {
            result.entries[i] = {
                sensor: parser.readFloat32(),
                control: parser.readFloat32(),
                integral: parser.readFloat32(),
            }
        }

        this.#data = result;
        this.render();
    }

    render() {
        if (!this.#data) return;
        const ctx = this.#ctx;
        const {count, index, entries} = this.#data;

        const {width: w, height: h} = this.#canvas.getBoundingClientRect();
        ctx.clearRect(0, 0, w, h);
        ctx.lineJoin = 'round';

        const margins = {left: 40, right: 40, top: 10, bottom: 10};
        const plotW = w - margins.left - margins.right;
        const plotH = h - margins.top - margins.bottom;

        const ordered = this.#getOrderedEntries(entries, count, index);
        const toX = i => margins.left + i * (plotW / (count - 1));

        // Draw sensor and control lines
        this.#drawSeries(ordered.map(e => e.sensor), "--chart-sensor", toX, margins, plotH, ctx);
        this.#drawSeries(ordered.map(e => e.control), "--chart-control", toX, margins, plotH, ctx);
        this.#drawSeries(ordered.map(e => e.integral), "--chart-integral", toX, margins, plotH, ctx);

        // Draw axes
        this.#drawAxis(ctx, ordered.map(e => e.sensor), margins, plotH, "--chart-text", "left", w);
        this.#drawAxis(ctx, ordered.map(e => e.control), margins, plotH, "--chart-text", "right", w);
    }

    #getOrderedEntries(entries, count, index) {
        const ordered = [];
        for (let i = 0; i < count; i++) {
            const pos = (index + i) % count;
            const entry = entries[pos];
            if (!Number.isNaN(entry.sensor) && !Number.isNaN(entry.control)) {
                ordered.push(entry);
            }
        }
        return ordered;
    }

    #computeScale(values, plotH) {
        const min = Math.min(...values);
        const max = Math.max(...values);
        const pad = (max - min) * 0.1 || 1;
        const yMin = min - pad;
        const yMax = max + pad;
        const scaleY = plotH / (yMax - yMin);
        return {yMin, yMax, scaleY};
    }

    #drawSeries(values, cssVar, toX, margins, plotH, ctx) {
        const {yMin, scaleY} = this.#computeScale(values, plotH);
        const toY = v => margins.top + plotH - (v - yMin) * scaleY;

        ctx.strokeStyle = getComputedStyle(this.#canvas).getPropertyValue(cssVar).trim();
        ctx.lineWidth = 2;
        ctx.beginPath();
        values.forEach((val, i) => {
            const x = toX(i);
            const y = toY(val);
            i === 0 ? ctx.moveTo(x, y) : ctx.lineTo(x, y);
        });
        ctx.stroke();
    }

    #drawAxis(ctx, values, margins, plotH, cssVar, align, w) {
        const {yMin, yMax} = this.#computeScale(values, plotH);
        ctx.fillStyle = getComputedStyle(this.#canvas).getPropertyValue(cssVar).trim();
        ctx.font = "0.65rem sans-serif";
        ctx.textAlign = align;
        ctx.textBaseline = "middle";

        for (let i = 0; i <= 5; i++) {
            const val = yMax - (i / 5) * (yMax - yMin);
            const y = margins.top + (plotH / 5) * i;
            const x = align === "left" ? 2 : w - 2;
            ctx.fillText(val.toFixed(2), x, y);
        }
    }
}