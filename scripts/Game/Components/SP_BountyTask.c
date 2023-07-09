[BaseContainerProps(configRoot:true)]
class SP_BountyTask: SP_Task
{
	//---------------------------------------------------------------------------//
	IEntity Bounty;
	
	[Attribute(defvalue: "20")]
	int m_iRewardAverageAmount;
	
	//------------------------------------------------------------------------------------------------------------//
	override bool FindOwner(out IEntity Owner)
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
		MyDirector.GetDirectorOccupiedByFriendly(Fact, MyDirector);
		if (!MyDirector.GetRandomUnitByFKey(Fact.GetFactionKey(), Owner))
		{
			return false;
		}
		if(Owner)
		{
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool FindTarget(out IEntity Target)
	{
		AIControlComponent comp = AIControlComponent.Cast(GetOwner().FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector MyDirector = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		if(!MyDirector)
			return false;
		
		FactionManager factionsMan = FactionManager.Cast(GetGame().GetFactionManager());
		string keyunused;
		Faction Fact = MyDirector.GetMajorityHolder(keyunused);
		if (!Fact)
			return false;
		
		SP_AIDirector NewDir;
		if (!MyDirector.GetDirectorOccupiedByEnemy(Fact, NewDir))
			return false;
		
		Faction EnFact = NewDir.GetMajorityHolder(keyunused);
		if (!EnFact)
			return false;
		
		if (!NewDir.GetRandomUnitByFKey(EnFact.GetFactionKey(), Target))
			return false;
		
		if (Target == GetOwner())
			return false;
		
		if(Target)
			return true;
		
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
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
			e_State = ETaskState.UNASSIGNED;
			return true;
		}
		return false;
	};
	override void CreateDescritions()
	{
		string OName;
		string DName;
		string DLoc;
		string OLoc;
		GetInfo(OName, DName, OLoc, DLoc);
		TaskDesc = string.Format("%1 has put a bounty of %3 %4 on %2's head.", OName, DName, m_iRewardAmount, FilePath.StripPath(reward));
		TaskDiag = string.Format("I've put a bounty on %1's head, last i heard he was located on %2, get me his dogtags and i'll make it worth your while. Come back to find me on %3. Reward is %4 %5", DName, DLoc, OLoc, m_iRewardAmount, FilePath.StripPath(reward));
		TaskTitle = string.Format("Bounty: retrieve %1's dogtags", DName);
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		if(!CharacterAssigned(Assignee))
		{
			return false;
		}
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SP_NamedTagPredicate TagPred = new SP_NamedTagPredicate(Diag.GetCharacterRankName(TaskTarget) + " " + Diag.GetCharacterName(TaskTarget));
		array <IEntity> FoundTags = new array <IEntity>();
		inv.FindItems(FoundTags, TagPred);
		if (FoundTags.Count() > 0)
		{
			return true;
		}
		return false;			
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool AssignReward()
	{
		EEditableEntityLabel RewardLabel;
		int index = Math.RandomInt(0,2);
		if(index == 0)
		{
			RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
			SP_RequestManagerComponent ReqMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			if(!ReqMan)
			{
				return false;
			}
			SP_BountyTask tasksample = SP_BountyTask.Cast(ReqMan.GetTaskSample(SP_BountyTask));
			if(!tasksample)
			{
				return false;
			}
			m_iRewardAverageAmount = tasksample.GetRewardAverage();
			if(m_iRewardAverageAmount)
			{
				m_iRewardAmount = Math.Floor(Math.RandomFloat(m_iRewardAverageAmount/2, m_iRewardAverageAmount + m_iRewardAverageAmount/2));
			}
			else
			{
				m_iRewardAmount = Math.RandomInt(5, 15)
			}
		}
		if(index == 1)
		{
			RewardLabel = EEditableEntityLabel.ITEMTYPE_WEAPON;
			m_iRewardAmount = 1;
		}
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REWARD);
		array<SCR_EntityCatalogEntry> Mylist = new array<SCR_EntityCatalogEntry>();
		RequestCatalog.GetEntityListWithLabel(RewardLabel, Mylist);
		SCR_EntityCatalogEntry entry = Mylist.GetRandomElement();
		reward = entry.GetPrefab();
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	override bool CompleteTask(IEntity Assignee)
	{
		InventoryStorageManagerComponent Assigneeinv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		InventoryStorageManagerComponent Ownerinv = InventoryStorageManagerComponent.Cast(TaskOwner.FindComponent(InventoryStorageManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		SP_NamedTagPredicate TagPred = new SP_NamedTagPredicate(Diag.GetCharacterRankName(TaskTarget) + " " + Diag.GetCharacterName(TaskTarget));
		array <IEntity> FoundTags = new array <IEntity>();
		Assigneeinv.FindItems(FoundTags, TagPred);
		m_TaskMarker.Finish(true);
		delete m_TaskMarker;
		if (FoundTags.Count() > 0)
		{
			if (GiveReward(Assignee))
			{
				InventoryItemComponent pInvComp = InventoryItemComponent.Cast(FoundTags[0].FindComponent(InventoryItemComponent));
				InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
				Assigneeinv.TryRemoveItemFromStorage(FoundTags[0],parentSlot.GetStorage());
				Ownerinv.TryInsertItem(FoundTags[0]);
				if (m_TaskMarker)
				{
					m_TaskMarker.Finish(true);
					delete m_TaskMarker;
				}
				SP_FactionManager factman = SP_FactionManager.Cast(GetGame().GetFactionManager());
				factman.OnTaskCompleted(this, Assignee);
				e_State = ETaskState.COMPLETED;
				delete Bounty;
				m_Copletionist = Assignee;
				return true;
			}
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	void GetInfo(out string OName, out string DName, out string OLoc, out string DLoc)
	{
		if (!TaskOwner || !TaskTarget)
		{
			return;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		AIControlComponent comp = AIControlComponent.Cast(TaskTarget.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector Director = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(TaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = CharRank.GetCharacterRankName(TaskOwner) + " " + Diag.GetCharacterName(TaskOwner);
		DName = CharRank.GetCharacterRankName(TaskTarget) + " " + Diag.GetCharacterName(TaskTarget);
		OLoc = Director.GetCharacterLocation(TaskOwner);
		DLoc = Director.GetCharacterLocation(TaskTarget);
	};
	//------------------------------------------------------------------------------------------------------------//
	override void UpdateState()
	{
		SCR_CharacterDamageManagerComponent OwnerDmgComp = SCR_CharacterDamageManagerComponent.Cast(TaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		SCR_CharacterDamageManagerComponent TargetDmgComp = SCR_CharacterDamageManagerComponent.Cast(TaskTarget.FindComponent(SCR_CharacterDamageManagerComponent));
		if (OwnerDmgComp.IsDestroyed() && !TargetDmgComp.IsDestroyed())
		{
			if(m_TaskMarker)
			{
				m_TaskMarker.Fail(true);
				delete m_TaskMarker;
			}
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
			if(m_TaskMarker)
			{
				m_TaskMarker.Fail(true);
				delete m_TaskMarker;
			}
			e_State = ETaskState.FAILED;
			return;
		}
	};
	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_BountyTask;};
	//------------------------------------------------------------------------------------------------------------//
	IEntity GetBountyEnt(){return Bounty;};
	//------------------------------------------------------------------------------------------------------------//
	override void DeleteLeftovers()
	{
		if(Bounty)
		{
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(Bounty.FindComponent(InventoryItemComponent));
			InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
			if(parentSlot)
			{
				SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(TaskOwner.FindComponent(SCR_InventoryStorageManagerComponent));
				if(inv)
				{
					inv.TryRemoveItemFromStorage(Bounty,parentSlot.GetStorage());
					delete Bounty;
				}
			}
		}
		if(Bounty)
		{
			delete Bounty;
		}
	};
	//------------------------------------------------------------------------------------------------------------//
	int GetRewardAverage()
	{
		if (m_iRewardAverageAmount)
		{
			return m_iRewardAverageAmount;
		}
		return null;
	};
};
//------------------------------------------------------------------------------------------------------------//
class SP_NamedTagPredicate : InventorySearchPredicate
{
	string m_OwnerName;
	//------------------------------------------------------------------------------------------------------------//
	void SP_NamedTagPredicate(string Name){m_OwnerName = Name;};
	//------------------------------------------------------------------------------------------------------------//
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
//------------------------------------------------------------------------------------------------------------//