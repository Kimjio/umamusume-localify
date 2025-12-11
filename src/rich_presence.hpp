#pragma once

#include <string>

#include "il2cpp/il2cpp_symbols.hpp"
#include "string_utils.hpp"

using namespace std;

extern uint64_t GetTextIdByName(const u16string& name);

namespace
{
	extern Il2CppString* localizeextension_text_hook(uint64_t id);

	string GetViewName(string viewId)
	{
		u16string textId;
		if (viewId == "Splash")
		{

		}
		if (viewId == "Title")
		{

		}
		if (viewId == "Download")
		{

		}
		if (viewId == "DayChangeHub")
		{

		}
		if (viewId == "SpecialLoginBonus")
		{

		}
		if (viewId == "TutorialOpMovie")
		{

		}
		if (viewId == "TutorialGachaTop")
		{

		}
		if (viewId == "TutorialCutInPlay")
		{

		}
		if (viewId == "Att")
		{

		}
		if (viewId == "Announce")
		{

		}
		if (viewId == "Home" ||
			viewId == "HomeHub")
		{
			textId = u"Home0036";
		}
		if (viewId == "Live")
		{

		}
		if (viewId == "GachaMain")
		{

		}
		if (viewId == "GachaResult")
		{

		}
		if (viewId == "RaceMain")
		{

		}
		if (viewId == "SingleModeStart")
		{

		}
		if (viewId == "SingleModeMonthStart")
		{

		}
		if (viewId == "SingleModeMain")
		{

		}
		if (viewId == "SingleModePaddock")
		{

		}
		if (viewId == "SingleModeRaceEntry")
		{

		}
		if (viewId == "SingleModeConfirmComplete")
		{

		}
		if (viewId == "SingleModeResult")
		{

		}
		if (viewId == "SingleModeSkillLearning")
		{

		}
		if (viewId == "SingleModeSuccessionCut")
		{

		}
		if (viewId == "SingleModeSuccessionEvent")
		{

		}
		if (viewId == "SingleModeScenarioTeamRaceTop")
		{

		}
		if (viewId == "SingleModeScenarioTeamRaceDeck")
		{

		}
		if (viewId == "SingleModeScenarioTeamRaceCharaSelect")
		{

		}
		if (viewId == "SingleModeScenarioTeamRaceOpponentSelect")
		{

		}
		if (viewId == "SingleModeScenarioTeamRaceRaceList")
		{

		}
		if (viewId == "SingleModeScenarioTeamRacePaddock")
		{

		}
		if (viewId == "SingleModeScenarioTeamRaceGrandResult")
		{

		}
		if (viewId == "SingleModeScenarioFreeShop")
		{

		}
		if (viewId == "SingleModeScenarioLiveSelect")
		{

		}
		if (viewId == "SingleModeScenarioLiveTop")
		{

		}
		if (viewId == "SingleModeScenarioVenusScenarioRaceTop")
		{

		}
		if (viewId == "SingleModeScenarioVenusScenarioRacePaddock")
		{

		}
		if (viewId == "Story")
		{

		}
		if (viewId == "EpisodeMain")
		{

		}
		if (viewId == "EpisodeMainCharacterSelect")
		{

		}
		if (viewId == "EpisodeMainUnlockRaceCutin")
		{

		}
		if (viewId == "EpisodeCharacter")
		{

		}
		if (viewId == "MainStoryPaddock")
		{

		}
		if (viewId == "EpisodeExtra")
		{

		}
		if (viewId == "StoryMovie")
		{

		}
		if (viewId == "TeamStadium")
		{

		}
		if (viewId == "TeamStadiumDeckHub")
		{

		}
		if (viewId == "TeamStadiumDeck")
		{

		}
		if (viewId == "TeamStadiumCharacterSelect")
		{

		}
		if (viewId == "TeamStadiumOpponentSelect")
		{

		}
		if (viewId == "TeamStadiumDecide")
		{

		}
		if (viewId == "TeamStadiumRaceList")
		{

		}
		if (viewId == "TeamStadiumPaddock")
		{

		}
		if (viewId == "TeamStadiumGrandResult")
		{

		}
		if (viewId == "TeamStadiumRaceHub")
		{

		}
		if (viewId == "OutGame")
		{

		}
		if (viewId == "TrainedCharaList")
		{

		}
		if (viewId == "Friend")
		{

		}
		if (viewId == "CharacterNoteTop" ||
			viewId == "CharacterNoteSelect" ||
			viewId == "CharacterNoteMain" ||
			viewId == "CharacterNoteHub")
		{
			textId = u"Directory0001";
		}
		if (viewId == "CharacterCardLimitBreakCut")
		{

		}
		if (viewId == "TipsComic")
		{
			textId = u"Outgame0072";
		}
		if (viewId == "TipsChara")
		{
			textId = u"Outgame0093";
		}
		if (viewId == "ProfileTop")
		{

		}
		if (viewId == "Honor")
		{

		}
		if (viewId == "Directory")
		{

		}
		if (viewId == "Gallery")
		{

		}
		if (viewId == "TalkGallery")
		{

		}
		if (viewId == "DressChange")
		{

		}
		if (viewId == "ProfileCard")
		{

		}
		if (viewId == "ProfileCardHonor")
		{

		}
		if (viewId == "PhotoLibrary")
		{
			textId = u"Outgame213009";
		}
		if (viewId == "PhotoStudio" ||
			viewId == "PhotoStudioPlayCut" ||
			viewId == "PhotoStudioViewTop" ||
			viewId == "PhotoStudioCharaViewer")
		{
			textId = u"Outgame352001";
		}
		if (viewId == "Mission")
		{

		}
		if (viewId == "CharacterCardHaveList")
		{

		}
		if (viewId == "SupportCardList")
		{

		}
		if (viewId == "SupportCardHaveList")
		{

		}
		if (viewId == "SupportCardWaitingRoom")
		{

		}
		if (viewId == "SupportCardDeckEdit")
		{

		}
		if (viewId == "CharacterCardCatalog")
		{

		}
		if (viewId == "CharacterPieceExchange")
		{

		}
		if (viewId == "DailyRace")
		{

		}
		if (viewId == "DailyRacePaddock")
		{

		}
		if (viewId == "LegendRace")
		{

		}
		if (viewId == "LegendRacePaddock")
		{

		}
		if (viewId == "DailyLegendRaceTop")
		{

		}
		if (viewId == "MenuItem")
		{

		}
		if (viewId == "LiveTheater")
		{

		}
		if (viewId == "MenuTrophyRoom")
		{

		}
		if (viewId == "MenuShop")
		{

		}
		if (viewId == "Circle")
		{

		}
		if (viewId == "Champions")
		{

		}
		if (viewId == "ChampionsLobby")
		{

		}
		if (viewId == "ChampionsPaddock")
		{

		}
		if (viewId == "CraneGame")
		{

		}
		if (viewId == "RoomMatchHub")
		{

		}
		if (viewId == "RoomMatchTop")
		{

		}
		if (viewId == "RoomMatchLobby")
		{

		}
		if (viewId == "RoomMatchCharacterSelect")
		{

		}
		if (viewId == "RoomMatchHostEntry")
		{

		}
		if (viewId == "RoomMatchGuestEntry")
		{

		}
		if (viewId == "RoomMatchPaddock")
		{

		}
		if (viewId == "RoomMatchCharacterEntry")
		{

		}
		if (viewId == "RoomMatchRaceResult")
		{

		}
		if (viewId == "RoomMatchDeckEdit")
		{

		}
		if (viewId == "PracticeRaceHub")
		{

		}
		if (viewId == "PracticeRaceTop")
		{

		}
		if (viewId == "PracticeRaceSelectRace")
		{

		}
		if (viewId == "PracticeRaceCharacterEntry")
		{

		}
		if (viewId == "PracticeRaceCharacterSelect")
		{

		}
		if (viewId == "PracticeRacePaddock")
		{

		}
		if (viewId == "PracticeRaceResult")
		{

		}
		if (viewId == "PracticeRaceDeckEdit")
		{

		}
		if (viewId == "TrainingChallengeHub")
		{

		}
		if (viewId == "TrainingChallengeTop")
		{

		}
		if (viewId == "TrainingChallengeLeading")
		{

		}
		if (viewId == "SupportCardRanking")
		{

		}
		if (viewId == "StoryEventHub")
		{

		}
		if (viewId == "StoryEventTop")
		{

		}
		if (viewId == "StoryEventMission")
		{

		}
		if (viewId == "RouletteDerby")
		{

		}
		if (viewId == "ChallengeMatchHub")
		{

		}
		if (viewId == "ChallengeMatchTop")
		{

		}
		if (viewId == "ChallengeMatchCharaSelect")
		{

		}
		if (viewId == "ChallengeMatchPaddock")
		{

		}
		if (viewId == "TransferEvent")
		{

		}
		if (viewId == "TeamBuildingHub")
		{

		}
		if (viewId == "TeamBuildingTop")
		{

		}
		if (viewId == "TeamBuildingOpponentSelect")
		{

		}
		if (viewId == "TeamBuildingMyTeamInfo")
		{

		}
		if (viewId == "TeamBuildingCaptainSelect")
		{

		}
		if (viewId == "TeamBuildingScout")
		{

		}
		if (viewId == "TeamBuildingVsCutin")
		{

		}
		if (viewId == "TeamBuildingCollection")
		{

		}
		if (viewId == "TeamBuildingFirstTransition")
		{

		}
		if (viewId == "TeamBuildingPaddock")
		{

		}
		if (viewId == "TeamBuildingScoutRanking")
		{

		}
		if (viewId == "HeroesHub")
		{

		}
		if (viewId == "HeroesOpening")
		{

		}
		if (viewId == "HeroesTop")
		{

		}
		if (viewId == "HeroesTeamEdit")
		{

		}
		if (viewId == "HeroesTeamMemberSelect")
		{

		}
		if (viewId == "HeroesGetSkillEvent")
		{

		}
		if (viewId == "HeroesStage1MatchingResult")
		{

		}
		if (viewId == "HeroesStage1RacingBase")
		{

		}
		if (viewId == "HeroesStage1GrandResult")
		{

		}
		if (viewId == "HeroesFinalRaceList")
		{

		}
		if (viewId == "HeroesPaddock")
		{

		}
		if (viewId == "FanRaid")
		{

		}
		if (viewId == "CollectEventMapHub")
		{

		}
		if (viewId == "CollectEventMap")
		{

		}
		if (viewId == "CollectEventLoginBonusGallery")
		{

		}
		if (viewId == "CollectRaidHub")
		{

		}
		if (viewId == "CollectRaidTop")
		{

		}
		if (viewId == "CollectRaidMission")
		{

		}
		if (viewId == "FactorResearchHub")
		{

		}
		if (viewId == "FactorResearchTop")
		{

		}
		if (viewId == "FactorResearchCharaSelect")
		{

		}
		if (viewId == "CampaignRaffle")
		{

		}
		if (viewId == "CampaignsValentineSpStory")
		{

		}
		if (viewId == "MapEvent")
		{

		}

		if (!textId.empty())
		{
			return u16_u8(localizeextension_text_hook(GetTextIdByName(textId))->chars).append(" ");
		}

		return "";
	}

	string GetSceneName(string sceneId)
	{
		u16string textId;
		if (sceneId == "Title")
		{

		}
		if (sceneId == "Home")
		{
			textId = u"Home0036";
		}
		if (sceneId == "Race")
		{
			textId = u"Outgame213036";
		}
		if (sceneId == "Live")
		{
			textId = u"Common0035";
		}
		if (sceneId == "LiveTheater")
		{
			textId = u"Home0037";
		}
		if (sceneId == "Story" || sceneId == "StoryMovie")
		{
			textId = u"Home0066";
		}
		if (sceneId == "Gacha")
		{
			textId = u"Gacha0004";
		}
		if (sceneId == "Episode")
		{
			textId = u"Home0036";
		}
		if (sceneId == "SingleMode")
		{
			textId = u"Common0273";
		}
		if (sceneId == "OutGame")
		{
			// TODO
		}
		if (sceneId == "Circle")
		{
			textId = u"Circle0321";
		}
		if (sceneId == "DailyRace")
		{
			textId = u"Race0043";
		}
		if (sceneId == "LegendRace")
		{
			textId = u"Race0072";
		}
		if (sceneId == "TeamStadium")
		{
			textId = u"Race0583";
		}
		if (sceneId == "CraneGame")
		{

		}
		if (sceneId == "Champions" || sceneId == "ChampionsLobby")
		{
			textId = u"Champions0034";
		}
		if (sceneId == "Tutorial")
		{

		}
		if (sceneId == "StoryEvent")
		{
			textId = u"StoryEvent0018";
		}
		if (sceneId == "ChallengeMatch")
		{
			textId = u"ChallengeMatch408005";
		}
		if (sceneId == "RoomMatch")
		{
			textId = u"RoomMatch0001";
		}
		if (sceneId == "PracticeRace")
		{
			textId = u"PracticeRace400101";
		}
		if (sceneId == "TrainingChallenge")
		{
			textId = u"TrainingChallenge4180100";
		}
		if (sceneId == "TeamBuilding")
		{
			textId = u"TeamBuilding408033";
		}
		if (sceneId == "FanRaid")
		{

		}
		if (sceneId == "CampaignRaffle")
		{

		}
		if (sceneId == "CollectEventMap")
		{
			textId = u"CollectEvent508000";
		}
		if (sceneId == "CollectRaid")
		{
			textId = u"CollectEvent508000";
		}
		if (sceneId == "MapEvent")
		{

		}
		if (sceneId == "FactorResearch")
		{

		}
		if (sceneId == "Heroes")
		{

		}

		if (!textId.empty())
		{
			return u16_u8(localizeextension_text_hook(GetTextIdByName(textId))->chars).append(" ");
		}

		return sceneId;
	}
}
