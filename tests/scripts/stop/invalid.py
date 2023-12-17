import orbital


try:
    orbital.stop(None)
except TypeError as e:
    assert(str(e) == "stop() argument must be type 'int' or 'str'")

try:
    orbital.stop('')
except IndexError as e:
    assert(str(e) == "string index out of range: ''")

try:
    orbital.stop('__orbital__')
except ValueError as e:
    assert(str(e) == "invalid literal for std::stoull() with base 10: ''")

try:
    orbital.stop('__orbital___')
except ValueError as e:
    assert(str(e) == "invalid literal for std::stoull() with base 10: '_'")

try:
    orbital.stop('__orbital__!')
except ValueError as e:
    assert(str(e) == "invalid literal for std::stoull() with base 10: '!'")

try:
    orbital.stop('__orbital__1')
except SystemError as e:
    assert(str(e) == "invalid script ID: 1")

try:
    orbital.stop('__orbital__-1')
except SystemError as e:
    assert(str(e) == "invalid script ID: -1")

try:
    orbital.stop('__orbital__10')
except SystemError as e:
    assert(str(e) == "invalid script ID: 10")

try:
    orbital.stop('__orbital__99999999999999999999')
except OverflowError as e:
    assert(str(e) == "int too big to convert: '99999999999999999999'")
