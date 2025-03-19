#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "MonoSingleton.hpp"

namespace Gallop
{
	class PaymentUtility : public MonoSingleton<PaymentUtility>
	{
	public:
		void OpenBuyJewelDialog(Il2CppObject* evacuationCanvas);
		
		void OpenBuyJewelDialog(Il2CppDelegate* finish, bool removeAllOnFinish = true, Il2CppDelegate* onClose = nullptr, Il2CppDelegate* checkOpenShopDialog = nullptr, Il2CppObject* evacuationCanvas = nullptr, Il2CppArraySize_t<int>* autoScrollIdList = nullptr);
		
		void SendRequestPaymentItemList(Il2CppDelegate* onComplete, bool removeAllOnFinish, Il2CppDelegate* onCancel, Il2CppObject* evacuationCanvas);

		using MonoSingleton::MonoSingleton;

		static const char* namespaze;
		static const char* klassName;
	};
}
