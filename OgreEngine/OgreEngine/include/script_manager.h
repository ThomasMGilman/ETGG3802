#pragma once

#ifndef _SCRIPT_MANAGER_H_
#define _SCRIPT_MANAGER_H_

#include <singleton.h>
#include <script_functions.h>

#define SCRIPT_MANAGER ScriptManager::getSingletonPtr()

namespace OgreEngine
{
	class ScriptManager : public Singleton<ScriptManager>
	{
	private:
	protected:
	public:

		ScriptManager();

		void handle_error();
	};
}

#endif