#include "LiveUtils.hpp"

#include "scripts/ScriptInternal.hpp"

#include "scripts/CriMw.CriWare.Runtime/CriWare/CriAtomExPlayback.hpp"
#include "scripts/Cute.Cri.Assembly/Cute/Cri/AudioPlayback.hpp"

static bool IsMovingLivePlayback = false;

namespace Localify
{
	void LiveUtils::MoveLivePlayback(float value)
	{
		if (IsMovingLivePlayback)
		{
			return;
		}

		IsMovingLivePlayback = true;

		auto director = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop.Live", "Director"));
		if (director)
		{
			bool isPauseLive = il2cpp_class_get_method_from_name_type<bool (*)()>(director->klass, "IsPauseLive", 0)->methodPointer();

			auto _liveCurrentTimeField = il2cpp_class_get_field_from_name(director->klass, "_liveCurrentTime");
			il2cpp_field_set_value(director, _liveCurrentTimeField, &value);

			auto LiveTimeController = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(director->klass, "get_LiveTimeController", 0)->methodPointer(director);

			if (!isPauseLive)
			{
				// prevent voice desync
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(LiveTimeController->klass, "PauseLive", 0)->methodPointer(LiveTimeController);
			}

			auto _elapsedTimeField = il2cpp_class_get_field_from_name(LiveTimeController->klass, "_elapsedTime");
			il2cpp_field_set_value(LiveTimeController, _elapsedTimeField, &value);

			il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, float)>(LiveTimeController->klass, "set_CurrentTime", 1)->methodPointer(LiveTimeController, value);

			auto AudioManager = GetSingletonInstance(il2cpp_symbols::get_class("umamusume.dll", "Gallop", "AudioManager"));
			auto CriAudioManager = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(AudioManager->klass, "get_CriAudioManager", 0)->methodPointer(AudioManager);

			auto audioCtrlDictField = il2cpp_class_get_field_from_name(CriAudioManager->klass, "audioCtrlDict");
			Il2CppObject* audioCtrlDict;
			il2cpp_field_get_value(CriAudioManager, audioCtrlDictField, &audioCtrlDict);

			auto _songPlaybackField = il2cpp_class_get_field_from_name(AudioManager->klass, "_songPlayback");
			Cute::Cri::AudioPlayback _songPlayback;
			il2cpp_field_get_value(AudioManager, _songPlaybackField, &_songPlayback);

			auto _songCharaPlaybacksField = il2cpp_class_get_field_from_name(AudioManager->klass, "_songCharaPlaybacks");
			Il2CppArraySize_t<Cute::Cri::AudioPlayback>* _songCharaPlaybacks;
			il2cpp_field_get_value(AudioManager, _songCharaPlaybacksField, &_songCharaPlaybacks);

			void** params = new void* [1];
			params[0] = &_songPlayback.soundGroup;

			Il2CppException* exception;

			auto audioCtrl = il2cpp_runtime_invoke(il2cpp_class_get_method_from_name(audioCtrlDict->klass, "get_Item", 1), audioCtrlDict, params, &exception);

			delete[] params;

			if (exception)
			{
				return;
			}

			auto poolField = il2cpp_class_get_field_from_name(audioCtrl->klass, "pool");
			Il2CppObject* pool;
			il2cpp_field_get_value(audioCtrl, poolField, &pool);

			if (pool)
			{
				FieldInfo* sourceListField = il2cpp_class_get_field_from_name(pool->klass, "sourceList");
				Il2CppObject* sourceList;
				il2cpp_field_get_value(pool, sourceListField, &sourceList);

				FieldInfo* itemsField = il2cpp_class_get_field_from_name(sourceList->klass, "_items");
				Il2CppArraySize_t<Il2CppObject*>* sources;
				il2cpp_field_get_value(sourceList, itemsField, &sources);

				Il2CppObject* cuteAudioSource = nullptr;

				for (int i = 0; i < sources->max_length; i++)
				{
					auto obj = sources->vector[i];

					if (obj)
					{
						auto isSame = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*, Cute::Cri::AudioPlayback)>(obj->klass, "IsSamePlaybackId", 1)->methodPointer(obj, _songPlayback);
						if (isSame)
						{
							cuteAudioSource = obj;
							break;
						}
					}
				}

				if (cuteAudioSource)
				{
					auto sourceListField = il2cpp_class_get_field_from_name(cuteAudioSource->klass, "sourceList");
					Il2CppObject* sourceList;
					il2cpp_field_get_value(cuteAudioSource, sourceListField, &sourceList);

					auto usingIndexField = il2cpp_class_get_field_from_name(cuteAudioSource->klass, "usingIndex");
					int usingIndex;
					il2cpp_field_get_value(cuteAudioSource, usingIndexField, &usingIndex);

					params = new void* [1];
					params[0] = &usingIndex;

					auto AtomSource = il2cpp_runtime_invoke(il2cpp_class_get_method_from_name(sourceList->klass, "get_Item", 1), sourceList, params, &exception);

					delete[] params;

					if (exception)
					{
						return;
					}

					auto player = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(AtomSource->klass, "get_player", 0)->methodPointer(AtomSource);
					il2cpp_class_get_method_from_name_type<CriWare::CriAtomExPlayback(*)(Il2CppObject*)>(player->klass, "StopWithoutReleaseTime", 0)->methodPointer(player);
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, long)>(player->klass, "SetStartTime", 1)->methodPointer(player, static_cast<long>(roundf(value * 1000.0f)));
					auto playback = il2cpp_class_get_method_from_name_type<CriWare::CriAtomExPlayback(*)(Il2CppObject*)>(player->klass, "Start", 0)->methodPointer(player);
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, CriWare::CriAtomExPlayback)>(player->klass, "Update", 1)->methodPointer(player, playback);

					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(player->klass, "Pause", 0)->methodPointer(player);

					_songPlayback.criAtomExPlayback = playback;
					il2cpp_field_set_value(AudioManager, _songPlaybackField, &_songPlayback);
					il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, CriWare::CriAtomExPlayback)>(AtomSource->klass, "set_Playback", 1)->methodPointer(AtomSource, playback);
				}
			}


			if (_songCharaPlaybacks)
			{
				for (int i = 0; i < _songCharaPlaybacks->max_length; i++)
				{
					auto charaPlayback = _songCharaPlaybacks->vector[i];

					void** params = new void* [1];
					params[0] = &charaPlayback.soundGroup;

					Il2CppException* exception;

					auto audioCtrl = il2cpp_runtime_invoke(il2cpp_class_get_method_from_name(audioCtrlDict->klass, "get_Item", 1), audioCtrlDict, params, &exception);

					delete[] params;

					if (exception)
					{
						continue;
					}

					auto poolField = il2cpp_class_get_field_from_name(audioCtrl->klass, "pool");
					Il2CppObject* pool;
					il2cpp_field_get_value(audioCtrl, poolField, &pool);

					if (pool)
					{
						FieldInfo* sourceListField = il2cpp_class_get_field_from_name(pool->klass, "sourceList");
						Il2CppObject* sourceList;
						il2cpp_field_get_value(pool, sourceListField, &sourceList);

						FieldInfo* itemsField = il2cpp_class_get_field_from_name(sourceList->klass, "_items");
						Il2CppArraySize_t<Il2CppObject*>* sources;
						il2cpp_field_get_value(sourceList, itemsField, &sources);

						Il2CppObject* cuteAudioSource = nullptr;

						for (int i = 0; i < sources->max_length; i++)
						{
							auto obj = sources->vector[i];

							if (obj)
							{
								auto isSame = il2cpp_class_get_method_from_name_type<bool (*)(Il2CppObject*, Cute::Cri::AudioPlayback)>(obj->klass, "IsSamePlaybackId", 1)->methodPointer(obj, charaPlayback);
								if (isSame)
								{
									cuteAudioSource = obj;
									break;
								}
							}
						}

						if (cuteAudioSource)
						{
							auto sourceListField = il2cpp_class_get_field_from_name(cuteAudioSource->klass, "sourceList");
							Il2CppObject* sourceList;
							il2cpp_field_get_value(cuteAudioSource, sourceListField, &sourceList);

							auto usingIndexField = il2cpp_class_get_field_from_name(cuteAudioSource->klass, "usingIndex");
							int usingIndex;
							il2cpp_field_get_value(cuteAudioSource, usingIndexField, &usingIndex);

							params = new void* [1];
							params[0] = &usingIndex;

							auto AtomSource = il2cpp_runtime_invoke(il2cpp_class_get_method_from_name(sourceList->klass, "get_Item", 1), sourceList, params, &exception);

							delete[] params;

							if (exception)
							{
								continue;
							}

							auto player = il2cpp_class_get_method_from_name_type<Il2CppObject * (*)(Il2CppObject*)>(AtomSource->klass, "get_player", 0)->methodPointer(AtomSource);
							il2cpp_class_get_method_from_name_type<CriWare::CriAtomExPlayback(*)(Il2CppObject*)>(player->klass, "StopWithoutReleaseTime", 0)->methodPointer(player);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, long)>(player->klass, "SetStartTime", 1)->methodPointer(player, static_cast<long>(roundf(value * 1000.0f)));
							auto playback = il2cpp_class_get_method_from_name_type<CriWare::CriAtomExPlayback(*)(Il2CppObject*)>(player->klass, "Start", 0)->methodPointer(player);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, CriWare::CriAtomExPlayback)>(player->klass, "Update", 1)->methodPointer(player, playback);

							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(player->klass, "Pause", 0)->methodPointer(player);

							charaPlayback.criAtomExPlayback = playback;

							il2cpp_array_setref_type_memmove(_songCharaPlaybacks, Cute::Cri::AudioPlayback, i, &playback);
							il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*, CriWare::CriAtomExPlayback)>(AtomSource->klass, "set_Playback", 1)->methodPointer(AtomSource, playback);
						}
					}
				}
			}

			if (!isPauseLive)
			{
				il2cpp_class_get_method_from_name_type<void (*)(Il2CppObject*)>(LiveTimeController->klass, "ResumeLive", 0)->methodPointer(LiveTimeController);
			}
		}

		IsMovingLivePlayback = false;
	}
}
