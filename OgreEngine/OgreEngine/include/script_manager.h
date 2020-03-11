#pragma once

#ifndef _SCRIPT_MANAGER_H_
#define _SCRIPT_MANAGER_H_
#include <stdafx.h>
#include <singleton.h>

#define SCRIPT_MANAGER ScriptManager::getSingletonPtr()

namespace OgreEngine
{
	class ScriptManager : public Singleton<ScriptManager>
	{
	private:
	protected:
	public:

		ScriptManager();

		~ScriptManager();

		void run_script(std::string fileName);

		void handle_error();
	};
}

#endif