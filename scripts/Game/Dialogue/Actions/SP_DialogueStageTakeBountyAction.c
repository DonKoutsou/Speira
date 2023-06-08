[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageTakeBountyAction : DialogueStage
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_ItemToGive;
	
	[Attribute("", UIWidgets.Coords, params: "", desc: "")]
	vector m_SpawnOffset;
	
	[Attribute()]
	private ResourceName m_pDefaultWaypoint;
	
	string LocName;
	FactionKey TFact;
	SP_AIDirector MyDirector;
	
	string DName;
	
	override void Perform(IEntity Character, IEntity Player)
	{
		AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
	
		FactionAffiliationComponent FC = FactionAffiliationComponent.Cast(Character.FindComponent(FactionAffiliationComponent));
		SCR_Faction myfact = SCR_Faction.Cast(FC.GetAffiliatedFaction());
		
		SP_AIDirector OrDirector = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		OrDirector.GetDirectorOccupiedByEnemy(myfact, MyDirector);
		string keyunused;
		Faction EnFaction =  MyDirector.GetMajorityHolder(keyunused);
		string key = EnFaction.GetFactionKey();
		
		vector mat[4];
		Character.GetWorldTransform(mat);
		vector spawnPos = m_SpawnOffset.Multiply3(mat) + mat[3];
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos; 
		IEntity BountyPaper;
		
		Resource res = Resource.Load(m_ItemToGive);
		if (res)
		{	
			BountyPaper = GetGame().SpawnEntityPrefab(res, Character.GetWorld(), params);
		}
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
		if(inv.TryInsertItem(BountyPaper))
		{
			SP_DialogueComponent diagcomp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
			diagcomp.DoAnouncerDialogue("The bounty has been added to your inventory");
		}
		else
		{
			SP_DialogueComponent diagcomp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
			diagcomp.DoAnouncerDialogue("No space in inventory, bounty left on the floor");
		}
		
		SP_BountyComponent PComp = SP_BountyComponent.Cast(BountyPaper.FindComponent(SP_BountyComponent));
		
		CharacterIdentityComponent CharID = CharacterIdentityComponent.Cast(Character.FindComponent(CharacterIdentityComponent));
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(Character.FindComponent(SCR_CharacterRankComponent));
		string OName = CharRank.GetCharacterRankName(Character) + " " + CharID.GetIdentity().GetName() + " " + CharID.GetIdentity().GetSurname();
		IEntity CharToDeliverTo;
		MyDirector.GetRandomUnitByFKey(key, CharToDeliverTo);
		LocName = MyDirector.GetCharacterLocation(CharToDeliverTo);
		CharacterIdentityComponent CharID2 = CharacterIdentityComponent.Cast(CharToDeliverTo.FindComponent(CharacterIdentityComponent));
		DName = CharRank.GetCharacterRankName(CharToDeliverTo) + " " + CharID2.GetIdentity().GetName() + " " + CharID2.GetIdentity().GetSurname();
		//Resource res = Resource.Load(m_pDefaultWaypoint);
		//AIWaypoint WP;
		//if (res)
		//	{
		//		WP = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(res, CharToDeliverTo.GetWorld(), params));
		//	}
		//IEntity Antagonist1;
		//IEntity Antagonist2;
		//Find2RandomAllies(myfact, Antagonist1, Antagonist2);
		//AddWPToCh(Antagonist1, WP);
		//AddWPToCh(Antagonist2, WP);
		PComp.SetInfo(OName, CharToDeliverTo, DName, LocName, key);
		TFact = key;
	};
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		FactionAffiliationComponent FC = FactionAffiliationComponent.Cast(Character.FindComponent(FactionAffiliationComponent));
		SCR_Faction myfact = SCR_Faction.Cast(FC.GetAffiliatedFaction());
		SP_AIDirector OrDirector = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		m_sCantBePerformedReason = "(No available bounties)";
		return OrDirector.GetDirectorOccupiedByEnemy(myfact, MyDirector);
	}
	override string GetDialogueText(IEntity Character, IEntity Player)
	{
			return GetRandomDeliver();
	}
	string GetRandomDeliver()
	{
		string TextToSend = string.Format(DialogueText, DName, LocName, TFact);
		return TextToSend;
	}
	void Find2RandomAllies(Faction fact, out IEntity Char1, out IEntity Char2)
	{
		SP_AIDirector m_1stDirector;
		string keyunused;
		MyDirector.GetDirectorOccupiedByFriendly(fact, m_1stDirector);
		FactionKey fact1 = m_1stDirector.GetMajorityHolder(keyunused).GetFactionKey();
		SP_AIDirector m_2ndDirector;
		MyDirector.GetDirectorOccupiedByFriendly(fact, m_2ndDirector);
		FactionKey fact2 = m_2ndDirector.GetMajorityHolder(keyunused).GetFactionKey();
		m_1stDirector.GetRandomUnitByFKey(fact1, Char1);
		m_2ndDirector.GetRandomUnitByFKey(fact2, Char2);
	}
	void AddWPToCh(IEntity char, AIWaypoint Waypoint)
	{
		AIControlComponent cont = AIControlComponent.Cast(char.FindComponent(AIControlComponent));
		SCR_AICombatComponent combat = SCR_AICombatComponent.Cast(char.FindComponent(SCR_AICombatComponent));
		PerceptionComponent per = PerceptionComponent.Cast(char.FindComponent(PerceptionComponent));
		BaseTarget target = per.GetTargetPerceptionObject(char, ETargetCategory.FRIENDLY);
		combat.SelectTarget(target);
		//AIGroup group;
		//if(cont)
		//{
		//	group = cont.GetAIAgent().GetParentGroup();
		//}
		//if (group)
		//{
		//	if (Waypoint)
		//		group.AddWaypoint(Waypoint);
		//}
	
	}
	
	

}