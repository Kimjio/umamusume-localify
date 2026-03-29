#include "../umamusume.hpp"
#include "../../ScriptInternal.hpp"
#include "PartsEpisodeList.hpp"

#include "DialogManager.hpp"
#include "Localize.hpp"

#include "WebViewManager.hpp"

#include "scripts/mscorlib/System/ValueTuple.hpp"

namespace
{
	Il2CppClass* PartsEpisodeList_klass = nullptr;

	void* PartsEpisodeList_SetupStoryExtraEpisodeList_4_addr = nullptr;
	void* PartsEpisodeList_SetupStoryExtraEpisodeList_4_orig = nullptr;

	void* PartsEpisodeList_SetupStoryExtraEpisodeList_addr = nullptr;
	void* PartsEpisodeList_SetupStoryExtraEpisodeList_orig = nullptr;

	FieldInfo* PartsEpisodeList__voiceButton = nullptr;
	FieldInfo* PartsEpisodeList__playVoiceButton = nullptr;
}

static void UpdateVoiceButtonKakao(Il2CppObject* partData, Il2CppObject* voiceButton, Il2CppObject* button, int storyId)
{
	if (button)
	{
		auto callback = GetButtonCommonOnClickDelegate(button);
		if (callback)
		{
			auto newFn = *[](Il2CppObject* self)
				{
					auto tuple = *il2cpp_object_unbox_type<System::ValueTuple3<Il2CppObject*, Il2CppObject*, int>*>(self);

					auto voiceButton = tuple.Item2;
					auto storyId = tuple.Item3;

					reinterpret_cast<void (*)(Il2CppObject*)>(il2cpp_class_get_method_from_name(voiceButton->klass, "StopVoiceIfNeed", 0)->methodPointer)(voiceButton);

					auto onLeft = CreateDelegate(self, *[](Il2CppObject* self)
						{
							auto tuple = *il2cpp_object_unbox_type<System::ValueTuple3<Il2CppObject*, Il2CppObject*, int>*>(self);

							auto storyId = tuple.Item3;

							auto masterDataManager = GetSingletonInstance(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "MasterDataManager"));
							auto masterBannerData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(masterDataManager->klass, "get_masterBannerData", 0)->methodPointer(masterDataManager);

							auto bannerList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, int)>
								(masterBannerData->klass, "GetListWithGroupId", 1)->methodPointer(masterBannerData, 7);

							FieldInfo* itemsField = il2cpp_class_get_field_from_name(bannerList->klass, "_items");
							Il2CppArraySize_t<Il2CppObject*>* arr;
							il2cpp_field_get_value(bannerList, itemsField, &arr);

							int announceId = -1;

							for (int i = 0; i < arr->max_length; i++)
							{
								auto item = arr->vector[i];
								if (item)
								{
									auto typeField = il2cpp_class_get_field_from_name(item->klass, "Type");
									int type;
									il2cpp_field_get_value(item, typeField, &type);
									auto conditionValueField = il2cpp_class_get_field_from_name(item->klass, "ConditionValue");
									int conditionValue;
									il2cpp_field_get_value(item, conditionValueField, &conditionValue);
									if (type == 7 && conditionValue == storyId)
									{
										auto transitionField = il2cpp_class_get_field_from_name(item->klass, "Transition");
										il2cpp_field_get_value(item, transitionField, &announceId);
										break;
									}
								}
							}

							if (announceId == -1 && storyId < 1005)
							{
								announceId = storyId - 1002;
							}

							auto action = CreateDelegateStatic(*[]() {});

							il2cpp_symbols::get_method_pointer<void (*)(int, Il2CppDelegate*, Il2CppDelegate*)>(ASSEMBLY_NAME, "Gallop", "DialogAnnounceEvent", "Open", 3)(announceId, action, action);
						});

					if (storyId < 1005)
					{
						auto onRight = CreateDelegate(self, *[](Il2CppObject* self)
							{
								auto tuple = *il2cpp_object_unbox_type<System::ValueTuple3<Il2CppObject*, Il2CppObject*, int>*>(self);

								auto partData = tuple.Item1;
								auto voiceButton = tuple.Item2;
								auto storyId = tuple.Item3;

								auto announceDataByStoryEventId = il2cpp_symbols::get_method_pointer<Il2CppObject * (*)(int)>(ASSEMBLY_NAME, "Gallop", "PartsEpisodeList", "GetAnnounceDataByStoryEventId", 1)(storyId);

								if (!announceDataByStoryEventId)
								{
									return;
								}

								auto cueSheetNameField = il2cpp_class_get_field_from_name(announceDataByStoryEventId->klass, "CueSheetName");
								Il2CppString* cueSheetName;
								il2cpp_field_get_value(announceDataByStoryEventId, cueSheetNameField, &cueSheetName);

								auto cueNameField = il2cpp_class_get_field_from_name(announceDataByStoryEventId->klass, "CueName");
								Il2CppString* cueName;
								il2cpp_field_get_value(announceDataByStoryEventId, cueNameField, &cueName);

								string optionKey = string("kakaoUmaAnnounceEvent").append(to_string(storyId));

								auto KakaoManager = il2cpp_symbols::get_class(ASSEMBLY_NAME, "", "KakaoManager");
								auto managerInstanceField = il2cpp_class_get_field_from_name(KakaoManager, "instance");

								Il2CppObject* manager;
								il2cpp_field_static_get_value(managerInstanceField, &manager);

								auto url = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*, Il2CppString*)>(
									manager->klass, "GetKakaoOptionValue", 1)->methodPointer(manager, il2cpp_string_new(optionKey.data()));

								auto title = il2cpp_class_get_method_from_name_type<Il2CppString * (*)(Il2CppObject*)>(partData->klass, "get_Title", 0)->methodPointer(partData);
								auto data = Gallop::DialogCommon::Data();
								data.SetSimpleOneButtonMessage(title, nullptr,
									CreateDelegate(self, *[](Il2CppObject* self)
										{
											auto tuple = *il2cpp_object_unbox_type<System::ValueTuple3<Il2CppObject*, Il2CppObject*, int>*>(self);

											auto voiceButton = tuple.Item2;

											reinterpret_cast<void (*)(Il2CppObject*)>(il2cpp_class_get_method_from_name(voiceButton->klass, "StopVoiceIfNeed", 0)->methodPointer)(voiceButton);
										}),
									GetTextIdByName(IL2CPP_STRING("Common0007")), Gallop::DialogCommonBase::FormType::BIG_ONE_BUTTON);

								Gallop::WebViewManager::Instance().Open(url, data, nullptr, nullptr, false);

								il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppString*, Il2CppString*)>(voiceButton->klass, "PlayAnnounceVoice", 2)->methodPointer(voiceButton, cueSheetName, cueName);
							});

						auto dialogData = Gallop::DialogCommon::Data();
						dialogData.SetSimpleTwoButtonMessage(
							Gallop::Localize::Get(GetTextIdByName(IL2CPP_STRING("StoryEvent0079"))),
							il2cpp_string_new("해당 스토리 이벤트는 개최 정보가 누락되어있습니다.\n\n웹 페이지를 보시겠습니까?"),
							onRight,
							GetTextIdByName(IL2CPP_STRING("Common0002")),
							GetTextIdByName(IL2CPP_STRING("Common0003")),
							onLeft
						);

						Gallop::DialogManager::PushDialog(dialogData);
					}
					else
					{
						reinterpret_cast<void (*)(void*)>(onLeft->method_ptr)(onLeft->target);
					}
				};

			auto ValueTuple3Class = GetGenericClass(GetRuntimeType("mscorlib.dll", "System", "ValueTuple`3"), GetRuntimeType(il2cpp_defaults.object_class), GetRuntimeType(il2cpp_defaults.object_class), GetRuntimeType(il2cpp_defaults.object_class));
			System::ValueTuple3<Il2CppObject*, Il2CppObject*, int> tuple = { partData, voiceButton, storyId };
			auto boxed = il2cpp_value_box(ValueTuple3Class, &tuple);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(button->klass, "SetOnClick", 1)->methodPointer(button, &CreateUnityAction(boxed, newFn)->delegate);
		}
	}
}

static void UpdateVoiceButton(Il2CppObject* partsEpisodeList, Il2CppObject* partData)
{
	auto partDataIdField = il2cpp_class_get_field_from_name(partData->klass, "<Id>k__BackingField");
	int currentStoryId;
	il2cpp_field_get_value(partData, partDataIdField, &currentStoryId);

	auto voiceButtonField = il2cpp_class_get_field_from_name(partsEpisodeList->klass, "_voiceButton");
	Il2CppObject* voiceButton;
	il2cpp_field_get_value(partsEpisodeList, voiceButtonField, &voiceButton);

	auto buttonField = il2cpp_class_get_field_from_name(voiceButton->klass, "_playVoiceButton");
	Il2CppObject* button;
	il2cpp_field_get_value(voiceButton, buttonField, &button);

	if (button)
	{
		if (Game::CurrentGameRegion == Game::Region::KOR)
		{
			UpdateVoiceButtonKakao(partData, voiceButton, button, currentStoryId);
		}
		else
		{
			auto callback = GetButtonCommonOnClickDelegate(button);

			if (callback)
			{
				auto newFn = *(
					[](Il2CppObject* storyIdBox)
					{
						int currentStoryId = *il2cpp_object_unbox_type<int*>(storyIdBox);
						auto masterDataManager = GetSingletonInstance(il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "MasterDataManager"));
						auto masterBannerData = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(masterDataManager->klass, "get_masterBannerData", 0)->methodPointer(masterDataManager);

						auto bannerList = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*, int)>(masterBannerData->klass, "GetListWithGroupId", 1)->methodPointer(masterBannerData, 7);

						FieldInfo* itemsField = il2cpp_class_get_field_from_name(bannerList->klass, "_items");
						Il2CppArraySize_t<Il2CppObject*>* arr;
						il2cpp_field_get_value(bannerList, itemsField, &arr);

						int announceId = -1;

						for (int i = 0; i < arr->max_length; i++)
						{
							auto item = arr->vector[i];
							if (item)
							{
								auto typeField = il2cpp_class_get_field_from_name(item->klass, "Type");
								int type;
								il2cpp_field_get_value(item, typeField, &type);
								auto conditionValueField = il2cpp_class_get_field_from_name(item->klass, "ConditionValue");
								int conditionValue;
								il2cpp_field_get_value(item, conditionValueField, &conditionValue);
								if (type == 7 && conditionValue == currentStoryId)
								{
									auto transitionField = il2cpp_class_get_field_from_name(item->klass, "Transition");
									il2cpp_field_get_value(item, transitionField, &announceId);
									break;
								}
							}
						}

						if (announceId == -1 && currentStoryId < 1005)
						{
							announceId = currentStoryId - 1002;
						}

						auto action = CreateDelegateStatic(*[]() {});

						il2cpp_symbols::get_method_pointer<void (*)(int, Il2CppDelegate*, Il2CppDelegate*)>(ASSEMBLY_NAME, "Gallop", "DialogAnnounceEvent", "Open", 3)(announceId, action, action);
					});
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, Il2CppDelegate*)>(button->klass, "SetOnClick", 1)->methodPointer(button,
					&CreateUnityAction(il2cpp_value_box(il2cpp_defaults.int32_class, &currentStoryId), newFn)->delegate);
			}
		}
	}
}

static void PartsEpisodeList_SetupStoryExtraEpisodeList_4_hook(Il2CppObject* self, Il2CppObject* extraSubCategory, Il2CppObject* partDataList, Il2CppObject* partData, Il2CppDelegate* onClick)
{
	reinterpret_cast<decltype(PartsEpisodeList_SetupStoryExtraEpisodeList_4_hook)*>(PartsEpisodeList_SetupStoryExtraEpisodeList_4_orig)(self, extraSubCategory, partDataList, partData, onClick);
	UpdateVoiceButton(self, partData);
}

static void PartsEpisodeList_SetupStoryExtraEpisodeList_hook(Il2CppObject* self, Il2CppObject* extraSubCategory, Il2CppObject* partDataList, Il2CppObject* partData, Il2CppObject* topEpisodePartDataList, Il2CppDelegate* onClick)
{
	reinterpret_cast<decltype(PartsEpisodeList_SetupStoryExtraEpisodeList_hook)*>(PartsEpisodeList_SetupStoryExtraEpisodeList_orig)(self, extraSubCategory, partDataList, partData, topEpisodePartDataList, onClick);
	UpdateVoiceButton(self, partData);
}

static void InitAddress()
{
	PartsEpisodeList_klass = il2cpp_symbols::get_class(ASSEMBLY_NAME, "Gallop", "PartsEpisodeList");
	PartsEpisodeList__voiceButton = il2cpp_class_get_field_from_name(PartsEpisodeList_klass, "_voiceButton");
	PartsEpisodeList__playVoiceButton = il2cpp_class_get_field_from_name(PartsEpisodeList_klass, "_playVoiceButton");
	PartsEpisodeList_SetupStoryExtraEpisodeList_4_addr = il2cpp_symbols::get_method_pointer(PartsEpisodeList_klass, "SetupStoryExtraEpisodeList", 4);
	PartsEpisodeList_SetupStoryExtraEpisodeList_addr = il2cpp_symbols::get_method_pointer(PartsEpisodeList_klass, "SetupStoryExtraEpisodeList", 5);
}

static void HookMethods()
{
	ADD_HOOK(PartsEpisodeList_SetupStoryExtraEpisodeList_4, "Gallop.PartsEpisodeList::SetupStoryExtraEpisodeList_4 at %p\n");
	ADD_HOOK(PartsEpisodeList_SetupStoryExtraEpisodeList, "Gallop.PartsEpisodeList::SetupStoryExtraEpisodeList at %p\n");
}

STATIC
{
	il2cpp_symbols::init_callbacks.emplace_back(InitAddress);
	il2cpp_symbols::init_callbacks.emplace_back(HookMethods);
}

namespace Gallop
{
}
