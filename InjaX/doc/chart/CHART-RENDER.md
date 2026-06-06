# CHART-RENDER(3)

## NAME

chart-render - SVG rendering engine for axes and scales

## SYNOPSIS

inja

```
{{ axis_object|call }}
```



## DESCRIPTION

The chart-render module provides the rendering engine that converts axis configuration objects into SVG markup. The primary function is `call()`, which evaluates an axis and returns SVG elements.

## FUNCTIONS

### call(axis)

Renders an axis configuration object into SVG elements.

**Arguments:** `axis` - Axis configuration object (created by axisBottom, axisLeft, etc.)

**Returns:** SVG string containing the rendered axis

**Example:**

inja

```
{% set myAxis = axisBottom(scale)|tickCount(8)|tickFormat("percent") %}
{{ myAxis|call }}
```



## RENDERED OUTPUT

The function generates SVG markup including:

- **Axis line** (`<line class="axis-line">`)
- **Tick groups** (`<g class="tick">`)
- **Tick lines** (`<line class="tick-line">`)
- **Tick labels** (`<text class="tick-label">`)
- **Grid lines** (`<line class="grid-line">` when enabled)

## CSS CLASSES

The rendered SVG uses these classes for styling:

| Class                     | Description                                          |
| :------------------------ | :--------------------------------------------------- |
| `axis-axis-[orientation]` | Container for entire axis (e.g., `axis-axis-bottom`) |
| `axis-line`               | The main axis line                                   |
| `tick`                    | Group containing a single tick                       |
| `tick-line`               | The tick mark line                                   |
| `tick-label`              | The tick text label                                  |
| `grid-line`               | Optional grid extension lines                        |

## EXAMPLE WITH STYLING

inja

```
<style>
  .axis-line { stroke: #333; stroke-width: 1.5; }
  .tick-line { stroke: #666; stroke-width: 1; }
  .tick-label { font-size: 11px; fill: #555; }
  .grid-line { stroke: #e0e0e0; stroke-width: 1; stroke-dasharray: 4,4; }
</style>

{% set xAxis = axisBottom(scale)|tickCount(10)|grid(true) %}
{{ xAxis|call }}
```



## INTERNAL WORKINGS

The `call()` function performs these steps:

1. Validates axis structure (scale must have domain and range)
2. Generates ticks using `ticks()` or `tickValues`
3. Formats tick labels according to `tickFormat`
4. Calculates label positions with respect to `tickAngle`
5. Produces SVG markup with proper text anchors

## SEE ALSO

chart-axis(3), chart-scale(3)