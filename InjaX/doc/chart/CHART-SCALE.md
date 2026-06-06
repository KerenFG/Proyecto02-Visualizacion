# CHART-SCALE(3)

## NAME

chart-scale - D3-like scale functions for Inja templates

## SYNOPSIS

inja

```
{{ scaleLinear()|domain([0,100])|range([0,500]) }}
{{ scaleBand()|domain(["A","B","C"])|range([0,200])|padding(0.1) }}
{{ scale(scale_obj, value) }}
```



## DESCRIPTION

The chart-scale module provides D3.js style scaling functions for mapping input domain values to output range values. Scales are represented as JSON objects that can be manipulated via function chaining within Inja templates.

## SCALE TYPES

### scaleLinear()

Creates a linear scale mapping continuous numeric domain to continuous numeric range.

**Example:**

inja

```
{% set myScale = scaleLinear()|domain([0,100])|range([0,500])|clamp(true) %}
{{ myScale|scale(50) }} {# returns 250 #}
```



### scaleBand()

Creates a band scale for categorical data with discrete bands.

**Example:**

inja

```
{% set bandScale = scaleBand()|domain(["Jan","Feb","Mar"])|range([0,300])|padding(0.2) %}
{{ bandScale|bandwidth }} {# returns band width #}
```



### scaleOrdinal()

Creates an ordinal scale mapping discrete domain to discrete range.

**Example:**

inja

```
{% set colorScale = scaleOrdinal()|domain(["low","med","high"])|range(["red","yellow","green"]) %}
{{ colorScale|scale("med") }} {# returns "yellow" #}
```



### scaleLog()

Creates a logarithmic scale.

**Example:**

inja

```
{% set logScale = scaleLog()|domain([1,10000])|range([0,100]) %}
{{ logScale|scale(100) }} {# returns 50 #}
```



### scalePower()

Creates a power (exponential) scale.

**Example:**

inja

```
{% set powScale = scalePower()|domain([0,10])|range([0,100])|exponent(2) %}
```



### scaleTime()

Creates a time scale (uses same evaluation as linear but with time domain values).

**Example:**

inja

```
{% set timeScale = scaleTime()|domain(["2024-01-01","2024-12-31"])|range([0,500]) %}
```



## FUNCTIONS

### domain(scale, array)

Sets the input domain of the scale.

**Arguments:** `domain_array` - Array of two or more values

**Example:**

inja

```
{{ scaleLinear()|domain([0,100]) }}
{{ scaleLinear()|domain([0,50,100])|range(["red","yellow","green"]) }}
```



### range(scale, array)

Sets the output range of the scale.

**Arguments:** `range_array` - Array of two or more values

**Example:**

inja

```
{{ scaleLinear()|range([0,800]) }}
{{ scaleOrdinal()|range(["#ff0000","#00ff00","#0000ff"]) }}
```



### extent(data, [field])

Computes the min and max of a data array.

**Arguments:**

- `data` - Array of values or objects
- `field` - Optional field name if data contains objects

**Example:**

inja

```
{% set data = [5, 12, 3, 8, 25] %}
{% set domain = data|extent %} {# returns [3, 25] #}

{% set objects = [{"value":10}, {"value":20}] %}
{% set domain = objects|extent("value") %} {# returns [10, 20] #}
```



### padding(scale, value)

Sets the padding for band scales (total padding).

**Arguments:** `value` - Padding ratio (0 to 1)

**Example:**

inja

```
{{ scaleBand()|padding(0.2) }}
```



### paddingInner(scale, value)

Sets the inner padding between bands.

**Example:**

inja

```
{{ scaleBand()|paddingInner(0.1) }}
```



### paddingOuter(scale, value)

Sets the outer padding before the first and after the last band.

**Example:**

inja

```
{{ scaleBand()|paddingOuter(0.05) }}
```



### clamp(scale, boolean)

If true, clamps output values to the range boundaries.

**Example:**

inja

```
{{ scaleLinear()|domain([0,100])|range([0,500])|clamp(true) }}
```



### nice(scale, count)

Extends the domain to "nice" round numbers.

**Arguments:** `count` - Approximate number of ticks (default: 10)

**Example:**

inja

```
{{ scaleLinear()|domain([0.123, 0.887])|nice(5) }} {# domain becomes [0, 1] #}
```



### round(scale, boolean)

If true, rounds output values to integers.

**Example:**

inja

```
{{ scaleLinear()|round(true) }}
```



### exponent(scale, value)

Sets the exponent for power scales.

**Example:**

inja

```
{{ scalePower()|exponent(3) }}
```



### scale(scale, value)

Evaluates the scale, mapping input to output.

**Example:**

inja

```
{% set s = scaleLinear()|domain([0,100])|range([0,500]) %}
{{ s|scale(75) }} {# returns 375 #}
```



### bandwidth(scale)

Returns the computed width/height of each band in a band scale.

**Example:**

inja

```
{% set s = scaleBand()|domain(["A","B","C"])|range([0,300])|padding(0.1) %}
{{ s|bandwidth }} {# returns 90 #}
```



### ticks(scale, [count])

Returns an array of representative values from the scale's domain.

**Arguments:** `count` - Approximate number of ticks (default: 10)

**Example:**

inja

```
{% set s = scaleLinear()|domain([0,100]) %}
{% for tick in s|ticks(5) %}
  <option value="{{ tick }}">{{ tick }}</option>
{% endfor %}
```



## JSON OUTPUT EXAMPLE

json

```
{
  "xScale": {
    "type": "linear",
    "domain": [0, 100],
    "range": [0, 500],
    "clamped": false,
    "rounded": false,
    "niceCount": 0
  }
}
```



## SEE ALSO

chart-axis(3), chart-render(3), chart-time(3)