# ARRAY-FILTERS(7)

## NAME

array-filters - InjaX array manipulation functions

## SYNOPSIS

Filter operations on arrays: mapping, selecting, rejecting, grouping, sorting, batching, zipping, and numeric aggregation.

**IMPORTANT:** JSON data must reside in an external file and be loaded through the Inja context. The examples assume context variables containing arrays and JSON objects.

## FILTERS

### `array`

Creates an array from the provided arguments.

**Parameters:** Any number of values, each becomes an element.

**Example:**

```
{{ [1, 2, 3] | array }} → [1,2,3]
{{ "a", "b", "c" | array }} → ["a","b","c"]
```

### `append`

Appends an item to an array.

**Parameters:**

- `arr`: Array to modify (if not an array, new array is created)
- `item`: Value to append

**Example:**

```
{{ my_array | append(4) }}
```

*Note: `my_array` is a context variable containing `[1,2,3]`*

### `batch`

Splits array into batches of specified size.

**Parameters:**

- `seq`: Input array
- `size`: Batch size (positive integer)
- `fill_with`: Optional filler for incomplete last batch (default: null, no filling)

**Example:**

```
{{ [1,2,3,4,5] | batch(2) }} → [[1,2],[3,4],[5]]
{{ [1,2,3] | batch(4, "x") }} → [[1,2,3,"x"]]
```

### `dictsort`

Sorts dictionary key-value pairs.

**Parameters:**

- `obj`: Dictionary/object (loaded from external JSON file)
- `by`: Sort criteria - "key" (default) or "value"

**Returns:** Array of objects with `key` and `value` properties.

**Example:**

Given an external JSON file:
```json
{"c": 3, "a": 1, "b": 2}
```

Loaded as variable `data` in the Inja context:
```
{{ data | dictsort("key") }}
→ [{"key":"a","value":1},{"key":"b","value":2},{"key":"c","value":3}]
```

### `items`

Converts dictionary to array of key-value pairs.

**Parameters:**

- `obj`: Dictionary/object (loaded from external JSON file)

**Returns:** Array of objects with `key` and `value` properties.

**Example:**

Given an external JSON file:
```json
{"name": "John", "age": 30}
```

Loaded as variable `person` in the context:
```
{{ person | items }}
→ [{"key":"name","value":"John"},{"key":"age","value":30}]
```

### `make_list`

Converts any value to an array.

**Parameters:**

- `value`: Value to convert

**Returns:** Original array if input is array; array of object values if input is object; single-element array otherwise.

**Example:**

```
{{ 42 | make_list }} → [42]
```

Given an external JSON object `{"a":1,"b":2}` loaded as `data`:
```
{{ data | make_list }} → [1,2]
```

### `map`

Extracts attribute or index from each array element.

**Parameters:**

- `seq`: Input array
- `key_or_path`: String path (dot notation) or integer index

**Example:**

Given an external JSON file:
```json
[{"name":"Alice"},{"name":"Bob"}]
```

Loaded as variable `users`:
```
{{ users | map("name") }} → ["Alice","Bob"]
```

### `obj`

Creates a dictionary from alternating key-value arguments.

**Parameters:** Even number of arguments: key1, value1, key2, value2, ...

**Example:**

```
{{ "name","John","age",30 | obj }} → {"name":"John","age":30}
```

### `regroup`

Groups array elements by attribute value.

**Parameters:**

- `seq`: Input array
- `attr`: Attribute path (dot notation)

**Returns:** Array of objects with `grouper` (the grouping value) and `list` (array of matching items).

**Example:**

Given an external JSON file:
```json
[
  {"type":"A","val":1},
  {"type":"B","val":2},
  {"type":"A","val":3}
]
```

Loaded as variable `items`:
```
{{ items | regroup("type") }}
→ [{"grouper":"A","list":[{"type":"A","val":1},{"type":"A","val":3}]},
   {"grouper":"B","list":[{"type":"B","val":2}]}]
```

### `reject`

Filters out array elements equal to a value.

**Parameters:**

- `seq`: Input array
- `value`: Value to reject

**Example:**

```
{{ [1,2,3,2,1] | reject(2) }} → [1,3,1]
```

### `rejectattr`

Filters out elements whose attribute passes a test.

**Parameters:**

- `seq`: Input array
- `attr`: Attribute path (dot notation)
- `test`: Test name (optional: "", "equalto", "defined", "none", "in", "match")
- `test_arg`: Test argument (optional)

**Example:**

Given an external JSON file:
```json
[
  {"name":"Alice"},
  {"name":""},
  {"name":"Bob"}
]
```

Loaded as variable `data`:
```
{{ data | rejectattr("name", "none") }}
→ [{"name":"Alice"},{"name":"Bob"}]
```

### `select`

Selects array elements equal to a value.

**Parameters:**

- `seq`: Input array
- `value`: Value to select

**Example:**

```
{{ [1,2,3,2,1] | select(2) }} → [2,2]
```

### `selectattr`

Selects elements whose attribute passes a test.

**Parameters:**

- `seq`: Input array
- `attr`: Attribute path (dot notation)
- `test`: Test name (optional: "", "equalto", "defined", "none", "in", "match")
- `test_arg`: Test argument (optional)

**Example:**

Given an external JSON file:
```json
[
  {"name":"Alice","age":25},
  {"name":"Bob","age":30}
]
```

Loaded as variable `people`:
```
{{ people | selectattr("age", "equalto", 30) }}
→ [{"name":"Bob","age":30}]
```

### `slice`

Extracts a slice from an array.

**Parameters:**

- `seq`: Input array
- `start`: Starting index (negative counts from end)
- `stop`: Stopping index (exclusive, negative counts from end)
- `step`: Step size (default: 1, can be negative)

**Example:**

```
{{ [0,1,2,3,4,5] | slice(1,4) }} → [1,2,3]
{{ [0,1,2,3,4,5] | slice(5,0,-1) }} → [5,4,3,2,1]
```

### `sort_by`

Sorts array by attribute value.

**Parameters:**

- `seq`: Input array
- `attr`: Attribute path (dot notation)

**Example:**

Given an external JSON file:
```json
[
  {"name":"Bob","age":30},
  {"name":"Alice","age":25}
]
```

Loaded as variable `people`:
```
{{ people | sort_by("name") }}
→ [{"name":"Alice","age":25},{"name":"Bob","age":30}]
```

### `sum`

Calculates sum of numeric values in array.

**Parameters:**

- `arr`: Input array (numeric values or numeric strings)

**Returns:** Number (double)

**Example:**

```
{{ [1,2,3,4] | sum }} → 10
{{ ["1.5","2.5",3] | sum }} → 7.0
```

### `tojson`

Converts value to JSON string representation.

**Parameters:**

- `value`: Any value

**Returns:** String

**Note:** This filter requires data to be accessible from the Inja context.

**Example:**

Given a variable `data` with `{"name":"John"}` loaded from an external JSON file:
```
{{ data | tojson }} → "{\"name\":\"John\"}"
```

### `unique`

Removes duplicate values from array.

**Parameters:**

- `arr`: Input array

**Returns:** Array with unique elements (preserves first occurrence)

**Example:**

```
{{ [1,2,2,3,3,3,1] | unique }} → [1,2,3]
```

### `zip`

Zips multiple arrays into array of tuples (shortest length).

**Parameters:** Any number of arrays

**Example:**

```
{{ [1,2,3], ["a","b","c"] | zip }} → [[1,"a"],[2,"b"],[3,"c"]]
```

### `zip_array`

Zips an array of arrays into array of tuples.

**Parameters:**

- `arrays`: Array containing sub-arrays to zip

**Example:**

```
{{ [[1,2,3],["a","b","c"]] | zip_array }} → [[1,"a"],[2,"b"],[3,"c"]]
```

### `zip_obj`

Zips keys array with data arrays into array of objects.

**Parameters:**

- `keys`: Array of key names (strings)
- `arrays`: One or more arrays to zip (at least 1)

**Example:**

```
{{ ["name","age"], ["Alice","Bob"], [25,30] | zip_obj }}
→ [{"name":"Alice","age":25},{"name":"Bob","age":30}]
```

## TESTS FOR ATTRIBUTES

When using `selectattr` and `rejectattr`, the following tests are available:

| Test        | Description                                  | Test Argument |
| :---------- | :------------------------------------------- | :------------ |
| (empty)     | Truthy: non-null, non-empty, non-zero        | None          |
| `"equalto"` | Equal to test_arg                            | Required      |
| `"defined"` | Not null                                     | None          |
| `"none"`    | Is null                                      | None          |
| `"in"`      | Value exists in array or substring in string | Required      |
| `"match"`   | Regex matches (strings only)                 | Required      |

## SEE ALSO

string-filters(7), html-filters(7), number-filters(7), tests(7)