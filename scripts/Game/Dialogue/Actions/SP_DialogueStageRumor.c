[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageRumor : DialogueStage
{
	ref array <int> usedindex = new array<int>();
	
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
		SP_AIDirector RandomDirector;
		index = Math.RandomInt(0, SP_AIDirector.AllDirectors.Count());
		RandomDirector = SP_AIDirector.AllDirectors[index];
		usedindex.Insert(index);
		
		while(usedindex.Contains(index) == true)
		{
			index = Math.RandomInt(0, SP_AIDirector.AllDirectors.Count());
			RandomDirector = SP_AIDirector.AllDirectors[index];
		}
		
		string FactioReadble = "";
		string faction = RandomDirector.GetMajorityHolder(FactioReadble);
		
		while (faction == key)
		{
			index = Math.RandomInt(0, SP_AIDirector.AllDirectors.Count());
			if(usedindex.Contains(index) == false)
			{
				RandomDirector = SP_AIDirector.AllDirectors[index];
				usedindex.Insert(index);
			}
			faction = RandomDirector.GetMajorityHolder(FactioReadble);
		}	
		if(SP_AIDirector.AllDirectors.Count() <= usedindex.Count())
		{
			usedindex.Clear();
		}		
		string TextToSend = string.Format(DialogueText, FactioReadble, RandomDirector.GetLocationName());
		return TextToSend;
	}

};