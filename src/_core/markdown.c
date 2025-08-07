#include "qstrip.h"

#include <stdio.h>
#include <stdlib.h>

// Code block state macros
#define CODE_NOT 0 // Not in any code
#define CODE_START 1 // Right after the first backtick
#define CODE_MULTI_FLN 2 // In first line of a multi-line code block
#define CODE_MULTI_IN  3 // Inside a multi-line code block
#define CODE_MULTI_END 4 // Right after the last backtick
#define CODE_INLN 5 // Inside an inline code block

// Link and image state macros
#define LINK_NOT 0 // Not in a link/image
#define LINK_TXT 1 // In the text part of a link/image
#define LINK_URL 2 // In the URL part of a link/image

// Table state macros
#define TABLE_UNK  0 // Might be in a table
#define TABLE_NOT  1 // Not in a table
#define TABLE_HEAD 2 // In a table header
#define TABLE_SEPR 3 // In a table separator
#define TABLE_BODY 4 // In a table body

// Mask bits
#define MASK_TABLE 1
#define MASK_LINK 2
#define MASK_IMAGE 4
#define MASK_CODE 8
#define MASK_ALL 15

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

// Similar utility to check if a table begins on this line
int table_lookahead(const char* text, Py_ssize_t len, Py_ssize_t pos) {
    // Check that the line has a pipe character
    int has_pipe = FALSE;
    while (pos < len && text[pos] != '\n') {
        if (text[pos] == '|') {
            has_pipe = TRUE;
            // Don't break to consume whole line
        }
        pos++;
    }

    if (!has_pipe || ++pos >= len) {
        return FALSE;
    }

    // Check that the next line is a separator
    // i.e. fail if any character is not a pipe, dash, or colon
    int has_dash = FALSE;
    while (pos < len && text[pos] != '\n') {
        char c = text[pos];
        if (c == '-') {
            has_dash = TRUE;
        } else if (c != '|' && c != '-' && c != ':') {
            return FALSE;
        }

        pos++;
    }

    return has_dash;
}

// Returns TRUE if the given string contains a pipe character before the
// next new line character, between text+pos (incl) and text+pos+len (excl).
int ln_has_pipes(const char* text, Py_ssize_t len, Py_ssize_t pos) {
    while (pos < len && text[pos] != '\n') {
        if (text[pos] == '|') {
            return TRUE;
        }
        pos++;
    }
    return FALSE;
}

char next_nonspace_char(const char* text, Py_ssize_t len, Py_ssize_t pos) {
    // Skip spaces and tabs
    while (pos < len && (text[pos] == ' ' || text[pos] == '\t')) {
        pos++;
    }

    // Return the next non-space character or '\0' if at the end
    return (pos < len) ? text[pos] : '\0';
}

char *strip(const char* text, int mask, Py_ssize_t len) {
    char *outbuf = malloc(len + 1);
    if (!outbuf) return NULL;

    Py_ssize_t j = 0;
    int lnstart = TRUE;
    int bold = FALSE;
    int italic = FALSE;
    int strikethrough = FALSE;
    int table = TABLE_UNK;
    int link = LINK_NOT;
    int image = LINK_NOT;
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

        if ((mask & MASK_CODE) && code != CODE_NOT) {
            goto code;
        }

        if (lnstart) {
            if (table == TABLE_HEAD) {
                table = TABLE_SEPR;
            } else if (table == TABLE_SEPR) {
                table = TABLE_BODY;
            } else if (table == TABLE_BODY) {
                if (!ln_has_pipes(text, len, i)) {
                    // If we are in a table but this line has no pipes,
                    // we have reached the end of the table
                    table = TABLE_NOT;
                }
            } else {
                // Skip leading spaces, heading markers, and blank lines
                if (c == ' ' || c == '#' || c == '\n' || c == '\r' || c == '\t' || c == '=' || c == '-') {
                    continue;
                }
            }


            lnstart = FALSE;
        } else if (c == '\n') {
            lnstart = TRUE;
        }

        // Handle table state
        if ((mask & MASK_TABLE) && table == TABLE_UNK && c == '|') {
            if (table_lookahead(text, len, i)) {
                table = TABLE_HEAD;
            } else {
                table = TABLE_NOT;
            }
        }

        if ((mask & MASK_TABLE) && (table == TABLE_HEAD || table == TABLE_BODY)) {
            if (c == '|') {
                // Emit a comma (if not at line start/end) and skip
                if (i > 0 && text[i - 1] != '\n' && hasnext && text[i + 1] != '\n') {
                    outbuf[j++] = ',';
                }

                // Collapse spaces after the pipe
                while (hasnext && text[i + 1] == ' ') {
                    i++;
                    hasnext = i + 1 < len;
                }

                lnstart = FALSE;
                continue;
            } else if (c == ' ') {
                // Check if there are only spaces before the next pipe or line end
                char next_char = next_nonspace_char(text, len, i);
                if (next_char == '\0' || next_char == '\n' || next_char == '|') {
                    // If so, skip the space
                    continue;
                }
            }
        }

        if ((mask & MASK_TABLE) && table == TABLE_SEPR) {
            // If we are in a table separator, skip all characters
            // until the next newline
            
            if (c == '\n') {
                table = TABLE_BODY;
            }

            continue;
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

        // Handle images
        if (mask & MASK_IMAGE) {
            if (image == LINK_NOT) {
                if (c == '!' && hasnext && text[i + 1] == '[' && link_lookahead(text, len, i + 2)) {
                    // Entering image
                    image = LINK_TXT;
                    i++;
                    continue;
                }
            } else {
                if (image == LINK_TXT && c == ']') {
                    image = LINK_URL;
                    continue;
                }
                
                if (image == LINK_URL) {
                    if (c == ')') {
                        image = LINK_NOT;
                    }
                    continue;
                }
            }
        }

        // Handle links
        if (mask & MASK_LINK) {
            if (link == LINK_NOT) {
                // Avoid treating image alt-text as a link when images are not being stripped
                if (c == '[' && !(i > 0 && text[i - 1] == '!') && link_lookahead(text, len, i + 1)) {
                    // Entering link
                    link = LINK_TXT;
                    continue;
                }
            } else {
                if (link == LINK_TXT && c == ']') {
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
        }

code:
        // Handle code
        if (mask & MASK_CODE) {
            if (code == CODE_MULTI_FLN) {
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
                    code = CODE_MULTI_FLN;
                } else if (code == CODE_INLN) {
                    code = CODE_NOT;
                } else if (code == CODE_MULTI_IN) {
                    code = CODE_MULTI_END;
                }
                
                continue;
            } else if (code == CODE_START) {
                code = CODE_INLN;
            }
        }

        outbuf[j++] = c;
    }
    outbuf[j] = '\0';

    return outbuf;
}

PyObject* py_strip_markdown(PyObject* self, PyObject* args) {
    const char* input;
    Py_ssize_t len;
    int mask = MASK_ALL;

    if (!PyArg_ParseTuple(args, "s#|i", &input, &len, &mask))
        return NULL;

    char* result = strip(input, mask, len);
    if (!result)
        return PyErr_NoMemory();

    PyObject* output = Py_BuildValue("s", result);
    free(result);
    return output;
}