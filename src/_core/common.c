#include "qstrip.h"

static PyMethodDef QStripMethods[] = {
    {"strip_markdown", py_strip_markdown, METH_VARARGS, "Strip a markdown string."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef qstripcoremodule = {
    PyModuleDef_HEAD_INIT,
    "_core",
    NULL,
    -1,
    QStripMethods
};

PyMODINIT_FUNC PyInit__core(void) {
    return PyModule_Create(&qstripcoremodule);
}