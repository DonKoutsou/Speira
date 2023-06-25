class SP_DeliverTask: SP_Task
{
	IEntity Package;
	IEntity GetPackage()
	{
		return Package;
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
			int dirindex = Math.RandomInt(0, SP_AIDirector.AllDirectors.Count());
			SP_AIDirector MyDirector = SP_AIDirector.AllDirectors.GetRandomElement();
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
			if(MyDirector.GetDirectorOccupiedByFriendly(myfact, NewDir) == false)
			{
				return false;
			}
			while(MyDirector == NewDir || !NewDir)
			{
				MyDirector.GetDirectorOccupiedByFriendly(myfact, NewDir);
			}
			if(NewDir.GetRandomUnitByFKey(key, CharToDeliverTo) == false)
			{
				return false;
			}
			while(!CharToDeliverTo)
			{
				NewDir.GetRandomUnitByFKey(key, CharToDeliverTo);
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
			Resource res = Resource.Load("{057AEFF961B81816}prefabs/Items/Package.et");
			if (res)
			{
				Package = GetGame().SpawnEntityPrefab(res, GetGame().GetWorld(), params);
				InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Character.FindComponent(InventoryStorageManagerComponent));
				if(inv.TryInsertItem(Package) == false)
				{
					delete Package;
					return false;
				}
			}
			SP_PackageComponent PComp = SP_PackageComponent.Cast(Package.FindComponent(SP_PackageComponent));
			PComp.SetInfo(OName, DName, DLoc);
			TaskDesc = string.Format("Deliver package received from %1, to %2. %2 is located on %3", OName, DName, DLoc);
			TaskDiag = string.Format("I am looking for someone to deliver a package for me to %1", DName);
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
		Resource res = Resource.Load("{057AEFF961B81816}prefabs/Items/Package.et");
		if (res)
		{
			Package = GetGame().SpawnEntityPrefab(res, GetGame().GetWorld(), params);
			InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(TaskOwner.FindComponent(InventoryStorageManagerComponent));
			if(inv.TryInsertItem(Package) == false)
			{
				delete Package;
				return false;
			}
		}
		SP_BountyComponent BComp = SP_BountyComponent.Cast(Package.FindComponent(SP_BountyComponent));
		BComp.SetInfo(OName, DName, DLoc);
		TaskDesc = string.Format("Deliver package received from %1, to %2. %2 is located on %3", OName, DName, DLoc);
		TaskDiag = string.Format("I am looking for someone to deliver a package for me to %1", DName);
		return true;
	};
	

};