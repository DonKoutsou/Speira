[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStagePaperCheckAction : DialogueStageItemCheckAction
{
	[Attribute(defvalue: "AltAction Text", desc: "Action Title", category: "Dialogue")]
	string AltActionText;
	[Attribute(defvalue: "AltDialogue Text", desc: "Dialogue Text", category: "Dialogue")]
    string AltDialogueText
	override string GetDialogueText(IEntity Character, IEntity Player)
	{
		if (IsPossible(Character, Player) == false)
		{
			return AltDialogueText;
		}
	 	return DialogueText;
	}
	override void Perform(IEntity Character, IEntity Player)
	{
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
		SP_PrefabResource_Predicate pred = new SP_PrefabResource_Predicate(m_WantedItem);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		inv.FindItems(entitiesToDrop, pred);
		if (entitiesToDrop.Count() < m_WantedAmount)
		{
			SCR_FactionManager factman = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			factman.SetFactionsHostile(SCR_Faction.Cast(factman.GetFactionByKey("SPEIRA")), SCR_Faction.Cast(factman.GetFactionByKey("USSR")));
			SP_DialogueComponent DiagComp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
			DiagComp.DoAnouncerDialogue("You were caught without papers, you will be considered enemy to USSR faction from now on");
			DiagComp.DoBackDialogue(Character, Player);
			DiagComp.ReleaseAI(Character, Player);
			GetGame().GetMenuManager().CloseAllMenus();
		}
	};
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{	
		return true;
	}
	bool IsPossible(IEntity Character, IEntity Player)
	{	
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
		if (!inv)
			return false;
		SP_PrefabResource_Predicate pred = new SP_PrefabResource_Predicate(m_WantedItem);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		inv.FindItems(entitiesToDrop, pred);
		if (entitiesToDrop.Count() < m_WantedAmount)
		{
			m_sCantBePerformedReason = "[Missing Item]";
			return false;
		}
		return true;
	}
	override string GetActionText(IEntity Character, IEntity Player)
	{
		if (IsPossible(Character, Player) == false)
		{
			string acttext = AltActionText;
			return acttext;
		}
		if (CanBeShown(Character, Player) == false)
		{
		 return STRING_EMPTY;
		}
	 	return ActionText;
	}

};