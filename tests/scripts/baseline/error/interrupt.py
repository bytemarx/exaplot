from _exaplot import _Interrupt

try:
    raise _Interrupt
except Exception:
    pass
