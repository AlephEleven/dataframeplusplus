# dataframeplusplus

DataFrame library for C++, using generator-based implementation, rather than saving dataset into memory.

## Applications
- Reads file to discover table shape, label names, types and filesize
- Query column data based on column names, returned in string format.
- Allows function passing during query, intended for converting to specific type (i.e Cost column converted to double).
- Create histogram bins, to preview distribution of data.
- Uses vectors as the equivalent to series in pandas.

# Known issues

## Delimiter inside column
- If the delimiter exists in a given column, the second half, will be considered a part of the next column.
- Can be fixed easily by punctuation removal, Find & Replace delimiter in excel.

Ex:
```
"Braund, Mr. Owen Harris" -> "Braund| Mr. Owen Harris"
```
