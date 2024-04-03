## Overview

```
         ___________________________________________________________
        |                                                           |
        |   APP                                                     |
        |         +---------------+                                 |
        |         |               |                                 |
        |         |      UI       |                                 |
        |         |               |                                 |
        |         +---------------+                                 |
        |               |   ^                                       |
        |               v   |                                       |
        |         +---------------+          +------------+         |
        |         |               |          |            |         |
        |         |   INTERFACE   |          |   SCRIPT   |         |
        |         |               |          |            |         |
        |         +---------------+          +------------+         |
        |               |   ^                     |  |              |
        |_______________|___|_____________________|  |______________|
                        |   |                    _|  |_
                        |   |                    \    /
         _______________|___|_____________________\  /______________
        |               |   |                      \/               |
        |   CORE        |   |                 +------------+        |
        |               v   |               +------------+ |        |
        |         +---------------+       +------------+ | |        |
        |         |               |       |            | | |        |
        |         |    EXAPLOT    |<------|   SCRIPT   | | |        |
        |         |    MODULE     |------>|   MODULE   | |-+        |
        |         |               |       |            |-+          |
        |         +---------------+       +------------+            |
        |___________________________________________________________|

```


## Core Library
The core library handles (almost) all Python-related activity. Given a script, it loads this script
into the (sub)interpreter's runtime and returns a handle to the script module. The application can
invoke the `run` (or any other) function within the script module through its handle.

Each `exa::Core` object manages a CPython subinterpreter and can load multiple scripts into its
runtime (but the application currently only uses one core with one script at a time).


## Application
The application is further compartmentalized via several management objects: the UI manager,
`AppUI`, the module interface, `Interface`, and the supervisor/mediator of these components,
`AppMain`.
```
                                                 ,-------------.
                                                (  UI ELEMENTS  )
                                                 `-------------'
                                                       /\
                                                       ||
                                                       \/
        +---------------+   +---------------+   +---------------+
        |               |   |               |   |               |
        |   Interface   |<=>|    AppMain    |<=>|     AppUI     |
        |               |   |               |   |               |
        +---------------+   +---------------+   +---------------+
               /\
               ||
               \/
         ,-------------.
        (    MODULE     )
         `-------------'
```
