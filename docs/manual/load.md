# LOAD

**Category:** Command  
**Syntax:** `LOAD`

## Description

The `LOAD` command initiates an interactive entry mode that allows you to type or paste a BASIC program line by line directly from the terminal. Upon entering your first valid line, any previously stored program is immediately cleared from memory. As you continue inputting code, your new lines are automatically sorted and stored sequentially by their line numbers, regardless of the order in which you type them. 

To finish coding and return to the command prompt, you simply press *Enter* key on a blank line. Alternatively, you can press *Esc* key at any time to immediately cancel the entry session; however, this does not roll back the system; any valid lines already accepted during the current session are kept, and the old program cleared at the start remains erased.

## See also

- [LIST](list.md)
- [RUN](run.md)
- [NEW](new.md)

## Example

```basic
LOAD
```
