# CHART-TRANSFORM(3)

## NAME

chart-transform - Data transformation utilities (stacking)

## SYNOPSIS

inja

```
{{ stack(data, keys) }}
{{ stackOrder(data, keys, "ascending") }}
{{ stackOffset(data, keys, "expand") }}
```



## DESCRIPTION

The chart-transform module provides data transformation functions, primarily for creating stacked layouts. Stacking transforms a table of values into a series of layers suitable for stacked bar charts, streamgraphs, and area charts.

## STACK FUNCTIONS

### stack(data, keys)

Creates a basic stacked series from the data.

**Arguments:**

- `data` - Array of objects containing numeric values
- `keys` - Array of field names to stack, or a single field name to auto-detect

**Output:** Array of series, each with:

- `key` - The field name
- `index` - Series index
- `data` - Array of `[y0, y1]` tuples per data point

**Example:**

inja

```
{% set data = [
  {"month":"Jan","apples":10,"oranges":5,"bananas":8},
  {"month":"Feb","apples":12,"oranges":7,"bananas":6}
] %}
{% set stacked = data|stack(["apples","oranges","bananas"]) %}
```



### stackKeys(data, [exclude])

Extracts numeric field names from data for stacking.

**Arguments:**

- `data` - Input data array
- `exclude` - Optional field name to exclude

**Example:**

inja

```
{% set keys = data|stackKeysExclude("month") %}
{# Returns ["apples","oranges","bananas"] #}
```



## STACK ORDERING

### stackOrder(data, keys, order)

Creates stacked series with specified order.

**Order values:**

- `"none"` - Preserves input order (default)
- `"reverse"` - Reverses input order
- `"ascending"` - Orders by total value (smallest first)
- `"descending"` - Orders by total value (largest first)

**Example:**

inja

```
{{ data|stackOrder(["apples","oranges","bananas"], "descending") }}
```



## STACK OFFSETS

### stackOffset(data, keys, offset)

Creates stacked series with baseline offset.

**Offset values:**

- `"none"` - Zero baseline (default)
- `"expand"` - Normalizes to 100% (relative frequency)
- `"diverging"` - Centers positive and negative values
- `"wiggle"` - Streamgraph with minimal wobble
- `"silhouette"` - Centers the stream

**Example:**

inja

```
{{ data|stackOffset(["apples","oranges"], "expand") }}
```



## OUTPUT DATA STRUCTURE

json

```
[
  {
    "key": "apples",
    "index": 0,
    "data": [
      [0, 10],   // y0, y1 for Jan
      [0, 12]    // y0, y1 for Feb
    ]
  },
  {
    "key": "oranges",
    "index": 1,
    "data": [
      [10, 15],  // stacked on apples
      [12, 19]
    ]
  }
]
```



## STACKED BAR CHART EXAMPLE

inja

```
{% set data = [
  {"Q1": 30, "Q2": 45, "Q3": 25},
  {"Q1": 20, "Q2": 35, "Q3": 40}
] %}

{% set stacked = data|stack(["Q1","Q2","Q3"]) %}
{% set xScale = scaleBand()|domain([0,1])|range([0,400])|padding(0.2) %}
{% set yScale = scaleLinear()|domain([0,100])|range([300,0]) %}

{% for series in stacked %}
  {% for point in series.data %}
    {% set y0 = yScale|scale(point[0]) %}
    {% set y1 = yScale|scale(point[1]) %}
    <rect x="{{ xScale|scale(point.index) }}" 
          y="{{ y1 }}" 
          width="{{ xScale|bandwidth }}" 
          height="{{ y0 - y1 }}" />
  {% endfor %}
{% endfor %}
```



## STREAMGRAPH (WIGGLE) EXAMPLE

inja

```
{% set data = [
  {"month":"Jan","cats":10,"dogs":15,"birds":5},
  {"month":"Feb","cats":12,"dogs":18,"birds":7},
  {"month":"Mar","cats":8,"dogs":20,"birds":10}
] %}

{% set stacked = data|stackOffset(["cats","dogs","birds"], "wiggle") %}
{% set yScale = scaleLinear()|domain([-30,50])|range([300,0]) %}

{% set areaGen = shapeArea()|x(function(d) { return xScale(d.data.month); })|y0(function(d) { return yScale|scale(d[0]); })|y1(function(d) { return yScale|scale(d[1]); }) %}

{% for series in stacked %}
  <path d="{{ series.data|area }}" fill="steelblue" opacity="0.7"/>
{% endfor %}
```



## SEE ALSO

chart-scale(3), chart-shapes(3)