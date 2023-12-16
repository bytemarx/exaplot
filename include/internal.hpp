#pragma once

#include "orbital.hpp"


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

typedef struct s_orbital_state {
    // const orbital::OrbitalInterface* iface;
    const orbital::OrbitalCore::_OrbIFace* iface;
} orbital_state;


extern "C" {

int orbitalExec(PyObject* module);
PyObject* orbital_init(PyObject*, PyObject* const*, Py_ssize_t, PyObject*);
PyObject* orbital_msg(PyObject*, PyObject*, PyObject*);
PyObject* orbital_plot(PyObject*, PyObject* const*, Py_ssize_t);
PyObject* orbital_stop(PyObject*, PyObject*);

}