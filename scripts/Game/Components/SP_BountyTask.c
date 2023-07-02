class SP_BountyTask: SP_Task
{
	//---------------------------------------------------------------------------//
	//Entities tied to task.
	IEntity Bounty;
	//---------------------------------------------------------------------------//
	IEntity GetBountyEnt()
	{
		return Bounty;
	}
	//---------------------------------------------------------------------------//
	//Takes owner and target of task and spits out their info
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
	
	override void UpdateState()
	{
		SCR_CharacterDamageManagerComponent OwnerDmgComp = SCR_CharacterDamageManagerComponent.Cast(TaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		SCR_CharacterDamageManagerComponent TargetDmgComp = SCR_CharacterDamageManagerComponent.Cast(TaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
		if (OwnerDmgComp.IsDestroyed() && !TargetDmgComp.IsDestroyed())
		{
			e_State = ETaskState.FAILED;
			return;
		}
		if (!OwnerDmgComp.IsDestroyed() && TargetDmgComp.IsDestroyed())
		{
			e_State = ETaskState.COMPLETED;
			return;
		}
		if (OwnerDmgComp.IsDestroyed())
		{
			e_State = ETaskState.FAILED;
			return;
		}
	};
	
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		SP_NamedTagPredicate TagPred = new SP_NamedTagPredicate(Diag.GetCharacterRankName(TaskTarget) + " " + Diag.GetCharacterName(TaskTarget));
		array <IEntity> FoundTags = new array <IEntity>();
		inv.FindItems(FoundTags, TagPred);
		if (FoundTags.Count() > 0)
		{
			return true;
		}
		return false;			
	};
	
	override bool CompleteTask(IEntity Assignee)
	{
		InventoryStorageManagerComponent Assigneeinv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		InventoryStorageManagerComponent Ownerinv = InventoryStorageManagerComponent.Cast(TaskOwner.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		SP_NamedTagPredicate TagPred = new SP_NamedTagPredicate(Diag.GetCharacterRankName(TaskTarget) + " " + Diag.GetCharacterName(TaskTarget));
		array <IEntity> FoundTags = new array <IEntity>();
		Assigneeinv.FindItems(FoundTags, TagPred);
		if (FoundTags.Count() > 0)
		{
			if (GiveReward(Assignee))
			{
				InventoryItemComponent pInvComp = InventoryItemComponent.Cast(FoundTags[0].FindComponent(InventoryItemComponent));
				InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
				Assigneeinv.TryRemoveItemFromStorage(FoundTags[0],parentSlot.GetStorage());
				Ownerinv.TryInsertItem(FoundTags[0]);
				return true;
			}
		}
		return false;
	};
	override bool SetupTaskEntity()
	{
		if (TaskOwner && TaskTarget)
		{
			string OName;
			string DName;
			string DLoc;
			string OLoc;
			GetInfo(OName, DName, DLoc, OLoc);
			if (OName == " " || DName == " " || DLoc == " " || OLoc == " ")
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
				if (inv.TryInsertItem(Bounty) == false)
				{
					delete Bounty;
					return false;
				}
			}
			SP_BountyComponent BComp = SP_BountyComponent.Cast(Bounty.FindComponent(SP_BountyComponent));
			BComp.SetInfo(OName, DName, DLoc);
			TaskDesc = string.Format("%1 has put a bounty on %2's head.", OName, DName);
			TaskDiag = string.Format("I've put a bounty on %1's head, last i heard he was located on %2, get me his dogtags and i'll make it worth your while. Come back to find me on %3. Reward is %4 watches", DName, DLoc, OLoc, m_iRewardAmount);
			e_State = ETaskState.UNASSIGNED;
			return true;
		}
		return false;
	};
	override bool Init()
	{
		m_iRewardAmount = Math.RandomInt(10, 20);
		if (!TaskOwner && !TaskTarget)
		{
			SP_AIDirector MyDirector = SP_AIDirector.AllDirectors.GetRandomElement();
			if (!MyDirector)
			{
				return false;
			}
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
			if (!MyDirector.GetDirectorOccupiedByEnemy(myfact, NewDir))
			{
				return false;
			}
			Faction EnFact = NewDir.GetMajorityHolder(keyunused);
			if (!EnFact)
			{
				return false;
			}
			FactionKey Enkey = EnFact.GetFactionKey();
			if (!NewDir.GetRandomUnitByFKey(Enkey, CharToDeliverTo))
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
class SP_NamedTagPredicate : InventorySearchPredicate
{
	string m_OwnerName;
	void SP_NamedTagPredicate(string Name)
	{
		m_OwnerName = Name;
	}

	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		DogTagEntity tag = DogTagEntity.Cast(item);
		if (tag)
		{
			string TagOwnerName;
			tag.GetCname(TagOwnerName);
			if (TagOwnerName == m_OwnerName)
			{
				return true;
			}

		}
		return false;
	}
}