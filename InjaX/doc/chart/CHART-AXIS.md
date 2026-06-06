# CHART-AXIS(3)

## NAME

chart-axis - Axis generators for charts (D3.js style)

## SYNOPSIS

inja

```
{{ axisBottom(scale)|tickCount(10)|tickFormat("%")|grid(true) }}
{{ axisLeft(scale)|tickValues([0,25,50,75,100])|tickSize(8) }}
```



## DESCRIPTION

The chart-axis module provides functions to generate SVG axis components. Axes include tick marks, labels, and optional grid lines. Axes are configured as JSON objects and can be rendered using the `call()` function from chart-render.

## AXIS ORIENTATIONS

### axisBottom(scale)

Creates a horizontal axis positioned at the bottom.

**Example:**

inja

```
{% set xAxis = axisBottom(xScale)|tickCount(10) %}
{{ xAxis|call }}
```



### axisTop(scale)

Creates a horizontal axis positioned at the top.

**Example:**

inja

```
{% set topAxis = axisTop(scale)|tickFormat("$") %}
```



### axisLeft(scale)

Creates a vertical axis positioned on the left.

**Example:**

inja

```
{% set yAxis = axisLeft(yScale)|tickSize(6)|tickPadding(4) %}
```



### axisRight(scale)

Creates a vertical axis positioned on the right.

**Example:**

inja

```
{% set rightAxis = axisRight(scale)|tickValues(["Jan","Feb","Mar"]) %}
```



## CONFIGURATION FUNCTIONS

### tickCount(axis, count)

Sets the approximate number of ticks to generate.

**Arguments:** `count` - Integer number of desired ticks

**Example:**

inja

```
{{ axisBottom(scale)|tickCount(5) }}
```



### tickValues(axis, array)

Specifies exact tick values to display.

**Arguments:** `array` - Array of tick values

**Example:**

inja

```
{{ axisBottom(scale)|tickValues([0, 25, 50, 75, 100]) }}
```



### tickFormat(axis, format)

Sets the format for tick labels.

**Formats:**

- `"default"` - Standard numeric formatting
- `"percent"` or `"%"` or `"pct"` - Percentage (e.g., "50%")
- `"currency"` or `"money"` or `"$"` - Currency (e.g., "$100")
- `"scientific"` or `"sci"` - Scientific notation
- `"date"` or `"time"` - Date formatting

**Example:**

inja

```
{{ axisBottom(scale)|tickFormat("percent") }}
{{ axisLeft(scale)|tickFormat("currency") }}
```



### tickSize(axis, size)

Sets the length of both inner and outer tick lines.

**Arguments:** `size` - Length in pixels (default: 6)

**Example:**

inja

```
{{ axisBottom(scale)|tickSize(10) }}
```



### tickSizeInner(axis, size)

Sets the length of inner tick lines (the ones pointing inward).

**Example:**

inja

```
{{ axisLeft(scale)|tickSizeInner(8) }}
```



### tickSizeOuter(axis, size)

Sets the length of outer tick lines (the ones at the ends).

**Example:**

inja

```
{{ axisBottom(scale)|tickSizeOuter(12) }}
```



### tickPadding(axis, padding)

Sets the distance between tick labels and tick lines.

**Arguments:** `padding` - Distance in pixels (default: 3)

**Example:**

inja

```
{{ axisBottom(scale)|tickPadding(5) }}
```



### tickAngle(axis, angle)

Rotates tick labels by the specified angle (in degrees).

**Arguments:** `angle` - Rotation angle in degrees (default: 0)

**Example:**

inja

```
{{ axisBottom(scale)|tickAngle(-45) }} {# Rotates labels -45 degrees #}
```



### grid(axis, enable)

Enables or disables grid lines extending from ticks.

**Arguments:** `enable` - Boolean (true/false)

**Example:**

inja

```
{{ axisLeft(scale)|grid(true) }}
```



## JSON REPRESENTATION

json

```
{
  "type": "axis",
  "orientation": "bottom",
  "scale": { ... },
  "ticks": { "count": 10 },
  "tickFormat": { "type": "enum", "value": "percent" },
  "tickValues": [],
  "tickSize": 6,
  "tickPadding": 3,
  "grid": false,
  "tickAngle": 0
}
```



## COMPLETE EXAMPLE

inja

```
{% set xScale = scaleLinear()|domain([0,100])|range([0,600]) %}
{% set yScale = scaleLinear()|domain([0,100])|range([400,0]) %}

{% set xAxis = axisBottom(xScale)|tickCount(10)|grid(true) %}
{% set yAxis = axisLeft(yScale)|tickFormat("$")|tickSize(8) %}

<svg width="650" height="450">
  <g transform="translate(50, 20)">
    {{ xAxis|call }}
    {{ yAxis|call }}
  </g>
</svg>
```



## SEE ALSO

chart-scale(3), chart-render(3)