#include <Python.h>

#include <rc_usefulincludes.h>
#include <roboticscape.h>

static char module_docstring[] =
  "This module provides an interface for the MODE and PAUSE buttons.";

static PyObject *buttonError;

// button functions
static PyObject *button_pressed(PyObject *self, PyObject *args);
static PyObject *button_released(PyObject *self, PyObject *args);

static PyMethodDef module_methods[] = {
  {"pressed",
   (PyCFunction)button_pressed,
   METH_VARARGS,
   "wait for button to be pressed"}
  ,
  {"released",
   (PyCFunction)button_released,
   METH_VARARGS,
   "wait for button to be released"}
  ,
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef module = {
  PyModuleDef_HEAD_INIT,
  "_buttons",   /* name of module */
  module_docstring, /* module documentation, may be NULL */
  -1,       /* size of per-interpreter state of the module,
	       or -1 if the module keeps state in global variables. */
  module_methods
};

/* python functions */

PyMODINIT_FUNC PyInit__buttons(void)
{
  PyObject *m;
  
  /* create module */
  m = PyModule_Create(&module);
  if (m == NULL)
    return NULL;

  /* create exception */
  buttonError = PyErr_NewException("button.error", NULL, NULL);
  Py_INCREF(buttonError);
  PyModule_AddObject(m, "error", buttonError);

  /* initialize cape */
  if (rc_get_state() == UNINITIALIZED) {
    // printf("* * * button: WILL CALL INIT * * *\n");
    if(rc_initialize())
      return NULL;
  }
  
  return m;
}

static
PyObject *button_pressed(PyObject *self,
			 PyObject *args)
{

  /* parse arguments */
  int button;
  if (!PyArg_ParseTuple(args, "I", &button)) {
    PyErr_SetString(buttonError, "Invalid arguments");
    return NULL;
  }

  int pressed = 0;
  if (button == 0) {
    /* pause button */

    // aquires mutex
    pthread_mutex_lock( &rc_buttons_pause_pressed_mutex );

    // waits until a measurement is available
    pthread_cond_wait( &rc_buttons_pause_pressed_condition,
		       &rc_buttons_pause_pressed_mutex );

    // releases mutex
    pthread_mutex_unlock( &rc_buttons_pause_pressed_mutex );
    
    if (rc_get_state() != EXITING)
      pressed = 1;
    
  } else if (button == 1) {
    /* mode button */

    // aquires mutex
    pthread_mutex_lock( &rc_buttons_mode_pressed_mutex );

    // waits until a measurement is available
    pthread_cond_wait( &rc_buttons_mode_pressed_condition,
		       &rc_buttons_mode_pressed_mutex );

    // releases mutex
    pthread_mutex_unlock( &rc_buttons_mode_pressed_mutex );

    if (rc_get_state() != EXITING)
      pressed = 1;

  } else {
    PyErr_SetString(buttonError, "Unknown button");
    return NULL;
  }
    
  /* Build the output tuple */
  PyObject *ret = Py_BuildValue("O", pressed ? Py_True : Py_False);

  return ret;
}

static
PyObject *button_released(PyObject *self,
			  PyObject *args)
{
  
  /* parse arguments */
  int button;
  if (!PyArg_ParseTuple(args, "I", &button)) {
    PyErr_SetString(buttonError, "Invalid arguments");
    return NULL;
  }

  int released = 0;
  if (button == 0) {
    /* pause button */

    // aquires mutex
    pthread_mutex_lock( &rc_buttons_pause_released_mutex );

    // waits until a measurement is available
    pthread_cond_wait( &rc_buttons_pause_released_condition,
		       &rc_buttons_pause_released_mutex );

    // releases mutex
    pthread_mutex_unlock( &rc_buttons_pause_released_mutex );
    
    if (rc_get_state() != EXITING)
      released = 1;
    
  } else if (button == 1) {
    /* mode button */

    // aquires mutex
    pthread_mutex_lock( &rc_buttons_mode_released_mutex );

    // waits until a measurement is available
    pthread_cond_wait( &rc_buttons_mode_released_condition,
		       &rc_buttons_mode_released_mutex );

    // releases mutex
    pthread_mutex_unlock( &rc_buttons_mode_released_mutex );

    if (rc_get_state() != EXITING)
      released = 1;
    
  } else {
    PyErr_SetString(buttonError, "Unknown button");
    return NULL;
  }
    
  /* Build the output tuple */
  PyObject *ret = Py_BuildValue("O", released ? Py_True : Py_False);

  return ret;
}
