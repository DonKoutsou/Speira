class SP_RetrieveTask: SP_Task
{
	IEntity ItemBounty;
	int m_iRequestedAmount;
	SCR_EArsenalItemType	m_requestitemtype;
	SCR_EArsenalItemMode	m_requestitemmode;
	IEntity GetItemBountyEnt()
	{
		return ItemBounty;
	}
	void GetInfo(out string OName, out string OLoc)
	{
		if (!TaskOwner)
		{
			return;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		AIControlComponent comp = AIControlComponent.Cast(TaskOwner.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector Director = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(TaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = CharRank.GetCharacterRankName(TaskOwner) + " " + Diag.GetCharacterName(TaskOwner);
		OLoc = Director.GetCharacterLocation(TaskOwner);
	};
	override void UpdateState()
	{
		SCR_CharacterDamageManagerComponent OwnerDmgComp = SCR_CharacterDamageManagerComponent.Cast(TaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (OwnerDmgComp.IsDestroyed())
		{
			e_State = ETaskState.FAILED;
			return;
		}
	};
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		if (TalkingChar != TaskOwner)
		{
			return false;
		}
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Assignee.FindComponent(InventoryStorageManagerComponent));
		SP_RequestPredicate RequestPred = new SP_RequestPredicate(m_requestitemtype, m_requestitemmode);
		array <IEntity> FoundItems = new array <IEntity>();
		inv.FindItems(FoundItems, RequestPred);
		if (FoundItems.Count() >= m_iRequestedAmount)
		{
			return true;
		}
		return false;			
	};
	override bool CompleteTask(IEntity Assignee)
	{
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(Assignee.FindComponent(SCR_InventoryStorageManagerComponent));
		SCR_InventoryStorageManagerComponent Ownerinv = SCR_InventoryStorageManagerComponent.Cast(TaskOwner.FindComponent(SCR_InventoryStorageManagerComponent));
		SP_RequestPredicate RequestPred = new SP_RequestPredicate(m_requestitemtype, m_requestitemmode);
		array <IEntity> FoundItems = new array <IEntity>();
		inv.FindItems(FoundItems, RequestPred);
		if (FoundItems.Count() >= m_iRequestedAmount)
		{
			if (GiveReward(Assignee))
			{
				foreach (IEntity item : FoundItems)
				{
					InventoryItemComponent pInvComp = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
					InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
					inv.TryRemoveItemFromStorage(item,parentSlot.GetStorage());
					if(m_requestitemtype == SCR_EArsenalItemType.HEADWEAR)
					{
						Ownerinv.EquipCloth(item);
					}
					else
					{
						Ownerinv.TryInsertItem(item);
					}
					e_State = ETaskState.COMPLETED;
					return true;
				}
				
			}
		}
		return false;
	};
	override bool Init()
	{
		m_iRewardAmount = Math.RandomInt(2, 4);
		m_iRequestedAmount = Math.RandomInt(1, 10);
		if (!TaskOwner)
		{
			SP_AIDirector MyDirector = SP_AIDirector.AllDirectors.GetRandomElement();
			if (!MyDirector)
			{
				return false;
			}
			IEntity Character;
			if (!MyDirector.GetRandomUnit(Character))
			{
				return false;
			}
			if (!Character)
			{
				return false;
			}
			SetInfo(Character, null);
		}
		if (SetupTaskEntity())
		{
			return true;
		}
		return false;
	};
	override bool SetupTaskEntity()
	{
		if (TaskOwner)
		{
			string OName;
			string OLoc;
			GetInfo(OName, OLoc);
			if (OName == " " || OLoc == " ")
			{
				return false;
			}
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = vector.Zero;
			Resource res = Resource.Load("{A99880B995D4588F}prefabs/Items/ItemBountyPaper.et");
			if (res)
			{
				ItemBounty = GetGame().SpawnEntityPrefab(res, GetGame().GetWorld(), params);
				InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(TaskOwner.FindComponent(InventoryStorageManagerComponent));
				if (inv.TryInsertItem(ItemBounty) == false)
				{
					delete ItemBounty;
					return false;
				}
			}
			SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
			SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
			int index = Math.RandomInt(0, 5);
			if(index == 0)
			{
				m_requestitemtype = SCR_EArsenalItemType.HEAL;
				m_requestitemmode = SCR_EArsenalItemMode.CONSUMABLE;
			}
			if(index == 1)
			{
				m_requestitemtype = SCR_EArsenalItemType.FOOD;
				m_requestitemmode = SCR_EArsenalItemMode.CONSUMABLE;
			}
			if(index == 2)
			{
				m_requestitemtype = SCR_EArsenalItemType.DRINK;
				m_requestitemmode = SCR_EArsenalItemMode.CONSUMABLE;
			}
			if(index == 3)
			{
				m_requestitemtype = SCR_EArsenalItemType.EXPLOSIVES;
				m_requestitemmode = SCR_EArsenalItemMode.WEAPON;
			}
			if(index == 4)
			{
				EquipedLoadoutStorageComponent loadoutStorage = EquipedLoadoutStorageComponent.Cast(TaskOwner.FindComponent(EquipedLoadoutStorageComponent));
				if (!loadoutStorage)
					return false;
				
				IEntity Helmet = loadoutStorage.GetClothFromArea(LoadoutHeadCoverArea);
				if (!Helmet)
				{
					return false;
				}
				EntityPrefabData prefabData = Helmet.GetPrefabData();
				ResourceName prefabName = prefabData.GetPrefabName();
				SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefabName);
				if(!entry)
				{
					return false;
				}
				SCR_InventoryStorageManagerComponent storeman = SCR_InventoryStorageManagerComponent.Cast(TaskOwner.FindComponent(SCR_InventoryStorageManagerComponent));
				if (!storeman)
				{
					return false;
				}
				storeman.TryRemoveItemFromInventory(Helmet);
				delete Helmet;
				m_requestitemtype = SCR_EArsenalItemType.HEADWEAR;
				m_requestitemmode = SCR_EArsenalItemMode.ATTACHMENT;
				m_iRequestedAmount = 1;
				m_iRewardAmount = 10;
			}
			SP_ItemBountyComponent IBComp = 	SP_ItemBountyComponent.Cast(ItemBounty.FindComponent(	SP_ItemBountyComponent));
			m_iRewardAmount = m_iRewardAmount * m_iRequestedAmount;
			IBComp.SetInfo(OName, m_requestitemtype, m_requestitemmode, EEditableEntityLabel.ITEMTYPE_ITEM, m_iRequestedAmount, OLoc);
			string itemdesc = typename.EnumToString(SCR_EArsenalItemType, m_requestitemtype) + " " + typename.EnumToString(SCR_EArsenalItemMode, m_requestitemmode);
			TaskDesc = string.Format("%1 is looking for %2 %3.", OName, m_iRequestedAmount.ToString(), itemdesc);
			TaskDiag = string.Format("I'm looking for %1 %2. Come back to find me on %3. Reward is %4 watches", m_iRequestedAmount.ToString(), itemdesc, OLoc, m_iRewardAmount);
			e_State = ETaskState.UNASSIGNED;
			return true;
		}
		return false;
	};
};
class SP_RequestPredicate : InventorySearchPredicate
{
	SCR_EArsenalItemType	m_requestitemtype;
	SCR_EArsenalItemMode	m_requestitemmode;
	void SP_RequestPredicate(SCR_EArsenalItemType type, SCR_EArsenalItemMode mode)
	{
		m_requestitemtype = type;
		m_requestitemmode = mode;
	}
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		EntityPrefabData prefabData = item.GetPrefabData();
		ResourceName prefabName = prefabData.GetPrefabName();
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REQUEST);
		SCR_EntityCatalogEntry entry = RequestCatalog.GetEntryWithPrefab(prefabName);
		if(!entry)
		{
			return false;
		}
		SCR_ArsenalItem arsenaldata = SCR_ArsenalItem.Cast(entry.GetEntityDataOfType(SCR_ArsenalItem));
		if (arsenaldata.GetItemType() == m_requestitemtype && arsenaldata.GetItemMode() == m_requestitemmode)
		{
			return true;
		}
		return false;
	}
}