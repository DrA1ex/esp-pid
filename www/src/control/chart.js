import {Control} from "../lib/index.js";

export class Chart extends Control {
    #ctx;
    #canvas;
    #ratio = window.devicePixelRatio || 1;
    #config = null;

    constructor(element, config) {
        super(element);

        this.#canvas = element;
        this.#ctx = element.getContext("2d");
        this.addClass("chart");

        this.#config = config;

        // re-render on color scheme change
        window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', () => {
            this.render();
        });

        window.addEventListener("resize", this.#resize.bind(this));
        setTimeout(() => this.#resize(), 0);
    }

    setConfig(config) {
        this.#config = config;
        this.render();
    }

    get config() {
        return this.#config;
    }

    #resize() {
        const {width, height} = this.#canvas.getBoundingClientRect();
        this.#canvas.width = width * this.#ratio;
        this.#canvas.height = height * this.#ratio;

        this.#ctx.setTransform(1, 0, 0, 1, 0, 0);
        this.#ctx.scale(this.#ratio, this.#ratio);

        this.render();
    }

    render() {
        if (!this.#config || !this.#config.data) return;

        const ctx = this.#ctx;
        const {margins, axes, series, data} = Object.assign({
            margins: {left: 40, right: 40, top: 10, bottom: 10},
            series: [],
            axes: {
                "default": {side: "right", ticks: 5}
            },
            data: null
        }, this.#config);

        const width = this.#canvas.width / this.#ratio;
        const height = this.#canvas.height / this.#ratio;

        ctx.clearRect(0, 0, width, height);
        ctx.lineJoin = "round";

        const plotWidth = width - margins.left - margins.right;
        const plotHeight = height - margins.top - margins.bottom;

        const toX = i => margins.left + i * (plotWidth / (data.count - 1));

        // Compute scales for each axis
        const axisScales = {};
        for (const axisName in axes) {
            const values = series
                .filter(s => s.axis === axisName)
                .flatMap(s => data.values[s.field]);

            axisScales[axisName] = this.#computeScale(values, plotHeight, axes[axisName]);
        }

        // Draw all series
        for (const s of series) {
            const scale = axisScales[s.axis || "default"];
            const color = getComputedStyle(this.#canvas).getPropertyValue(s.cssVar).trim() || "black";
            this.#drawSeries(
                data.values[s.field],
                color,
                toX,
                margins,
                plotHeight,
                ctx,
                scale
            );
        }

        // Draw all axes
        for (const axisName in axes) {
            this.#drawAxis(
                ctx,
                axisScales[axisName],
                axes[axisName],
                margins,
                plotHeight,
                width
            );
        }
    }

    #computeScale(values, plotHeight, axisCfg) {
        let min = Math.min(...values);
        let max = Math.max(...values);

        if (axisCfg.suggestedMin !== undefined) min = Math.min(axisCfg.suggestedMin, min);
        if (axisCfg.suggestedMax !== undefined) max = Math.max(axisCfg.suggestedMax, max);

        if (min === max) {
            min *= 0.95;
            max *= 1.05;
        }

        const yMin = min;
        const yMax = max;
        const scaleY = plotHeight / (yMax - yMin);

        return {yMin, yMax, scaleY};
    }

    #drawSeries(values, strokeStyle, toX, margins, plotHeight, ctx, scale) {
        const {yMin, scaleY} = scale;
        const toY = v => margins.top + plotHeight - (v - yMin) * scaleY;

        ctx.strokeStyle = strokeStyle;
        ctx.lineWidth = 2;
        ctx.beginPath();

        values.forEach((val, i) => {
            const x = toX(i);
            const y = toY(val);

            if (i === 0) ctx.moveTo(x, y);
            else ctx.lineTo(x, y);
        });

        ctx.stroke();
    }

    #drawAxis(ctx, scale, axisCfg, margins, plotHeight, width) {
        const {yMin, yMax} = scale;

        ctx.fillStyle = getComputedStyle(this.#canvas).getPropertyValue(axisCfg.cssVar).trim() || "black";
        ctx.font = "0.65rem sans-serif";
        ctx.textAlign = axisCfg.side ?? "left";
        ctx.textBaseline = "middle";

        const ticks = axisCfg.ticks ?? 5;
        for (let i = 0; i <= ticks; i++) {
            const val = yMax - (i / ticks) * (yMax - yMin);

            const y = margins.top + (plotHeight / ticks) * i;
            const x = axisCfg.side === "left" ? 2 : width - 2;

            ctx.fillText(val.toFixed(2), x, y);
        }
    }
}