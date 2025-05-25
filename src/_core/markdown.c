#include "qstrip.h"

#include <stdio.h>
#include <stdlib.h>

// Special macros for tracking code block states
#define CODE_NOT 0 // Not in any code
#define CODE_START 1 // Right after the first backtick
#define CODE_MULTI_FIRSTLINE 2 // In first line of a multi-line code block
#define CODE_MULTI_IN 3 // Inside a multi-line code block
#define CODE_MULTI_END 4 // Right after the last backtick
#define CODE_INLINE 5 // Inside an inline code block

// Special macros for tracking links and images
#define LINK_NOT 0 // Not in a link/image
#define LINK_TEXT 1 // In the text part of a link/image
#define LINK_URL 2 // In the URL part of a link/image

// Utility function to look ahead in the string to check if a url in parenthesis is coming
int link_lookahead(const char* text, Py_ssize_t len, Py_ssize_t pos) {
    // Advance until the closing ']'
    while (pos + 1 < len && text[pos] != '\n') {
        if (text[pos] == ']') {
            // Check if the next character is an opening parenthesis
            if (pos + 1 < len && text[pos + 1] == '(') {
                return TRUE; // Found a link
            }
            return FALSE; // Not a link
        }

        pos++;
    }

    return FALSE;
}

char *strip(const char* text, Py_ssize_t len) {
    char *outbuf = malloc(len + 1);
    if (!outbuf) return NULL;

    Py_ssize_t j = 0;
    int lnstart = TRUE;
    int bold = FALSE;
    int italic = FALSE;
    int strikethrough = FALSE;
    int link = LINK_NOT;
    int code = CODE_NOT;
    int hasnext;
    char c;

    for (Py_ssize_t i = 0; i < len; i++) {
        c = text[i];
        hasnext = i + 1 < len;

        // Completely ignore carriage returns
        if (c == '\r') {
            continue;
        }

        if (code != CODE_NOT) {
            goto code;
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
            if (hasnext && text[i + 1] == '*') {
                bold = !bold;
                i++;
                continue;
            } else {
                italic = !italic;
                continue;
            }
        }

        // Handle strikethrough markers
        if (c == '~') {
            if (hasnext && text[i + 1] == '~') {
                strikethrough = !strikethrough;
                i++;
                continue;
            }
        }

        // Handle links and images
        if (link == LINK_NOT) {
            if (c == '!' && hasnext && text[i + 1] == '[' && link_lookahead(text, len, i + 2)) {
                // Entering image
                link = LINK_TEXT;
                i++;
                continue;
            }

            if (c == '[' && link_lookahead(text, len, i + 1)) {
                // Entering link
                link = LINK_TEXT;
                continue;
            }
        } else {
            if (link == LINK_TEXT && c == ']') {
                link = LINK_URL;
                continue;
            }
            
            if (link == LINK_URL) {
                if (c == ')') {
                    link = LINK_NOT;
                }
                continue;
            }
        }

        

code:
        // Handle code
        if (code == CODE_MULTI_FIRSTLINE) {
            if (c == '\n') {
                code = CODE_MULTI_IN;
            }
            continue;
        }

        if (code == CODE_MULTI_END) {
            if (c == '\n') {
                code = CODE_NOT;
                lnstart = TRUE;
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

PyObject* py_strip_markdown(PyObject* self, PyObject* args) {
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