#pragma once
#include "DialogInnerBase.hpp"

namespace Gallop
{
	class DialogLiveStartConfirm : public DialogInnerBase
	{
	public:
		void SetupToggle();
		void OnClickOK();

		static void PushDialog(int musicId, bool isSendApi, Il2CppDelegate* onDecide);

		using DialogInnerBase::DialogInnerBase;

	private:
		void Setup(int musicId, bool isSendApi);
		void SendApi(Il2CppDelegate* finishCallback);
	};
}
