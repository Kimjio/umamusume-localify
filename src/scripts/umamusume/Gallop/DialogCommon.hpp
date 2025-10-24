#pragma once
#include "il2cpp/il2cpp_symbols.hpp"
#include "DialogCommonBase.hpp"
#include "DialogCommonBaseData.hpp"

namespace Gallop
{
	class DialogCommon : public DialogCommonBase
	{
	public:
		enum class DialogType
		{
			Game,
			System,
			LandscapeModeMenu = 1000,
			LandscapeModeRight,
		};

		enum class DialogObjType
		{
			Small,
			Middle,
			Big,
			None,
			LandscapeModeBig,
			LandscapeModeBigButton
		};

		enum class DispStackType
		{
			None,
			Simple,
			DialogOnDialog,
			LandscapeModeMenu = 1000,
			LandscapeModeRight,
		};

		void Close();
		void SetContentsRootLocalScale(float scale);

		using DialogCommonBase::DialogCommonBase;

		class Data : public DialogCommonBaseData
		{
		public:
			enum class ObjectParentType
			{
				Root,
				Base
			};

			Data();

			Data SetSimpleNoButtonMessage(Il2CppString* headerTextArg, Il2CppString* message);

			Data SetSimpleOneButtonMessage(Il2CppString* headerTextArg, Il2CppString* message, Il2CppDelegate* onClickCenterButton = nullptr, uint64_t closeTextId = GetTextIdByName(L"Common0007"), DialogCommonBase::FormType dialogFormType = DialogCommonBase::FormType::SMALL_ONE_BUTTON);

			Data SetSimpleOneButtonMessage(uint64_t headerTextId, Il2CppString* message, Il2CppDelegate* onClose = nullptr, uint64_t closeTextId = GetTextIdByName(L"Common0007"));

			Data SetSimpleTwoButtonMessage(Il2CppString* headerTextArg, Il2CppString* message, Il2CppDelegate* onRight = nullptr, uint64_t leftTextId = GetTextIdByName(L"Common0002"), uint64_t rightTextId = GetTextIdByName(L"Common0001"), Il2CppDelegate* onLeft = nullptr, DialogCommonBase::FormType dialogFormType = DialogCommonBase::FormType::SMALL_TWO_BUTTON);

			Data SetSimpleThreeButtonMessage(Il2CppString* headerTextArg, Il2CppString* message, Il2CppDelegate* onRight = nullptr, uint64_t rightTextId = GetTextIdByName(L"Common0001"), Il2CppDelegate* onCenter = nullptr, uint64_t centerTextId = GetTextIdByName(L"Common0002"), Il2CppDelegate* onLeft = nullptr, uint64_t leftTextId = GetTextIdByName(L"Common0002"), DialogCommonBase::FormType dialogFormType = DialogCommonBase::FormType::SMALL_THREE_BUTTON);

			void AddOpenCallback(Il2CppDelegate* callback);

			void AddCenterButtonCallback(Il2CppDelegate* callback);

			void AddLeftButtonCallback(Il2CppDelegate* callback);

			void AddRightButtonCallback(Il2CppDelegate* callback);

			void AddBeginCloseCallback(Il2CppDelegate* callback);

			void AddDestroyCallback(Il2CppDelegate* callback);

			void ReleaseDialogResourceAndHash();

			uint64_t DialogHash();

			void DialogHash(uint64_t value);

			int HeaderSortOrderOffset();

			void HeaderSortOrderOffset(int value);

			Il2CppString* LeftButtonText();

			void LeftButtonText(Il2CppString* value);

			Il2CppString* RightButtonText();

			void RightButtonText(Il2CppString* value);

			Il2CppString* RightButtonNoInteractableNotiffication();

			void RightButtonNoInteractableNotiffication(Il2CppString* value);

			Il2CppString* CenterButtonText();

			void CenterButtonText(Il2CppString* value);

			Il2CppDelegate* LeftButtonCallBack();

			void LeftButtonCallBack(Il2CppDelegate* value);

			Il2CppDelegate* RightButtonCallBack();

			void RightButtonCallBack(Il2CppDelegate* value);

			Il2CppDelegate* CenterButtonCallBack();

			void CenterButtonCallBack(Il2CppDelegate* value);

			DialogCommon::DialogType DialogType();

			void DialogType(DialogCommon::DialogType value);

			ObjectParentType ObjParentType();

			void ObjParentType(ObjectParentType value);

			DialogCommon::DispStackType DispStackType();

			void DispStackType(DialogCommon::DispStackType value);

			Il2CppDelegate* DestroyCallBack();

			void DestroyCallBack(Il2CppDelegate* value);

			bool AutoClose();

			void AutoClose(bool value);

			using DialogCommonBaseData::DialogCommonBaseData;
		};
	};
}
