class SP_DeliverTask: SP_Task
{
	IEntity Package;
	IEntity GetPackage()
	{
		return Package;
	}
	void GetInfo(out string OName, out string DName, out string OLoc, out string DLoc)
	{
		if (!TaskOwner || !TaskTarget)
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
		OLoc = Director.GetCharacterLocation(TaskOwner);
		DLoc = Director.GetCharacterLocation(TaskTarget);
	};
	override bool SetupTaskEntity()
	{
		string OName;
		string DName;
		string DLoc;
		string OLoc;
		GetInfo(OName, DName,OLoc, DLoc);
		if (OName == " " || DName == " " || DLoc == " ")
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
			if (inv.TryInsertItem(Package) == false)
			{
				delete Package;
				return false;
			}
		}
		SP_PackageComponent PComp = SP_PackageComponent.Cast(Package.FindComponent(SP_PackageComponent));
		PComp.SetInfo(OName, DName, DLoc);
		TaskDesc = string.Format("Deliver package received from %1, to %2. %2 is located on %3", OName, DName, DLoc);
		TaskDiag = string.Format("I am looking for someone to deliver a package for me to %1 on %2. Come find me on %3. Reward is %4", DName, DLoc, OLoc, m_iRewardAmount);
		e_State = ETaskState.UNASSIGNED;
		return true;
	}
	override void UpdateState()
	{
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(TaskTarget.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		SP_PackagePredicate PackPred = new SP_PackagePredicate(Diag.GetCharacterName(TaskTarget));
		array <IEntity> FoundPackages = new array <IEntity>();
		inv.FindItems(FoundPackages, PackPred);
		if (FoundPackages.Count() > 0)
		{
			foreach (IEntity MyPackage : FoundPackages)
			{
				if (MyPackage == Package)
				{
					e_State = ETaskState.COMPLETED;
					return;
				}
			}
		}
		SCR_CharacterDamageManagerComponent DmgComp = SCR_CharacterDamageManagerComponent.Cast(TaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
		if (DmgComp.IsDestroyed())
		{
			e_State = ETaskState.FAILED;
			return;
		}
	}
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		if (TalkingChar != TaskTarget)
		{
			return false;
		}
		if (!CharacterAssigned(Assignee))
		{
			return false;
		}
		if (e_State == ETaskState.COMPLETED)
		{
			return false;
		}
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		SP_PackagePredicate PackPred = new SP_PackagePredicate(Diag.GetCharacterRankName(TalkingChar) + " " + Diag.GetCharacterName(TalkingChar));
		array <IEntity> FoundPackages = new array <IEntity>();
		inv.FindItems(FoundPackages, PackPred);
		if (FoundPackages.Count() > 0)
		{
			for (int i, count = FoundPackages.Count(); i < count; i++)
			{
				if (FoundPackages[i] == Package)
				{
					return true;
				}
			}
		}
		return false;
	};
	override bool CompleteTask(IEntity Assignee)
	{
		InventoryStorageManagerComponent Assigneeinv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		InventoryStorageManagerComponent Targetinv = InventoryStorageManagerComponent.Cast(TaskTarget.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		SP_PackagePredicate PackPred = new SP_PackagePredicate(Diag.GetCharacterRankName(TaskTarget) + " " + Diag.GetCharacterName(TaskTarget));
		array <IEntity> FoundPackages = new array <IEntity>();
		Assigneeinv.FindItems(FoundPackages, PackPred);
		if (FoundPackages.Count() > 0)
		{
			if (GiveReward(Assignee))
			{
				InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
				InventoryItemComponent pInvComp = InventoryItemComponent.Cast(FoundPackages[0].FindComponent(InventoryItemComponent));
				InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
				inv.TryRemoveItemFromStorage(FoundPackages[0],parentSlot.GetStorage());
				Targetinv.TryInsertItem(FoundPackages[0]);
				e_State = ETaskState.COMPLETED;
				return true;
			}
		}
		return false;
	};
	override bool Init()
	{
		m_iRewardAmount = Math.RandomInt(5, 15);
		if (!TaskOwner && !TaskTarget)
		{
			int dirindex = Math.RandomInt(0, SP_AIDirector.AllDirectors.Count());
			SP_AIDirector MyDirector = SP_AIDirector.AllDirectors.GetRandomElement();
			FactionManager factionsMan = FactionManager.Cast(GetGame().GetFactionManager());
			string keyunused;
			Faction Fact = MyDirector.GetMajorityHolder(keyunused);
			if (!Fact)
			{
				return false;
			}
			FactionKey key = Fact.GetFactionKey();
			SCR_Faction myfact = SCR_Faction.Cast(factionsMan.GetFactionByKey(key));
			MyDirector.GetDirectorOccupiedByFriendly(myfact, MyDirector);
			IEntity Character;
			IEntity CharToDeliverTo;
			if (!MyDirector.GetRandomUnitByFKey(key, Character))
			{
				return false;
			}
			SP_AIDirector NewDir;
			if (!MyDirector.GetDirectorOccupiedByFriendly(myfact, NewDir))
			{
				return false;
			}
			if (!NewDir.GetRandomUnitByFKey(key, CharToDeliverTo))
			{
				return false;
			}
			if (!Character || !CharToDeliverTo)
			{
				return false;
			}
			SetInfo(Character, CharToDeliverTo);
		}
		if (SetupTaskEntity())
		{
			return true;
		}
		return false;
	};
};
class SP_PackagePredicate : InventorySearchPredicate
{
	string m_TargetName;
	void SP_PackagePredicate(string Name)
	{
		m_TargetName = Name;
	}

	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		SP_PackageComponent PackageComp = SP_PackageComponent.Cast(item.FindComponent(SP_PackageComponent));
		
		if (PackageComp)
		{
			string oname;
			string tname;
			string loc;
			PackageComp.GetInfo(oname, tname, loc);
			if (m_TargetName == tname)
			{
				return true;
			}
		}
		return false;
	}
}