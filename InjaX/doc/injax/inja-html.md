# HTML-FILTERS(7)

## NAME

html-filters - InjaX HTML escaping and formatting functions

## SYNOPSIS

Filter operations for HTML contexts: escaping, URL encoding, tag stripping, link generation, and XML attribute formatting.

## FILTERS

### `escape`

Escapes HTML special characters in string.

**Parameters:**

- `value`: String or value to escape

**Escaped characters:**

| Character | Entity |
| :-------- | :----- |
| &         | &amp;  |
| <         | &lt;   |
| >         | &gt;   |
| "         | &quot; |
| '         | &#39;  |
| /         | &#47;  |

**Example:**

```
{{ "<script>alert('xss')</script>" | escape }}
→ "&lt;script&gt;alert(&#39;xss&#39;)&lt;/script&gt;"
```

### `forceescape`

Alias for `escape`. Forces HTML escaping even if auto-escaping is disabled.

**Parameters:** Same as `escape`

### `safe`

Returns raw string without escaping (bypasses auto-escaping).

**Parameters:**

- `value`: Value to output as-is

**Warning:** Use only with trusted content to avoid XSS vulnerabilities.

**Example:**

```
{{ "<strong>Bold</strong>" | safe }}
→ "<strong>Bold</strong>"
```

### `striptags`

Removes all HTML/XML tags from string.

**Parameters:**

- `input`: String containing tags

**Example:**

```
{{ "<p>Hello <b>world</b>!</p>" | striptags }} → "Hello world!"
```

### `urlencode`

Encodes string for use in URL query parameters.

**Parameters:**

- `input`: String to encode

**Encoding:** Alphanumeric characters and `-_.~` remain unchanged; all others become `%XX` hexadecimal.

**Example:**

```
{{ "hello world" | urlencode }} → "hello%20world"
{{ "café & tea" | urlencode }} → "caf%C3%A9%20%26%20tea"
```

### `urlize`

Converts URLs in text to clickable HTML links.

**Parameters:**

- `input`: Text containing URLs
- `max_length`: Maximum link text length before truncation (default: 50, requires second parameter)

**Features:**

- Recognizes http://, https://, and www. URLs
- Auto-prepends http:// to www. links
- Truncates long URLs with "..."
- HTML-escapes link text

**Example:**

```
{{ "Visit https://example.com/path" | urlize }}
→ "Visit <a href=\"https://example.com/path\">https://example.com/path</a>"

{{ "www.example.com/very/long/path" | urlize(15) }}
→ "<a href=\"http://www.example.com/very/long/path\">www.exa...path</a>"
```

### `xmlattr`

Converts dictionary/object to XML/HTML attributes string.

**Parameters:**

- `obj`: Dictionary/object with attribute key-value pairs (loaded from external JSON file)

**Returns:** String starting with space, containing `key="value"` pairs, values HTML-escaped.

**Example:**

Given an external JSON file:
```json
{"class": "btn", "id": "submit", "disabled": true}
```

Loaded as variable `attrs` in the Inja context:
```
{{ attrs | xmlattr }}
→ " class=\"btn\" id=\"submit\" disabled=\"true\""
```

**Usage in template:**

Given an external JSON file `attributes.json`:
```json
{"class": "highlight", "data-id": 123}
```

Loaded as variable `html_attrs`:
```html
<div{{ html_attrs | xmlattr }}>Content</div>
```

→ `<div class="highlight" data-id="123">Content</div>`

## SEE ALSO

array-filters(7), string-filters(7), number-filters(7), tests(7)