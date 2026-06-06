# CHART-TIME(3)

## NAME

chart-time - Date and time parsing/formatting utilities

## SYNOPSIS

inja

```
{{ timeParse("%Y-%m-%d") }}
{{ timeFormat("%B %d, %Y") }}
{{ now() }}
```



## DESCRIPTION

The chart-time module provides D3-style date parsing and formatting functions. It supports standard D3 specifiers and handles both local time and UTC timezones.

## PARSING FUNCTIONS

### timeParse(specifier)

Creates a parser function for converting strings to timestamps (local time).

**Example:**

inja

```
{% set parseDate = timeParse("%Y-%m-%d") %}
{# Usage: parseDate("2024-12-25") returns timestamp #}
```



### utcParse(specifier)

Creates a parser function for UTC time.

**Example:**

inja

```
{% set parseUTC = utcParse("%Y-%m-%dT%H:%M:%SZ") %}
```



### isoParse()

Parses ISO 8601 format strings.

**Format:** `YYYY-MM-DDThh:mm:ss.sssZ`

**Example:**

inja

```
{{ isoParse() }} {# Returns parser for ISO dates #}
```



## FORMATTING FUNCTIONS

### timeFormat(specifier)

Creates a formatter function for timestamps to strings (local time).

**Example:**

inja

```
{% set formatDate = timeFormat("%B %d, %Y") %}
{# formatDate(timestamp) returns "December 25, 2024" #}
```



### utcFormat(specifier)

Creates a formatter function for UTC time.

**Example:**

inja

```
{% set formatUTC = utcFormat("%Y-%m-%d %H:%M:%S UTC") %}
```



### isoFormat()

Formats a timestamp as ISO 8601 string.

**Example:**

inja

```
{{ isoFormat() }} {# Returns ISO formatter #}
```



### now()

Returns the current timestamp.

**Example:**

inja

```
{{ now() }}
```



## D3 TIME SPECIFIERS

| Specifier | Description            | Example   |
| :-------- | :--------------------- | :-------- |
| `%Y`      | 4-digit year           | 2024      |
| `%y`      | 2-digit year           | 24        |
| `%m`      | Month (01-12)          | 12        |
| `%d`      | Day (01-31)            | 25        |
| `%H`      | Hour (00-23)           | 14        |
| `%I`      | Hour (01-12)           | 02        |
| `%M`      | Minute (00-59)         | 30        |
| `%S`      | Second (00-59)         | 45        |
| `%L`      | Milliseconds (000-999) | 123       |
| `%B`      | Full month name        | December  |
| `%b`      | Abbreviated month      | Dec       |
| `%A`      | Full weekday           | Wednesday |
| `%a`      | Abbreviated weekday    | Wed       |

## COMPLETE EXAMPLE WITH SCALES

inja

```
{% set data = [
  {"date": "2024-01-15", "value": 100},
  {"date": "2024-02-15", "value": 150},
  {"date": "2024-03-15", "value": 200}
] %}

{% set parseDate = timeParse("%Y-%m-%d") %}
{% set formatDate = timeFormat("%b %d") %}

{# Convert string dates to timestamps #}
{% for row in data %}
  {% set row.timestamp = parseDate(row.date) %}
{% endfor %}

{# Create time scale #}
{% set timeScale = scaleTime()|domain(data|extent("timestamp"))|range([0, 600]) %}

<svg width="650" height="100">
  <g transform="translate(50, 20)">
    {% set xAxis = axisBottom(timeScale)|tickFormat("date")|tickCount(5) %}
    {{ xAxis|call }}
  </g>
</svg>
```



## DATE FORMATTING EXAMPLES

inja

```
{% set ts = now() %}

{# Different formats #}
{{ timeFormat("%Y-%m-%d")(ts) }}     {# 2024-12-25 #}
{{ timeFormat("%B %d, %Y")(ts) }}    {# December 25, 2024 #}
{{ timeFormat("%a %b %e")(ts) }}     {# Wed Dec 25 #}
{{ timeFormat("%I:%M %p")(ts) }}     {# 02:30 PM #}
{{ timeFormat("%Y-%m-%d %H:%M:%S")(ts) }} {# 2024-12-25 14:30:45 #}
```



## SEE ALSO

chart-scale(3), chart-axis(3)