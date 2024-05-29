/*
 * ExaPlot
 * private declarations
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include "exaplot.hpp"

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
    exa::Interface* iface;
} exa_state;


typedef struct {
    PyObject_HEAD
    exa::RunParamType type;
    PyObject* value;
    PyObject* display;
} PyRunParam;


namespace exa {

exa_state* getModuleStateFromObject(PyObject*);

}


extern "C" {

int moduleSlot_initTypes(PyObject*);
int moduleSlot_initExceptions(PyObject*);
int moduleSlot_initInterface(PyObject*);
int module_traverse(PyObject*, visitproc, void*);
int module_clear(PyObject*);
PyObject* exa_init(PyObject*, PyObject* const*, Py_ssize_t, PyObject*);
PyObject* exa_stop(PyObject*, PyObject*);
PyObject* exa_msg(PyObject*, PyObject*, PyObject*);
PyObject* exa_datafile(PyObject*, PyObject*, PyObject*);
PyObject* exa_plot(PyObject*, PyObject* const*, Py_ssize_t, PyObject*);
PyObject* exa__set_plot_property(PyObject*, PyObject*);
PyObject* exa__get_plot_property(PyObject*, PyObject*);
PyObject* exa__show_plot(PyObject*, PyObject*);

}
