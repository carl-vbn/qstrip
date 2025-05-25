#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

// Special macros for tracking code block states
#define CODE_NOT 0 // Not in any code
#define CODE_START 1 // Right after the first backtick
#define CODE_MULTI_FIRSTLINE 2 // In first line of a multi-line code block
#define CODE_MULTI_IN 3 // Inside a multi-line code block
#define CODE_MULTI_END 4 // Right after the last backtick
#define CODE_INLINE 5 // Inside an inline code block

char *strip(const char* name, Py_ssize_t len) {
    char *outbuf = malloc(len + 1);
    if (!outbuf) return NULL;

    Py_ssize_t j = 0;
    int lnstart = TRUE;
    int bold = FALSE;
    int italic = FALSE;
    int code = CODE_NOT;
    int hasnext;
    char c;

    for (Py_ssize_t i = 0; i < len; i++) {
        c = name[i];
        hasnext = i + 1 < len;

        // Completely ignore carriage returns
        if (c == '\r') {
            continue;
        }

        // Skip leading spaces, heading markers, and line breaks
        if (lnstart) {
            if (c == ' ' || c == '#' || c == '\n' || c == '\r' || c == '\t' || c == '=' || c == '-') {
                continue;
            }

            lnstart = FALSE;
        } else if (c == '\n') {
            lnstart = TRUE;
        }

        // Handle italic and bold markers
        if (c == '*') {
            if (hasnext && name[i + 1] == '*') {
                bold = !bold;
                i++;
                continue;
            } else {
                italic = !italic;
                continue;
            }
        }

        // Handle code markers

        if (code == CODE_MULTI_FIRSTLINE) {
            if (c == '\n') {
                code = CODE_MULTI_IN;
            }
            continue;
        }

        if (code == CODE_MULTI_END) {
            if (c == '\n') {
                code = CODE_NOT;
            }
            continue;
        }

        if (c == '`') {
            if (code == CODE_NOT) {
                code = CODE_START;
            } else if (code == CODE_START) {
                code = CODE_MULTI_FIRSTLINE;
            } else if (code == CODE_INLINE) {
                code = CODE_NOT;
            } else if (code == CODE_MULTI_IN) {
                code = CODE_MULTI_END;
            }
            
            continue;
        } else if (code == CODE_START) {
            code = CODE_INLINE;
        }

        outbuf[j++] = c;
    }
    outbuf[j] = '\0';

    return outbuf;
}

static PyObject* py_strip_markdown(PyObject* self, PyObject* args) {
    const char* input;
    Py_ssize_t len;

    if (!PyArg_ParseTuple(args, "s#", &input, &len))
        return NULL;

    char* result = strip(input, len);
    if (!result)
        return PyErr_NoMemory();

    PyObject* output = Py_BuildValue("s", result);
    free(result);
    return output;
}

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