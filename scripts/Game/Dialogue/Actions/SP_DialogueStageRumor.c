[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageRumor : DialogueStage
{
	[Attribute("")]
	ref array<string> m_sAIDirectorName;
	
	override void Perform(IEntity Character, IEntity Player)
	{
	};
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		return true;
	};
	override string GetDialogueText(IEntity Character, IEntity Player)
	{
			FactionAffiliationComponent FC = FactionAffiliationComponent.Cast(Character.FindComponent(FactionAffiliationComponent));
			string key = FC.GetAffiliatedFaction().GetFactionKey();
			return GetRandomLocationPopulation(key);
	};
	string GetRandomLocationPopulation(string key)
	{
		int index;
		index = Math.RandomInt(0, m_sAIDirectorName.Count());
		string EntName = "SP_AIDirector_" + m_sAIDirectorName[index];
		SP_AIDirector RendomDirector = SP_AIDirector.Cast(GetGame().FindEntity(EntName));
		string faction = RendomDirector.GetMajorityHolder();
		if(faction == key)
		{
			index = Math.RandomInt(0, m_sAIDirectorName.Count());
			EntName = "SP_AIDirector_" + m_sAIDirectorName[index];
			RendomDirector = SP_AIDirector.Cast(GetGame().FindEntity(EntName));
			faction = RendomDirector.GetMajorityHolder();
		}
		string TextToSend = string.Format(DialogueText, faction, RendomDirector.GetLocationName());
		return TextToSend;
		
	}

};