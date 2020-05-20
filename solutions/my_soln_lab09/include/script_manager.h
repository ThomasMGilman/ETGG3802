#pragma once
#include <stdafx.h>
#include <singleton.h>

#define SCRIPT_MANAGER ssuge::ScriptManager::get_singleton_ptr()

namespace ssuge
{
	class ScriptManager : public Singleton<ScriptManager>
	{
	protected:
		/// Cache of all loaded classes (the string = the class name, the PyObject* = the class)
		std::map<std::string, PyObject*> mLoadedClasses;

	public:
		/// Default constructor
		ScriptManager();

		/// Destructor
		~ScriptManager();

		/// Utility function that extracts a c++ string from a PyObject.  This function will
		/// return an empty string if the given object is not a PyUnicode object.
		static std::string get_string_from_pyobject(PyObject* py_str);

		/// Utility function that attempts to extract a group of 3 floats from the given PyObject.
		/// returns True and sets the error string to that given if it can't do this extraction.  Returns
		/// true if everything went OK.
		static bool extract_vector3(PyObject* obj, Ogre::Vector3& vec, std::string error_message);

	protected:
		/// Investigates the given module, looking for classes.  If it finds one, it puts 
		/// (an incref'ed) 
		void look_for_classes(PyObject* loaded_module);
	public:
		/// Attempts to find the given named class (first in the map of cached classes; if it
		/// can't be found there, it looks in the current execution frame).  Returns the "vanilla"
		/// ssuge.GameObject class if it can't find any other class
		PyObject* get_class(std::string name);

		/// Can be called if an error happens in the python interpreter and shows a message on the
		/// visual log.  If no error has occurred, nothing is done.
		void handle_error();

		/// Executes the given script.  That script can be an executable-type script, or it can
		/// contain a class, or a mixture of both.  If the file contains a class, it will be stored
		/// away for later use (by the get_class method).
		void run_script(std::string fname);
		
	};
}
