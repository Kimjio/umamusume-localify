#pragma once

namespace Gallop
{
	class WebViewDefine
	{
	public:
		enum Url
		{
			None,
			Update,
			Event,
			Bug,
			NewsDetail,
			TermOfService,
			Tokusho,
			ShopPayment,
			PrivacyPolicy,
			License,
			Help,
			AppleId,
			StoryEventHelp,
			TrainingChallengeHelp,
			ChallengeMatchHelp,
			FanRaidHelp,
			CollectRaidHelp,
			TeamBuildingHelp,
			FactorResearchHelp,
			HeroesHelp,
			UltimateRaceHelp,
			RatingRaceHelp,
		};

		enum HelpCategory
		{
			Home = 1,
			Character,
			Story,
			Twinkle,
			HolydayRace,
			Training,
			Skill,
			Live,
			School,
			Gacha,
			Menu,
			TrainerNote,
			StoryEvent
		};

		enum FontNameDefine
		{
			JP_DYNAMIC01,
		};

		enum DmmUrlType
		{
			Cygames,
			Dmm,
			Official,
			Web,
		};
	};
}