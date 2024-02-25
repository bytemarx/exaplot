import orbital


try:
    orbital.init(None)
except TypeError as e:
    assert(str(e) == "init() argument #1 must be type 'str'")

try:
    orbital.init(1)
except TypeError as e:
    assert(str(e) == "init() argument #1 must be type 'str'")

try:
    orbital.init(1, 2, 3)
except TypeError as e:
    assert(str(e) == "init() argument #1 must be type 'str'")

try:
    orbital.init("a", 1)
except TypeError as e:
    assert(str(e) == "init() argument #2 must be type 'str'")

try:
    orbital.init(plots=None)
except TypeError as e:
    assert(str(e) == "init() plots must be either an 'int' or 'list' type")

try:
    orbital.init(plots=1.5)
except TypeError as e:
    assert(str(e) == "init() plots must be either an 'int' or 'list' type")

try:
    orbital.init(plots=0)
except ValueError as e:
    assert(str(e) == "init() 'plots' keyword must be an integer greater than zero")

try:
    orbital.init(plots=-1)
except ValueError as e:
    assert(str(e) == "init() 'plots' keyword must be an integer greater than zero")

try:
    orbital.init(plots=[])
except ValueError as e:
    assert(str(e) == "init() plots list is missing entries")

try:
    orbital.init(plots=[None])
except TypeError as e:
    assert(str(e) == "init() plot #1 must be type 'tuple'")

try:
    orbital.init(plots=[(0,)])
except TypeError as e:
    assert(str(e) == "init() plot #1 must be a 4-tuple")

try:
    orbital.init(plots=[(0,0)])
except TypeError as e:
    assert(str(e) == "init() plot #1 must be a 4-tuple")

try:
    orbital.init(plots=[(0,0,0)])
except TypeError as e:
    assert(str(e) == "init() plot #1 must be a 4-tuple")

try:
    orbital.init(plots=[(0,0,0,0,0)])
except TypeError as e:
    assert(str(e) == "init() plot #1 must be a 4-tuple")

try:
    orbital.init(plots=[(None, None, None, None)])
except TypeError as e:
    assert(str(e) == "init() entry #1 of plot #1 must be type 'int'")

try:
    orbital.init(plots=[(0, None, None, None)])
except TypeError as e:
    assert(str(e) == "init() entry #2 of plot #1 must be type 'int'")

try:
    orbital.init(plots=[(0, 0, 0, -1)])
except ValueError as e:
    assert(str(e) == "init() entry #4 of plot #1 is invalid: -1")

try:
    orbital.init(plots=[(0, 0, 0, 0), None])
except TypeError as e:
    assert(str(e) == "init() plot #2 must be type 'tuple'")

try:
    orbital.init(plots=[(0, 0, 0, 0), None])
except TypeError as e:
    assert(str(e) == "init() plot #2 must be type 'tuple'")

try:
    orbital.init(foo=None)
except TypeError as e:
    assert(str(e) == "init() got an unexpected keyword argument 'foo'")
