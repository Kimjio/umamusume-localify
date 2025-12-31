#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "PaymentUtility.hpp"

namespace
{
	void* PaymentUtility_OpenBuyJewelDialog_1_addr = nullptr;

	void* PaymentUtility_OpenBuyJewelDialog_addr = nullptr;
	void* PaymentUtility_OpenBuyJewelDialog_orig = nullptr;

	void* PaymentUtility_SendRequestPaymentItemList_addr = nullptr;

	FieldInfo* PaymentUtility__finishBuyJewelCallbackForUserField;

	FieldInfo* PaymentUtility__onCloseShopDialogField;

	FieldInfo* PaymentUtility__checkOpenShopDialogField;

	FieldInfo* PaymentUtility__autoScrollIdListField;

	FieldInfo* PaymentUtility__removeAllOnFinishField;
}

static void PaymentUtility_OpenBuyJewelDialog_hook(Il2CppObject* self, Il2CppDelegate* finish, bool removeAllOnFinish, Il2CppDelegate* onClose, Il2CppDelegate* checkOpenShopDialog, Il2CppObject* evacuationCanvas, Il2CppArraySize_t<int>* autoScrollIdList)
{
	static Il2CppObject* _evacuationCanvas;
	_evacuationCanvas = evacuationCanvas;

	il2cpp_field_set_value(self, PaymentUtility__finishBuyJewelCallbackForUserField, finish);
	il2cpp_field_set_value(self, PaymentUtility__onCloseShopDialogField, onClose);
	il2cpp_field_set_value(self, PaymentUtility__checkOpenShopDialogField, checkOpenShopDialog);
	il2cpp_field_set_value(self, PaymentUtility__autoScrollIdListField, autoScrollIdList);
	il2cpp_field_set_value(self, PaymentUtility__removeAllOnFinishField, &removeAllOnFinish);

	auto OnComplete = CreateDelegateWithClassStatic(il2cpp_symbols::get_class("mscorlib.dll", "System", "Action"), *[]()
		{
			auto paymentUtility = Gallop::PaymentUtility::Instance();
			Gallop::PaymentUtility::Instance().OpenBuyJewelDialog(_evacuationCanvas);
		}
	);
	Gallop::PaymentUtility(self).SendRequestPaymentItemList(&OnComplete->delegate, false, nullptr, _evacuationCanvas);
}

static void InitAddress()
{
	PaymentUtility_OpenBuyJewelDialog_1_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "PaymentUtility", "OpenBuyJewelDialog", 1);
	PaymentUtility_OpenBuyJewelDialog_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "PaymentUtility", "OpenBuyJewelDialog", 6);
	PaymentUtility_SendRequestPaymentItemList_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "PaymentUtility", "SendRequestPaymentItemList", 4);
	PaymentUtility__finishBuyJewelCallbackForUserField = il2cpp_class_get_field_from_name(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "PaymentUtility"), "_finishBuyJewelCallbackForUser");
	PaymentUtility__onCloseShopDialogField = il2cpp_class_get_field_from_name(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "PaymentUtility"), "_onCloseShopDialog");
	PaymentUtility__checkOpenShopDialogField = il2cpp_class_get_field_from_name(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "PaymentUtility"), "_checkOpenShopDialog");
	PaymentUtility__autoScrollIdListField = il2cpp_class_get_field_from_name(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "PaymentUtility"), "_autoScrollIdList");
	PaymentUtility__removeAllOnFinishField = il2cpp_class_get_field_from_name(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "PaymentUtility"), "_removeAllOnFinish");
}

static void HookMethods()
{
	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		ADD_HOOK(PaymentUtility_OpenBuyJewelDialog, "Gallop.PaymentUtility::OpenBuyJewelDialog at %p\n");
	}
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
	const char* PaymentUtility::namespaze = "Gallop";
	const char* PaymentUtility::klassName = "PaymentUtility";

	void PaymentUtility::OpenBuyJewelDialog(Il2CppObject* evacuationCanvas)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppObject*)>(PaymentUtility_OpenBuyJewelDialog_1_addr)(instance, evacuationCanvas);
	}

	void PaymentUtility::OpenBuyJewelDialog(Il2CppDelegate* finish, bool removeAllOnFinish, Il2CppDelegate* onClose, Il2CppDelegate* checkOpenShopDialog, Il2CppObject* evacuationCanvas, Il2CppArraySize_t<int>* autoScrollIdList)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppDelegate*, bool, Il2CppDelegate*, Il2CppDelegate*, Il2CppObject*, Il2CppArraySize_t<int>*)>(PaymentUtility_OpenBuyJewelDialog_addr)(instance, finish, removeAllOnFinish, onClose, checkOpenShopDialog, evacuationCanvas, autoScrollIdList);
	}

	void PaymentUtility::SendRequestPaymentItemList(Il2CppDelegate* onComplete, bool removeAllOnFinish, Il2CppDelegate* onCancel, Il2CppObject* evacuationCanvas)
	{
		reinterpret_cast<void (*)(Il2CppObject*, Il2CppDelegate*, bool, Il2CppDelegate*, Il2CppObject*)>(PaymentUtility_SendRequestPaymentItemList_addr)(instance, onComplete, removeAllOnFinish, onCancel, evacuationCanvas);
	}
}