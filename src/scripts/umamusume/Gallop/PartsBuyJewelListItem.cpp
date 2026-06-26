#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "PartsBuyJewelListItem.hpp"

namespace
{
	Il2CppMethodPointer PartsBuyJewelListItem_Setup_addr = nullptr;
	void* PartsBuyJewelListItem_Setup_orig = nullptr;
}

static void PartsBuyJewelListItem_Setup_hook(Il2CppObject* self, Il2CppObject* purchaseItem, Il2CppDelegate* onComplete, Il2CppObject* evacuationCanvas)
{
	reinterpret_cast<decltype(PartsBuyJewelListItem_Setup_hook)*>(PartsBuyJewelListItem_Setup_orig)(self, purchaseItem, onComplete, evacuationCanvas);

	auto _purchaseButtonField = il2cpp_class_get_field_from_name(self->klass, "_purchaseButton");
	Il2CppObject* _purchaseButton;
	il2cpp_field_get_value(self, _purchaseButtonField, &_purchaseButton);

	il2cpp_symbols::get_method_pointer<void (*)(Il2CppObject*, bool)>(_purchaseButton->klass, "SetButtonInteractableWithColor", 1)(_purchaseButton, false);
	UnityEngine::MonoBehaviour(_purchaseButton).gameObject().SetActive(false);
}

static void InitAddress()
{
	PartsBuyJewelListItem_Setup_addr = il2cpp_symbols::get_method_pointer(ASSEMBLY_NAME, "Gallop", "PartsBuyJewelListItem", "Setup", 3);
}

static void HookMethods()
{
#ifdef _MSC_VER
	if (Game::CurrentGameRegion == Game::Region::KOR)
	{
		ADD_HOOK(PartsBuyJewelListItem_Setup, "Gallop.PartsBuyJewelListItem::Setup at %p\n");
	}
#endif
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}
