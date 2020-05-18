#ifdef PY_

#define PY_SSIZE_T_CLEAN
#include <python3.8/Python.h>

char *text_preprocessing(const char *const path, size_t max_features, uint mode);
PyObject *script_processing(const char *const path, size_t max_features, uint mode);

#endif