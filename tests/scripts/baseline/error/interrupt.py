from _orbital import _Interrupt

try:
    raise _Interrupt
except Exception:
    pass
