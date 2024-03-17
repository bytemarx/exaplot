#pragma once

#include "orbital.hpp"

#include <optional>


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
    void *orb_passthrough;

} PyInterpreterState;

typedef struct s_orbital_state {
    PyTypeObject* type_RunParam;
    PyObject* obj_InterruptException;
    orbital::OrbitalInterface* iface;
} orbital_state;


typedef struct {
    PyObject_HEAD
    orbital::RunParamType type;
    PyObject* value;
    PyObject* display;
} PyRunParam;


namespace orbital {

orbital_state* getModuleStateFromObject(PyObject*);
orbital_state* getModuleStateFromObjectType(PyTypeObject*);

}


extern "C" {

int moduleSlot_initTypes(PyObject*);
int moduleSlot_initExceptions(PyObject*);
int moduleSlot_initInterface(PyObject*);
int module_traverse(PyObject*, visitproc, void*);
int module_clear(PyObject*);
PyObject* orbital_init(PyObject*, PyObject* const*, Py_ssize_t, PyObject*);
PyObject* orbital_stop(PyObject*, PyObject*);
PyObject* orbital_msg(PyObject*, PyObject*, PyObject*);
PyObject* orbital_plot(PyObject*, PyObject* const*, Py_ssize_t);
PyObject* orbital__set_plot_property(PyObject*, PyObject*);
PyObject* orbital__get_plot_property(PyObject*, PyObject*);
PyObject* orbital__show_plot(PyObject*, PyObject*);

}
