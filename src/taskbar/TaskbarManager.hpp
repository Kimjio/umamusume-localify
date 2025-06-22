#pragma once

#include <Windows.h>
#include <ShlObj_core.h>

namespace TaskbarManager
{
	inline ITaskbarList3* taskbarList = nullptr;

	inline HWND taskbarWnd = nullptr;

	inline void Initialze(HWND hWnd)
	{
		taskbarWnd = hWnd;
		CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, reinterpret_cast<LPVOID*>(&taskbarList));

		if (taskbarList)
		{
			taskbarList->SetProgressState(taskbarWnd, TBPF_NOPROGRESS);
		}
	}

	inline void SetProgressValue(ULONGLONG completed, ULONGLONG total)
	{
		if (taskbarList)
		{
			taskbarList->SetProgressValue(taskbarWnd, completed, total);
		}
	}

	inline void SetProgressState(TBPFLAG state)
	{
		if (taskbarList)
		{
			taskbarList->SetProgressState(taskbarWnd, state);
		}
	}
}
