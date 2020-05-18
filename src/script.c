#include "script.h"

#ifdef PY_

char *text_preprocessing(const char *const path, size_t max_features, uint mode) {
    PyObject *processed = script_processing(path, max_features, mode);
    if (processed == NULL) {
        fprintf(stderr, "Text processing error\n");
        return NULL;
    }

    const char *unicode = PyUnicode_AsUTF8(processed);

    char *buffer = malloc(strlen(unicode) * sizeof(char) + 1);
    strcpy(buffer, unicode);

    // Py_DECREF(processed);
    return buffer;
}

PyObject *script_processing(const char *const path, size_t max_features, uint mode) {
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue = NULL;

    setenv("PYTHONPATH", "./script/", 1);

    Py_Initialize();
    pName = PyUnicode_DecodeFSDefault("preprocessing");

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, "process");

        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(3);
            PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(path));
            PyTuple_SetItem(pArgs, 1, PyLong_FromSize_t(max_features));
            PyTuple_SetItem(pArgs, 2, PyLong_FromLong(mode));
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);

            if (pValue != NULL) {
                // Py_DECREF(pValue);
            } else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr, "Call failed\n");
                return NULL;
            }
        } else {
            if (PyErr_Occurred()) PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", "argv[2]");
        }

        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", "argv[1]");
        return NULL;
    }

    if (Py_FinalizeEx() < 0) {
        return NULL;
    }

    return pValue;
}

#endif