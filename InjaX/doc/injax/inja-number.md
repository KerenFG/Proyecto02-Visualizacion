# NUMBER-FILTERS(7)

## NAME

number-filters - InjaX numeric operations and formatting functions

## SYNOPSIS

Filter operations for numbers: absolute value, random selection, file size formatting, and text indentation.

## FILTERS

### `abs`

Returns absolute value of a number or each number in an array.

**Parameters:**

- `value`: Number, array of numbers, or convertible string

**Returns:** Number or array of numbers

**Special handling:** For integer `LLONG_MIN`, returns `LLONG_MAX`.

**Example with literal array:**

```
{{ -42 | abs }} → 42
{{ [-5, 3, -2, 0] | abs }} → [5, 3, 2, 0]
{{ "-123.45" | abs }} → 123.45
```

**Example with external JSON data:**

Given an external JSON file `data.json`:
```json
[-5, 3, -2, 0]
```

Loaded as variable `numbers`:
```
{{ numbers | abs }} → [5, 3, 2, 0]
```

### `filesizeformat`

Formats byte count as human-readable file size.

**Parameters:**

- `value`: Number of bytes (integer or numeric string)
- `binary`: Use binary units (KiB/MiB) if true, decimal (KB/MB) if false (default: false)

**Units:**

| Decimal | Binary |
| :------ | :----- |
| B       | B      |
| KB      | KiB    |
| MB      | MiB    |
| GB      | GiB    |
| TB      | TiB    |
| PB      | PiB    |

**Examples:**

```
{{ 0 | filesizeformat }} → "0 B"
{{ 1024 | filesizeformat }} → "1.0 KB"
{{ 1024 | filesizeformat(true) }} → "1.0 KiB"
{{ 1536000 | filesizeformat }} → "1.5 MB"
{{ 500 | filesizeformat }} → "500 B"
```

**Example with external JSON data:**

Given an external JSON file `fileinfo.json`:
```json
{
  "size_bytes": 1536000,
  "use_binary": false
}
```

Loaded as variable `file`:
```
{{ file.size_bytes | filesizeformat(file.use_binary) }} → "1.5 MB"
```

### `indent`

Indents each line of text with spaces.

**Parameters:**

- `value`: String to indent (non-strings are JSON-dumped)
- `width`: Number of spaces per indent (default: 4)
- `indent_first`: Whether to indent first line (default: false)
- `blank`: Whether to indent blank lines (default: false)

**Examples:**

```
{{ "line1\nline2\n\nline4" | indent(2, true) }}
→ "  line1\n  line2\n\n  line4"

{{ "first\nsecond" | indent(4, true, true) }}
→ "    first\n    second"
```

**Example with external JSON data:**

Given an external JSON file `textdata.json`:
```json
{
  "content": "line1\nline2\n\nline4",
  "spaces": 2,
  "indent_first_line": true
}
```

Loaded as variable `data`:
```
{{ data.content | indent(data.spaces, data.indent_first_line) }}
→ "  line1\n  line2\n\n  line4"
```

### `random`

Selects random element from array or random character from string.

**Parameters:**

- `source`: Array, string, or other value

**Returns:** Random element (if array/string) or original value (if not iterable)

**Note:** Uses thread-local Mersenne Twister with high-resolution clock seeding.

**Examples with literal values:**

```
{{ [1,2,3,4,5] | random }} → 3 (random)
{{ "abcdef" | random }} → "c" (random)
{{ 42 | random }} → 42
```

**Example with external JSON data:**

Given an external JSON file `options.json`:
```json
["red", "green", "blue", "yellow"]
```

Loaded as variable `colors`:
```
{{ colors | random }} → "green" (random)
```

**Example with array from external JSON:**

Given an external JSON file `dataset.json`:
```json
{
  "items": [10, 20, 30, 40, 50],
  "labels": ["A", "B", "C", "D", "E"]
}
```

Loaded as variable `data`:
```
{{ data.items | random }} → 30 (random)
{{ data.labels | random }} → "C" (random)
```

## SEE ALSO

array-filters(7), string-filters(7), html-filters(7), tests(7)