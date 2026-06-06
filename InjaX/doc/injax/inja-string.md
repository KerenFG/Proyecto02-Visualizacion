# STRING-FILTERS(7)

## NAME

string-filters - InjaX string manipulation functions

## SYNOPSIS

Filter operations on strings: centering, reversing, trimming, truncating, titling, word counting, wrapping, and conversion.

## FILTERS

### `center`

Centers a string within a field of specified width.

**Parameters:**

- `input`: String to center
- `width`: Field width (if ≤ input length, returns input)
- `fill`: Fill character(s) (default: space)

**Examples:**

```
{{ "hello" | center(11, "-") }} → "---hello---"
{{ "hi" | center(5) }} → "  hi "
```

**Example with external JSON data:**

Given an external JSON file `text.json`:
```json
{
  "message": "hello",
  "field_width": 11,
  "padding": "-"
}
```

Loaded as variable `data`:
```
{{ data.message | center(data.field_width, data.padding) }} → "---hello---"
```

### `reverse`

Reverses the characters of a string.

**Parameters:**

- `input`: String to reverse

**Example:**

```
{{ "hello" | reverse }} → "olleh"
```

**Example with external JSON data:**

Given an external JSON file `data.json`:
```json
{"word": "hello"}
```

Loaded as variable `data`:
```
{{ data.word | reverse }} → "olleh"
```

### `string`

Converts any value to its string representation.

**Parameters:**

- `value`: Any value (number, boolean, null, array, object)

**Examples with literal values:**

```
{{ 42 | string }} → "42"
{{ true | string }} → "true"
{{ null | string }} → ""
```

**Example with external JSON data:**

Given an external JSON file `values.json`:
```json
{
  "number": 42,
  "flag": true,
  "empty": null,
  "nested": {"a": 1}
}
```

Loaded as variable `data`:
```
{{ data.number | string }} → "42"
{{ data.flag | string }} → "true"
{{ data.empty | string }} → ""
{{ data.nested | string }} → "{\"a\":1}"
```

### `title`

Converts string to title case (first letter of each word capitalized, rest lowercase).

**Parameters:**

- `input`: String to convert

**Examples:**

```
{{ "hello WORLD" | title }} → "Hello World"
{{ "the-quick_brown fox" | title }} → "The-Quick_Brown Fox"
```

**Example with external JSON data:**

Given an external JSON file `phrases.json`:
```json
{
  "phrase1": "hello WORLD",
  "phrase2": "the-quick_brown fox"
}
```

Loaded as variable `phrases`:
```
{{ phrases.phrase1 | title }} → "Hello World"
{{ phrases.phrase2 | title }} → "The-Quick_Brown Fox"
```

### `trim`

Removes leading and trailing whitespace.

**Parameters:**

- `input`: String to trim

**Whitespace:** space, tab, newline, carriage return, form feed, vertical tab

**Examples:**

```
{{ "  hello  " | trim }} → "hello"
{{ "\n\t test \r\n" | trim }} → "test"
```

**Example with external JSON data:**

Given an external JSON file `userinput.json`:
```json
{"name": "  John Doe  \n"}
```

Loaded as variable `user`:
```
{{ user.name | trim }} → "John Doe"
```

### `truncate`

Truncates string to specified length with custom ending.

**Parameters:**

- `input`: String to truncate
- `length`: Maximum length (must be ≥ 1)
- `end`: Suffix to append when truncated (default: "...")

**Examples:**

```
{{ "Hello world" | truncate(8) }} → "Hello..."
{{ "Hello world" | truncate(10, "!") }} → "Hello wor!"
{{ "Hello" | truncate(10) }} → "Hello"
```

**Example with external JSON data:**

Given an external JSON file `content.json`:
```json
{
  "text": "Hello world",
  "max_length": 8,
  "suffix": "..."
}
```

Loaded as variable `content`:
```
{{ content.text | truncate(content.max_length, content.suffix) }} → "Hello..."
```

### `wordcount`

Counts number of words in string.

**Parameters:**

- `input`: String to analyze

**Returns:** Integer

**Examples:**

```
{{ "Hello world from InjaX" | wordcount }} → 4
{{ "One. Two? Three!" | wordcount }} → 3
```

**Example with external JSON data:**

Given an external JSON file `document.json`:
```json
{"paragraph": "Hello world from InjaX"}
```

Loaded as variable `doc`:
```
{{ doc.paragraph | wordcount }} → 4
```

### `wordwrap`

Wraps text to specified line width.

**Parameters:**

- `input`: Text to wrap
- `width`: Maximum line width
- `break_str`: Line break string (default: "\n")
- `break_long_words`: Whether to break words longer than width (default: false)

**Examples:**

```
{{ "This is a long sentence" | wordwrap(10) }}
→ "This is a\nlong\nsentence"

{{ "Supercalifragilistic" | wordwrap(5, "\n", true) }}
→ "Super\ncalif\nragil\nistic"
```

**Example with external JSON data:**

Given an external JSON file `textconfig.json`:
```json
{
  "content": "This is a long sentence",
  "line_width": 10,
  "newline": "\n",
  "break_words": false
}
```

Loaded as variable `config`:
```
{{ config.content | wordwrap(config.line_width, config.newline, config.break_words) }}
→ "This is a\nlong\nsentence"
```

### Complete example

*File: article.json*

```json
{
  "title": "  hello WORLD  ",
  "content": "This is a very long article that needs to be displayed properly.",
  "max_title_length": 10,
  "wrap_width": 15
}
```

*File: template.inja*
```
Title: {{ article.title | trim | title }}
Content preview: {{ article.content | truncate(article.max_title_length) }}
Wrapped content:
{{ article.content | wordwrap(article.wrap_width) }}
Reversed title: {{ article.title | trim | reverse }}
```

*Output:*
```
Title: Hello World
Content preview: This is...
Wrapped content:
This is a very
long article
that needs to be
displayed
properly.
Reversed title: DLROW OLLEH
```

## SEE ALSO

array-filters(7), html-filters(7), number-filters(7), tests(7)
