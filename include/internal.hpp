#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>


typedef struct _is {
    PyInterpreterState *next;
    int64_t id;
    int64_t id_refcount;
    int requires_idref;
    PyThread_type_lock id_mutex;
    int _initialized;
    int finalizing;
    uint64_t monitoring_version;
    uint64_t last_restart_version;
    struct pythreads {
        uint64_t next_unique_id;
        PyThreadState *head;
        long count;
        size_t stacksize;
    } threads;

    // CUSTOM ORBITAL MODIFICATION
    const void *orb_passthrough;

} PyInterpreterState;
