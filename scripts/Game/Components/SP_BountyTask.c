class SP_BountyTask: SP_Task
{
	IEntity Bounty;
	IEntity GetBountyEnt()
	{
		return Bounty;
	}
	void GetInfo(out string OName, out string DName, out string DLoc)
	{
		if(!TaskOwner || !TaskTarget)
		{
			return;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		AIControlComponent comp = AIControlComponent.Cast(TaskTarget.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector Director = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(TaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = CharRank.GetCharacterRankName(TaskOwner) + " " + Diag.GetCharacterName(TaskOwner);
		DName = CharRank.GetCharacterRankName(TaskTarget) + " " + Diag.GetCharacterName(TaskTarget);
		DLoc = Director.GetCharacterLocation(TaskTarget);
	};
	
	override bool Init()
	{
		if(!TaskOwner && !TaskTarget)
		{
			SP_AIDirector MyDirector = SP_AIDirector.AllDirectors.GetRandomElement();
			if(!MyDirector)
			{
				return false;
			}
			FactionManager factionsMan = FactionManager.Cast(GetGame().GetFactionManager());
			string keyunused;
			Faction Fact = MyDirector.GetMajorityHolder(keyunused);
			if(!Fact)
			{
				return false;
			}
			FactionKey key = Fact.GetFactionKey();
			SCR_Faction myfact = SCR_Faction.Cast(factionsMan.GetFactionByKey(key));
			MyDirector.GetDirectorOccupiedByFriendly(myfact, MyDirector);
			IEntity Character;
			IEntity CharToDeliverTo;
			if(MyDirector.GetRandomUnitByFKey(key, Character) == false)
			{
				return false;
			}
			while(!Character)
			{
				MyDirector.GetRandomUnitByFKey(key, Character);
			}
			SP_AIDirector NewDir;
			if(MyDirector.GetDirectorOccupiedByEnemy(myfact, NewDir) == false)
			{
				return false;
			}
			Faction EnFact = NewDir.GetMajorityHolder(keyunused);
			if(!EnFact)
			{
				return false;
			}
			FactionKey Enkey = EnFact.GetFactionKey();
			if(NewDir.GetRandomUnitByFKey(Enkey, CharToDeliverTo) == false)
			{
				return false;
			}
			while(!CharToDeliverTo)
			{
				NewDir.GetRandomUnitByFKey(Enkey, CharToDeliverTo);
			}
			SetInfo(Character, CharToDeliverTo);
			string OName;
			string DName;
			string DLoc;
			GetInfo(OName, DName, DLoc);
			if(OName == " " || DName == " " || DLoc == " ")
			{
				return false;
			}
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = vector.Zero;
			Resource res = Resource.Load("{F73F8F714B2662FC}prefabs/Items/BountyPaper.et");
			if (res)
			{
				Bounty = GetGame().SpawnEntityPrefab(res, GetGame().GetWorld(), params);
				InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Character.FindComponent(InventoryStorageManagerComponent));
				if(inv.TryInsertItem(Bounty) == false)
				{
					delete Bounty;
					return false;
				}
			}
			SP_BountyComponent BComp = SP_BountyComponent.Cast(Bounty.FindComponent(SP_BountyComponent));
			BComp.SetInfo(OName, DName, DLoc);
			TaskDesc = string.Format("%1 has put a bounty on %2's head.", OName, DName);
			TaskDiag = string.Format("I've put a bounty on %1's head, get me his dogtags and i'll make it worth your while", DName);
			return true;
		}
		string OName;
		string DName;
		string DLoc;
		GetInfo(OName, DName, DLoc);
		if(OName == " " || DName == " " || DLoc == " ")
		{
			return false;
		}
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = vector.Zero;
		Resource res = Resource.Load("{F73F8F714B2662FC}prefabs/Items/BountyPaper.et");
		if (res)
		{
			Bounty = GetGame().SpawnEntityPrefab(res, GetGame().GetWorld(), params);
			InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(TaskOwner.FindComponent(InventoryStorageManagerComponent));
			if(inv.TryInsertItem(Bounty) == false)
			{
				delete Bounty;
				return false;
			}
		}
		SP_BountyComponent BComp = SP_BountyComponent.Cast(Bounty.FindComponent(SP_BountyComponent));
		BComp.SetInfo(OName, DName, DLoc);
		TaskDesc = string.Format("%1 has put a bounty on %2's head.", OName, DName);
		TaskDiag = string.Format("I've put a bounty on %1's head, get me his dogtags and i'll make it worth your while", DName);
		return true;
	};
};