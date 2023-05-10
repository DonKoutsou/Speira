[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageTimedRewardAction : DialogueStage
{
	
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ref array <ref ResourceName> m_ItemToGive;
	
	[Attribute("", UIWidgets.Coords, params: "", desc: "")]
	vector m_SpawnOffset;
	
	[Attribute()]
    int TimeOffset;
	bool TimeChecked = false;
	int TimeToBeAvailable;
	int DayToBeAvailable;
	override void Perform(IEntity Character, IEntity Player)
	{
		vector mat[4];
		Character.GetWorldTransform(mat);
		vector spawnPos = m_SpawnOffset.Multiply3(mat) + mat[3];
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos; 
		for (int i = 0; i < m_ItemToGive.Count(); i++)
		{
			Resource res = Resource.Load(m_ItemToGive[i]);
			if (res)
			{	
				GetGame().SpawnEntityPrefab(res, Character.GetWorld(), params);
			}
		}
	};
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		int currentday = GetGame().GetTimeAndWeatherManager().GetDay();
		if (DayToBeAvailable == currentday)
		{
			int currenttime = GetGame().GetTimeAndWeatherManager().GetTime().m_iHours;
			if (TimeToBeAvailable <= currenttime)
			{
				return true;
			}
		}
		if (DayToBeAvailable < currentday)
		{
			return true;
		}
		m_sCantBePerformedReason = "[" + "Not ready, come back at" + " " + TimeToBeAvailable + " " + "o'clock" + "]";
		return false;
	}
	override string GetActionText(IEntity Character, IEntity Player)
	{
		if(TimeChecked == false)
		{
			TimeToBeAvailable = GetGame().GetTimeAndWeatherManager().GetTime().m_iHours + TimeOffset;
			DayToBeAvailable = GetGame().GetTimeAndWeatherManager().GetDay();
			if(TimeToBeAvailable > 24)
			{
				TimeToBeAvailable = TimeToBeAvailable - 24;
				DayToBeAvailable = DayToBeAvailable + 1;
			}
		}
		TimeChecked = true;
		if (CanBePerformed(Character, Player) == false)
		{
			string acttext = ActionText + " " + m_sCantBePerformedReason;
			return acttext;
		}
		if (CanBeShown(Character, Player) == false)
		{
		 return STRING_EMPTY;
		}
	 	return ActionText;
	}

};