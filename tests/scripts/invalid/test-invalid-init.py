import orbital


try:
    orbital.init(None)
except TypeError as e:
    assert(str(e) == "init() 'plots' argument must be either an 'int' or 'list' type")

try:
    orbital.init(1.5)
except TypeError as e:
    assert(str(e) == "init() 'plots' argument must be either an 'int' or 'list' type")

try:
    orbital.init(0)
except ValueError as e:
    assert(str(e) == "init() 'plots' argument must be an integer greater than zero")

try:
    orbital.init(-1)
except ValueError as e:
    assert(str(e) == "init() 'plots' argument must be an integer greater than zero")

try:
    orbital.init([])
except ValueError as e:
    assert(str(e) == "init() plots list is missing entries")

try:
    orbital.init([None])
except TypeError as e:
    assert(str(e) == "init() 'plots[0]' value must be type 'tuple[int, int, int, int]'")

try:
    orbital.init([(0,)])
except TypeError as e:
    assert(str(e) == "init() 'plots[0]' value must be type 'tuple[int, int, int, int]'")

try:
    orbital.init([(0,0)])
except TypeError as e:
    assert(str(e) == "init() 'plots[0]' value must be type 'tuple[int, int, int, int]'")

try:
    orbital.init([(0,0,0)])
except TypeError as e:
    assert(str(e) == "init() 'plots[0]' value must be type 'tuple[int, int, int, int]'")

try:
    orbital.init([(0,0,0,0,0)])
except TypeError as e:
    assert(str(e) == "init() 'plots[0]' value must be type 'tuple[int, int, int, int]'")

try:
    orbital.init([(None, None, None, None)])
except TypeError as e:
    assert(str(e) == "init() 'plots[0][0]' value must be type 'int'")

try:
    orbital.init([(0, None, None, None)])
except TypeError as e:
    assert(str(e) == "init() 'plots[0][1]' value must be type 'int'")

try:
    orbital.init([(0, 0, 0, -1)])
except ValueError as e:
    assert(str(e) == "init() 'plots[0][3]' value is invalid: -1")

try:
    orbital.init([(0, 0, 0, 0), None])
except TypeError as e:
    assert(str(e) == "init() 'plots[1]' value must be type 'tuple[int, int, int, int]'")

try:
    orbital.init(foo=None)
except ValueError as e:
    assert(str(e) == "init() invalid value for parameter 'foo'")
