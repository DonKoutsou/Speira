[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageDeliverBountyAction : DialogueStage
{
	[Attribute("Bounty Paper Prefab", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_Bounty;
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_ItemToGive;
	[Attribute("1", UIWidgets.EditBox, params: "1 1000", desc: "")]
	int m_WantedAmount;
	[Attribute("1", UIWidgets.EditBox, params: "1 1000", desc: "")]
	int m_RewardAmount;
	[Attribute("", UIWidgets.Coords, params: "", desc: "")]
	vector m_SpawnOffset;
	
	override void Perform(IEntity Character, IEntity Player)
	{
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(Player.FindComponent(SCR_CharacterIdentityComponent));
		id.AdjustCharRep(5);
		SP_DialogueComponent DiagComp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		DiagComp.DoAnouncerDialogue("Your reputation has improved");
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
		if (!inv)
			return;		
		
		SP_PrefabResource_Predicate pred = new SP_PrefabResource_Predicate(m_Bounty);
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
		vector mat[4];
		Character.GetWorldTransform(mat);
		vector spawnPos = m_SpawnOffset.Multiply3(mat) + mat[3];
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos; 
		Resource res = Resource.Load(m_ItemToGive);
		int RewardCount = m_RewardAmount;
		if (res)
		{
			for (int i = 0; i < RewardCount; i++)
			{
				GetGame().SpawnEntityPrefab(res, Character.GetWorld(), params);
			}
		}
		
	};
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
		if (!inv)
			return false;
		
		SP_PrefabResource_Predicate pred = new SP_PrefabResource_Predicate(m_Bounty);
		array<IEntity> entitiesToDrop = new array<IEntity>;
		inv.FindItems(entitiesToDrop, pred);
		if (entitiesToDrop.Count() < m_WantedAmount)
		{
			m_sCantBePerformedReason = "not enough";
			return false;
		}		
		return true;
	}
	override bool CanBeShown(IEntity Character, IEntity Player)
	{
		CharacterIdentityComponent CharID = CharacterIdentityComponent.Cast(Character.FindComponent(CharacterIdentityComponent));
		SCR_CharacterRankComponent CRank = SCR_CharacterRankComponent.Cast(Character.FindComponent(SCR_CharacterRankComponent));
		string rank = CRank.GetCharacterRankName(Character);
		string Name = rank + " " + CharID.GetIdentity().GetName() + " " + CharID.GetIdentity().GetSurname();
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
		array<IEntity> mybounties = new array<IEntity>();
		if (!inv)
			return false;
		else
		{
			SP_PrefabResource_Predicate pred = new SP_PrefabResource_Predicate(m_Bounty);
			array<IEntity> entitiesToDrop = new array<IEntity>;
			inv.FindItems(entitiesToDrop, pred);
			for (int i = 0; i < entitiesToDrop.Count(); i++)
			{
				SP_BountyComponent PComp = SP_BountyComponent.Cast(entitiesToDrop[i].FindComponent(SP_BountyComponent));
				if (PComp.OwnerName == Name)
				{
					mybounties.Insert(entitiesToDrop[i]);
				}
			}
		}
		if(mybounties.Count() > 0)
		{
			array<IEntity> dogtags = new array<IEntity>;
			SP_DogtagPredicate Dogpred = new SP_DogtagPredicate();
			inv.FindItems(dogtags, Dogpred);
			if(dogtags.Count() <= 0)
			{
				return false;
			}
			for (int i = 0; i < mybounties.Count(); i++)
			{
				SP_BountyComponent BComp = SP_BountyComponent.Cast(mybounties[i].FindComponent(SP_BountyComponent));
				for (int d = 0; d < dogtags.Count(); d++)
				{
					string ownername;
					DogTagEntity dogtagent = DogTagEntity.Cast(dogtags[d]);
					dogtagent.GetCname(ownername);
					if (BComp.GetTargetName() == ownername)
					{
							return true;
					}
				}
			}
		
		}
		//SCR_CharacterDamageManagerComponent ch = SCR_CharacterDamageManagerComponent.Cast(PComp.Target.FindComponent(SCR_CharacterDamageManagerComponent));
		//if(ch.GetState() == EDamageState.DESTROYED)
		return false;
	};
};