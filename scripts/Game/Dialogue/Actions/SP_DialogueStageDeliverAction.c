class SP_ConsumablePredicate : InventorySearchPredicate
{
	EConsumableType m_ConsumableFilter;
	
	void SP_ConsumablePredicate(EConsumableType type)
	{
		QueryComponentTypes.Insert(SCR_ConsumableItemComponent);
		m_ConsumableFilter = type;
	}

	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		return (SCR_ConsumableItemComponent.Cast(queriedComponents[0])).GetConsumableType() == m_ConsumableFilter);
	}
}

class SP_PrefabResource_Predicate : InventorySearchPredicate
{
	ResourceName m_WantedPrefabName;
	void SP_PrefabResource_Predicate(ResourceName prefabName)
	{
		m_WantedPrefabName = prefabName;
	}

	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		return item.GetPrefabData().GetPrefabName() == m_WantedPrefabName;
	}
}
[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageDeliverAction : DialogueStage
{
	[Attribute("Item needed to be delivered", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_WantedItem;
	[Attribute("1", UIWidgets.EditBox, params: "1 1000", desc: "")]
	int m_WantedAmount;
	
	override void Perform(IEntity Character, IEntity Player)
	{
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
		if (!inv)
			return;		
		
		SP_PrefabResource_Predicate pred = new SP_PrefabResource_Predicate(m_WantedItem);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		inv.FindItems(entitiesToDrop, pred);
		
		int movedCount = 0;
		ref array<IEntity> givenItems = {};
		
		for (int i = 0; i < entitiesToDrop.Count(); i++)
		{
			IEntity item = entitiesToDrop[i];
			if (!item) continue;
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
			bool removed = inv.TryRemoveItemFromStorage(item,parentSlot.GetStorage());
			if (removed)
			{
				movedCount++;				
				givenItems.Insert(item);
				
				if (movedCount >= m_WantedAmount)
					break;
			}
		}				
		
		// todo: what to do next
		if (movedCount >= m_WantedAmount)
		{	
			for (int i = 0; i < movedCount; i++)
			{			
				delete givenItems.Get(i);
			}
		}
		givenItems.Clear();;
		
	};
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
		if (!inv)
			return false;
		
		SP_PrefabResource_Predicate pred = new SP_PrefabResource_Predicate(m_WantedItem);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		inv.FindItems(entitiesToDrop, pred);
		if (entitiesToDrop.Count() < m_WantedAmount)
		{
			m_sCantBePerformedReason = "not enough";
			return false;
		}		
		return true;
	}

};