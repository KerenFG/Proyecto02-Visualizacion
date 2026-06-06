# CHART-SHAPES(3)

## NAME

chart-shapes - SVG shape generators (lines, areas, arcs, symbols)

## SYNOPSIS

inja

```
{{ shapeLine()|x("date")|y("value")|line(data) }}
{{ shapeArea()|x("x")|y0(0)|y1("y")|area(data) }}
{{ shapePie()|value("population")|pie(data) }}
```



## DESCRIPTION

The chart-shapes module provides D3-style shape generators for creating SVG paths from data. Shapes include line charts, area charts, pie/donut charts, symbols, and radial plots.

## SHAPE GENERATORS

### shapeLine()

Creates a line path generator.

**Configuration:**

- `x(field)` - X-coordinate accessor (field name or number)
- `y(field)` - Y-coordinate accessor (field name or number)
- `curve(type)` - Curve interpolation: `"linear"`, `"step"`, `"smooth"`, `"basis"`

**Example:**

inja

```
{% set lineGen = shapeLine()|x("date")|y("sales")|curve("smooth") %}
{{ lineGen|line(salesData) }}
```



### shapeArea()

Creates an area path generator (filled region between y0 and y1).

**Configuration:**

- `x(field)` - X-coordinate accessor
- `y0(field)` - Lower boundary accessor (default: 0)
- `y1(field)` - Upper boundary accessor
- `curve(type)` - Curve interpolation

**Example:**

inja

```
{% set areaGen = shapeArea()|x("year")|y0(0)|y1("value") %}
{{ areaGen|area(data) }}
```



### shapePie()

Creates a pie chart generator that computes angles from data.

**Configuration:**

- `value(field)` - Value accessor for slice sizes
- `startAngle(degrees)` - Starting angle in radians (default: 0)
- `endAngle(degrees)` - Ending angle in radians (default: 2π)
- `sortValues(boolean)` - Sort slices by value (default: true)

**Output:** Array of objects with `startAngle`, `endAngle`, `data`, `value`

**Example:**

inja

```
{% set pieGen = shapePie()|value("amount")|sortValues(false) %}
{% set slices = pieGen|pie(data) %}
```



### shapeArc()

Creates an arc path generator for pie/donut slices.

**Configuration:**

- `innerRadius(number)` - Inner radius (0 for pie, >0 for donut)
- `outerRadius(number)` - Outer radius

**Example:**

inja

```
{% set arcGen = shapeArc()|innerRadius(50)|outerRadius(100) %}
{% for slice in slices %}
  <path d="{{ arcGen|arc(slice) }}" fill="steelblue" />
{% endfor %}
```



### shapeSymbol()

Creates a symbol path generator (circles, squares, triangles, crosses).

**Configuration:**

- `size(number)` - Area of symbol (default: 64)
- `shape(string)` - Shape type: `"circle"`, `"square"`, `"triangle"`, `"cross"`

**Example:**

inja

```
{% set symbolGen = shapeSymbol()|size(80)|shape("triangle") %}
{{ symbolGen|symbol }}
```



### shapeRadialLine()

Creates a radial (polar) line generator.

**Configuration:**

- `angle(field)` - Angle accessor (in radians)
- `radius(field)` - Radius accessor

**Example:**

inja

```
{% set radialGen = shapeRadialLine()|angle("theta")|radius("r") %}
{{ radialGen|radialLine(radialData) }}
```



## CURVE TYPES

| Type       | Description                              |
| :--------- | :--------------------------------------- |
| `"linear"` | Straight line segments between points    |
| `"step"`   | Step function (horizontal then vertical) |
| `"smooth"` | Smooth Catmull-Rom spline                |
| `"basis"`  | B-spline interpolation                   |

## COMPLETE LINE CHART EXAMPLE

inja

```
{% set data = [
  {"date": "Jan", "value": 30},
  {"date": "Feb", "value": 45},
  {"date": "Mar", "value": 25}
] %}

{% set xScale = scaleBand()|domain(["Jan","Feb","Mar"])|range([0,400])|padding(0.5) %}
{% set yScale = scaleLinear()|domain([0,50])|range([300,0]) %}

{% set lineGen = shapeLine()|x(function(d) { return scale(xScale, d.date); })|y(function(d) { return scale(yScale, d.value); }) %}

<svg width="450" height="350">
  <path d="{{ lineGen|line(data) }}" fill="none" stroke="steelblue" stroke-width="2"/>
</svg>
```



## COMPLETE PIE CHART EXAMPLE

inja

```
{% set data = [
  {"category": "Apples", "count": 40},
  {"category": "Oranges", "count": 30},
  {"category": "Bananas", "count": 20}
] %}

{% set pieGen = shapePie()|value("count") %}
{% set arcGen = shapeArc()|innerRadius(0)|outerRadius(100) %}
{% set slices = pieGen|pie(data) %}
{% set colors = ["#ff9999","#99ff99","#9999ff"] %}

<svg width="250" height="250">
  <g transform="translate(125, 125)">
    {% for slice in slices %}
      <path d="{{ arcGen|arc(slice) }}" fill="{{ colors[loop.index0] }}" stroke="white"/>
    {% endfor %}
  </g>
</svg>
```



## SEE ALSO

chart-scale(3), chart-axis(3), chart-render(3)