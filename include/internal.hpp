#pragma once

#include "zetaplot.hpp"

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

    void *passthrough;

} PyInterpreterState;

typedef struct {
    PyTypeObject* type_RunParam;
    PyObject* obj_InterruptException;
    zeta::Interface* iface;
} zeta_state;


typedef struct {
    PyObject_HEAD
    zeta::RunParamType type;
    PyObject* value;
    PyObject* display;
} PyRunParam;


namespace zeta {

zeta_state* getModuleStateFromObject(PyObject*);

}


extern "C" {

int moduleSlot_initTypes(PyObject*);
int moduleSlot_initExceptions(PyObject*);
int moduleSlot_initInterface(PyObject*);
int module_traverse(PyObject*, visitproc, void*);
int module_clear(PyObject*);
PyObject* zeta_init(PyObject*, PyObject* const*, Py_ssize_t, PyObject*);
PyObject* zeta_stop(PyObject*, PyObject*);
PyObject* zeta_msg(PyObject*, PyObject*, PyObject*);
PyObject* zeta_plot(PyObject*, PyObject* const*, Py_ssize_t);
PyObject* zeta__set_plot_property(PyObject*, PyObject*);
PyObject* zeta__get_plot_property(PyObject*, PyObject*);
PyObject* zeta__show_plot(PyObject*, PyObject*);

}
