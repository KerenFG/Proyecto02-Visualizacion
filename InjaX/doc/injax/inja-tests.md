# TESTS(7)

## NAME

tests - InjaX type-checking and condition functions

## SYNOPSIS

Test functions for use in conditional expressions: type checking, value testing, and pattern matching.

## TESTS

### `contains`

Checks if value exists in array, string, or object.

**Parameters:**

- `haystack`: Array, string, or object to search
- `needle`: Value to find

**Returns:** Boolean

**Behavior:**

- Array: element equality
- String: substring presence
- Object: key existence (needle must be string)

**Examples with literal values:**

```
{% if contains([1,2,3], 2) %}yes{% endif %} → "yes"
{% if contains("hello", "ell") %}yes{% endif %} → "yes"
```

**Examples with external JSON data:**

Given an external JSON file `data.json`:
```json
{
  "numbers": [1, 2, 3],
  "text": "hello",
  "obj": {"a": 1}
}
```

Loaded as variable `data`:
```
{% if contains(data.numbers, 2) %}yes{% endif %} → "yes"
{% if contains(data.text, "ell") %}yes{% endif %} → "yes"
{% if contains(data.obj, "a") %}yes{% endif %} → "yes"
```

### `endsWith`

Checks if string ends with suffix.

**Parameters:**

- `value`: String to check
- `suffix`: Suffix to test

**Example with literal value:**

```
{% if endsWith("hello.txt", ".txt") %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `files.json`:
```json
{"filename": "hello.txt"}
```

Loaded as variable `file`:
```
{% if endsWith(file.filename, ".txt") %}yes{% endif %} → "yes"
```

### `hasKey`

Checks if object contains specific key.

**Parameters:**

- `obj`: Object to check
- `key`: Key name (string)

**Example with external JSON data:**

Given an external JSON file `person.json`:
```json
{"name": "John", "age": 30}
```

Loaded as variable `person`:
```
{% if hasKey(person, "age") %}yes{% endif %} → "yes"
```

### `isArray`

Checks if value is an array.

**Parameters:** Single value

**Example with literal value:**

```
{% if isArray([1,2,3]) %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `data.json`:
```json
{"items": [1, 2, 3]}
```

Loaded as variable `data`:
```
{% if isArray(data.items) %}yes{% endif %} → "yes"
```

### `isBoolean`

Checks if value is boolean (true/false).

**Parameters:** Single value

**Example with literal value:**

```
{% if isBoolean(true) %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `config.json`:
```json
{"enabled": true}
```

Loaded as variable `config`:
```
{% if isBoolean(config.enabled) %}yes{% endif %} → "yes"
```

### `isContained`

Checks if item exists in array (reverse of contains).

**Parameters:**

- `item`: Value to find
- `collection`: Array to search

**Example with literal value:**

```
{% if isContained(2, [1,2,3]) %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `dataset.json`:
```json
{
  "value": 2,
  "list": [1, 2, 3]
}
```

Loaded as variable `data`:
```
{% if isContained(data.value, data.list) %}yes{% endif %} → "yes"
```

### `isDefined`

Checks if value is not null.

**Parameters:** Single value

**Example with literal value:**

```
{% if isDefined(null) %}no{% else %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `data.json`:
```json
{"value": null}
```

Loaded as variable `data`:
```
{% if isDefined(data.value) %}no{% else %}yes{% endif %} → "yes"
```

### `isDivisibleBy`

Checks if integer is divisible by another integer.

**Parameters:**

- `dividend`: Value to test
- `divisor`: Divisor (non-zero)

**Example with literal value:**

```
{% if isDivisibleBy(10, 2) %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `numbers.json`:
```json
{"number": 10, "divisor": 2}
```

Loaded as variable `nums`:
```
{% if isDivisibleBy(nums.number, nums.divisor) %}yes{% endif %} → "yes"
```

### `isEmpty`

Checks if string, array, or object is empty.

**Parameters:** Single value

**Examples with literal values:**

```
{% if isEmpty("") %}yes{% endif %} → "yes"
{% if isEmpty([]) %}yes{% endif %} → "yes"
```

**Examples with external JSON data:**

Given an external JSON file `data.json`:
```json
{
  "empty_string": "",
  "empty_array": [],
  "non_empty": "hello"
}
```

Loaded as variable `data`:
```
{% if isEmpty(data.empty_string) %}yes{% endif %} → "yes"
{% if isEmpty(data.empty_array) %}yes{% endif %} → "yes"
{% if isEmpty(data.non_empty) %}no{% endif %} → "no"
```

### `isEscaped`

Checks if string contains HTML escape sequences.

**Parameters:** Single value

**Example with literal value:**

```
{% if isEscaped("&lt;tag&gt;") %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `html.json`:
```json
{"escaped": "&lt;tag&gt;", "raw": "<tag>"}
```

Loaded as variable `html`:
```
{% if isEscaped(html.escaped) %}yes{% endif %} → "yes"
{% if isEscaped(html.raw) %}no{% endif %} → "no"
```

### `isEven`

Checks if integer is even.

**Parameters:** Single value

**Example with literal value:**

```
{% if isEven(42) %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `numbers.json`:
```json
{"value": 42}
```

Loaded as variable `num`:
```
{% if isEven(num.value) %}yes{% endif %} → "yes"
```

### `isFloat`

Checks if value is floating-point number.

**Parameters:** Single value

**Example with literal value:**

```
{% if isFloat(3.14) %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `values.json`:
```json
{"pi": 3.14, "answer": 42}
```

Loaded as variable `vals`:
```
{% if isFloat(vals.pi) %}yes{% endif %} → "yes"
{% if isFloat(vals.answer) %}no{% endif %} → "no"
```

### `isInteger`

Checks if value is integer number.

**Parameters:** Single value

**Example with literal value:**

```
{% if isInteger(42) %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `values.json`:
```json
{"count": 42, "pi": 3.14}
```

Loaded as variable `vals`:
```
{% if isInteger(vals.count) %}yes{% endif %} → "yes"
{% if isInteger(vals.pi) %}no{% endif %} → "no"
```

### `isIterable`

Checks if value is iterable (array or object).

**Parameters:** Single value

**Examples with literal values:**

```
{% if isIterable([1,2,3]) %}yes{% endif %} → "yes"
```

**Examples with external JSON data:**

Given an external JSON file `data.json`:
```json
{
  "list": [1, 2, 3],
  "dict": {"a": 1},
  "string": "hello"
}
```

Loaded as variable `data`:
```
{% if isIterable(data.list) %}yes{% endif %} → "yes"
{% if isIterable(data.dict) %}yes{% endif %} → "yes"
{% if isIterable(data.string) %}no{% endif %} → "no"
```

### `isLower`

Checks if string contains no uppercase letters (non-letters ignored).

**Parameters:** Single value

**Example with literal value:**

```
{% if isLower("hello 123") %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `strings.json`:
```json
{"lower": "hello 123", "upper": "HELLO 123"}
```

Loaded as variable `str`:
```
{% if isLower(str.lower) %}yes{% endif %} → "yes"
{% if isLower(str.upper) %}no{% endif %} → "no"
```

### `isMapping`

Checks if value is object (dictionary/map).

**Parameters:** Single value

**Example with external JSON data:**

Given an external JSON file `data.json`:
```json
{"obj": {"a": 1}, "arr": [1, 2, 3]}
```

Loaded as variable `data`:
```
{% if isMapping(data.obj) %}yes{% endif %} → "yes"
{% if isMapping(data.arr) %}no{% endif %} → "no"
```

### `isNone`

Checks if value is null.

**Parameters:** Single value

**Example with literal value:**

```
{% if isNone(null) %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `data.json`:
```json
{"value": null}
```

Loaded as variable `data`:
```
{% if isNone(data.value) %}yes{% endif %} → "yes"
```

### `isNull`

Alias for `isNone`. Checks if value is null.

**Example with external JSON data:**

Given an external JSON file `data.json`:
```json
{"value": null}
```

Loaded as variable `data`:
```
{% if isNull(data.value) %}yes{% endif %} → "yes"
```

### `isNumber`

Checks if value is number (integer or float).

**Parameters:** Single value

**Example with literal value:**

```
{% if isNumber(42) %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `data.json`:
```json
{"count": 42, "pi": 3.14, "text": "hello"}
```

Loaded as variable `data`:
```
{% if isNumber(data.count) %}yes{% endif %} → "yes"
{% if isNumber(data.pi) %}yes{% endif %} → "yes"
{% if isNumber(data.text) %}no{% endif %} → "no"
```

### `isObject`

Checks if value is object.

**Parameters:** Single value

**Example with external JSON data:**

Given an external JSON file `data.json`:
```json
{"obj": {"a": 1}, "arr": [1, 2, 3]}
```

Loaded as variable `data`:
```
{% if isObject(data.obj) %}yes{% endif %} → "yes"
{% if isObject(data.arr) %}no{% endif %} → "no"
```

### `isOdd`

Checks if integer is odd.

**Parameters:** Single value

**Example with literal value:**

```
{% if isOdd(41) %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `numbers.json`:
```json
{"value": 41}
```

Loaded as variable `num`:
```
{% if isOdd(num.value) %}yes{% endif %} → "yes"
```

### `isSequence`

Checks if value is array.

**Parameters:** Single value

**Example with literal value:**

```
{% if isSequence([1,2,3]) %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `data.json`:
```json
{"items": [1, 2, 3], "obj": {"a": 1}}
```

Loaded as variable `data`:
```
{% if isSequence(data.items) %}yes{% endif %} → "yes"
{% if isSequence(data.obj) %}no{% endif %} → "no"
```

### `isString`

Checks if value is string.

**Parameters:** Single value

**Example with literal value:**

```
{% if isString("hello") %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `data.json`:
```json
{"text": "hello", "number": 42}
```

Loaded as variable `data`:
```
{% if isString(data.text) %}yes{% endif %} → "yes"
{% if isString(data.number) %}no{% endif %} → "no"
```

### `isUndefined`

Checks if value is null (alias for isNone).

**Parameters:** Single value

**Example with external JSON data:**

Given an external JSON file `data.json`:
```json
{"value": null}
```

Loaded as variable `data`:
```
{% if isUndefined(data.value) %}yes{% endif %} → "yes"
```

### `isUpper`

Checks if string contains no lowercase letters (non-letters ignored).

**Parameters:** Single value

**Example with literal value:**

```
{% if isUpper("HELLO 123") %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `strings.json`:
```json
{"upper": "HELLO 123", "lower": "hello 123"}
```

Loaded as variable `str`:
```
{% if isUpper(str.upper) %}yes{% endif %} → "yes"
{% if isUpper(str.lower) %}no{% endif %} → "no"
```

### `matches`

Checks if string matches regular expression.

**Parameters:**

- `value`: String to test
- `pattern`: Regular expression pattern

**Note:** Uses `std::regex` (ECMAScript syntax).

**Example with literal value:**

```
{% if matches("user@example.com", "^[^@]+@[^@]+\.[^@]+$") %}
  valid email
{% endif %}
```

**Example with external JSON data:**

Given an external JSON file `emails.json`:
```json
{"email": "user@example.com"}
```

Loaded as variable `contact`:
```
{% if matches(contact.email, "^[^@]+@[^@]+\.[^@]+$") %}
  valid email
{% endif %}
```

### `startsWith`

Checks if string starts with prefix.

**Parameters:**

- `value`: String to check
- `prefix`: Prefix to test

**Example with literal value:**

```
{% if startsWith("hello.txt", "hello") %}yes{% endif %} → "yes"
```

**Example with external JSON data:**

Given an external JSON file `files.json`:
```json
{"filename": "hello.txt"}
```

Loaded as variable `file`:
```
{% if startsWith(file.filename, "hello") %}yes{% endif %} → "yes"
```

## Complete example 

*File: userdata.json*
```json
{
  "name": "John Doe",
  "email": "john@example.com",
  "age": 25,
  "tags": ["admin", "user", "verified"],
  "active": true,
  "metadata": null,
  "config": {"theme": "dark"}
}
```

*File: template.inja*
```
{% if isString(user.name) %}Name is valid{% endif %}
{% if matches(user.email, "^[^@]+@[^@]+\.[^@]+$") %}Email format OK{% endif %}
{% if isNumber(user.age) and user.age >= 18 %}Adult{% endif %}
{% if isContained("admin", user.tags) %}Admin user{% endif %}
{% if isBoolean(user.active) and user.active %}Account active{% endif %}
{% if isNone(user.metadata) %}No metadata{% endif %}
{% if isMapping(user.config) %}Configuration present{% endif %}
```

## SEE ALSO

array-filters(7), string-filters(7), html-filters(7), number-filters(7)
