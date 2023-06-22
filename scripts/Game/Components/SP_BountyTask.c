class SP_BountyTask: SP_Task
{
	IEntity Bounty;
	
	void GetInfo(out string OName, out string DName, out string DLoc)
	{
		if(!TaskOwner || !TaskTarget)
		{
			return;
		}
		AIControlComponent comp = AIControlComponent.Cast(TaskTarget.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector Director = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(TaskOwner.FindComponent(SCR_CharacterRankComponent));
		CharacterIdentityComponent CharID = CharacterIdentityComponent.Cast(TaskOwner.FindComponent(CharacterIdentityComponent));
		CharacterIdentityComponent CharID2 = CharacterIdentityComponent.Cast(TaskTarget.FindComponent(CharacterIdentityComponent));
		OName = CharRank.GetCharacterRankName(TaskOwner) + " " + CharID.GetIdentity().GetName() + " " + CharID.GetIdentity().GetSurname();
		DName = CharRank.GetCharacterRankName(TaskTarget) + " " + CharID2.GetIdentity().GetName() + " " + CharID2.GetIdentity().GetSurname();
		DLoc = Director.GetCharacterLocation(TaskTarget)
	};
	
	override void Init()
	{
		if(!TaskOwner && !TaskTarget)
		{
			int dirindex = Math.RandomInt(0, SP_AIDirector.AllDirectors.Count());
			SP_AIDirector MyDirector = SP_AIDirector.AllDirectors[dirindex];
			FactionManager factionsMan = FactionManager.Cast(GetGame().GetFactionManager());
			string keyunused;
			FactionKey key = MyDirector.GetMajorityHolder(keyunused).GetFactionKey();
			SCR_Faction myfact = SCR_Faction.Cast(factionsMan.GetFactionByKey(key));
			MyDirector.GetDirectorOccupiedByFriendly(myfact, MyDirector);
			IEntity Character;
			IEntity CharToDeliverTo;
			MyDirector.GetRandomUnitByFKey(key, Character);
			while(!Character)
			{
				MyDirector.GetRandomUnitByFKey(key, Character);
			}
			SP_AIDirector NewDir;
			MyDirector.GetDirectorOccupiedByEnemy(myfact, NewDir);
			NewDir.GetRandomUnitByFKey(key, CharToDeliverTo);
			while(!CharToDeliverTo)
			{
				MyDirector.GetRandomUnitByFKey(key, CharToDeliverTo);
			}
			SetInfo(Character, CharToDeliverTo);
		}
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = vector.Zero;
		Resource res = Resource.Load("{F73F8F714B2662FC}prefabs/Items/BountyPaper.et");
		if (res)
		{
			Bounty = GetGame().SpawnEntityPrefab(res, GetGame().GetWorld(), params);
		}
		SP_BountyComponent BComp = SP_BountyComponent.Cast(Bounty.FindComponent(SP_BountyComponent));
		string OName;
		string DName;
		string DLoc;
		GetInfo(OName, DName, DLoc);
		BComp.SetInfo(OName, DName, DLoc);
		TaskDesc = string.Format("%1 has put a bounty on %2's head.", OName, DName);
	};
};