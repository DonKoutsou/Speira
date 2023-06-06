[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageTakePackageAction : DialogueStage
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ref array <ref ResourceName> m_ItemToGive;
	[Attribute("", UIWidgets.Coords, params: "", desc: "")]
	vector m_SpawnOffset;
	string LocName;
	SP_AIDirector MyDirector;
	
	string DName;
	
	override void Perform(IEntity Character, IEntity Player)
	{
		AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		
		
		FactionAffiliationComponent FC = FactionAffiliationComponent.Cast(Character.FindComponent(FactionAffiliationComponent));
		SCR_Faction myfact = SCR_Faction.Cast(FC.GetAffiliatedFaction());
		
		SP_AIDirector OrDirector = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		OrDirector.GetDirectorOccupiedByFriendly(myfact, MyDirector);
		string keyunused;
		string key = MyDirector.GetMajorityHolder(keyunused).GetFactionKey();
		
		vector mat[4];
		Character.GetWorldTransform(mat);
		vector spawnPos = m_SpawnOffset.Multiply3(mat) + mat[3];
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos; 
		IEntity Package;
		for (int i = 0; i < m_ItemToGive.Count(); i++)
		{
			Resource res = Resource.Load(m_ItemToGive[i]);
			if (res)
			{	
				Package = GetGame().SpawnEntityPrefab(res, Character.GetWorld(), params);
			}
		}
		SP_PackageComponent PComp = SP_PackageComponent.Cast(Package.FindComponent(SP_PackageComponent));
		
		CharacterIdentityComponent CharID = CharacterIdentityComponent.Cast(Character.FindComponent(CharacterIdentityComponent));
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(Character.FindComponent(SCR_CharacterRankComponent));
		string OName = CharRank.GetCharacterRankName(Character) + " " + CharID.GetIdentity().GetName() + " " + CharID.GetIdentity().GetSurname();
		IEntity CharToDeliverTo;
		MyDirector.GetRandomUnitByFKey(key, CharToDeliverTo);
		LocName = MyDirector.GetCharacterLocation(CharToDeliverTo);
		CharacterIdentityComponent CharID2 = CharacterIdentityComponent.Cast(CharToDeliverTo.FindComponent(CharacterIdentityComponent));
		DName = CharRank.GetCharacterRankName(CharToDeliverTo) + " " + CharID2.GetIdentity().GetName() + " " + CharID2.GetIdentity().GetSurname();
		PComp.SetInfo(OName, DName, LocName, key);
	};
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		FactionAffiliationComponent FC = FactionAffiliationComponent.Cast(Character.FindComponent(FactionAffiliationComponent));
		SCR_Faction myfact = SCR_Faction.Cast(FC.GetAffiliatedFaction());
		SP_AIDirector OrDirector = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		m_sCantBePerformedReason = "(No available deliveries)";
		return OrDirector.GetDirectorOccupiedByFriendly(myfact, MyDirector);
	}
	override string GetDialogueText(IEntity Character, IEntity Player)
	{
			FactionAffiliationComponent FC = FactionAffiliationComponent.Cast(Character.FindComponent(FactionAffiliationComponent));
			string key = FC.GetAffiliatedFaction().GetFactionKey();
			return GetRandomDeliver(key);
	};
	string GetRandomDeliver(string key)
	{
		string TextToSend = string.Format(DialogueText, DName, LocName);
		return TextToSend;
	}

};