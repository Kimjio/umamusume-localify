#pragma once

#include <Windows.h>
#include <ShlObj_core.h>

#include <cstdint>

namespace TaskbarManager
{
	inline ITaskbarList3* taskbarList = nullptr;

	inline HWND taskbarWnd = nullptr;

	inline TBPFLAG currentState = TBPF_NOPROGRESS;

	inline uint64_t currentProgressValue = 0;

	inline void Initialze(HWND hWnd)
	{
		taskbarWnd = hWnd;
		CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, reinterpret_cast<LPVOID*>(&taskbarList));

		if (taskbarList)
		{
			taskbarList->SetProgressState(taskbarWnd, TBPF_NOPROGRESS);
		}
	}

	inline void SetProgressState(TBPFLAG state)
	{
		if (taskbarList)
		{
			if (currentState == state)
			{
				return; // Avoid unnecessary updates
			}

			currentState = state;
			taskbarList->SetProgressState(taskbarWnd, state);
		}
	}

	inline void SetProgressValue(ULONGLONG completed, ULONGLONG total)
	{
		if (taskbarList)
		{
			if (currentProgressValue == completed)
			{
				return; // Avoid unnecessary updates
			}

			currentProgressValue = completed;
			currentState = TBPF_NORMAL;
			SetProgressState(currentState);
			taskbarList->SetProgressValue(taskbarWnd, completed, total);
		}
	}
}
