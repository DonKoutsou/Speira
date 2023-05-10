modded enum EGameOverTypes
{
	DEATH = 3000,
	BROTHERFOUND = 3001,
}
class SP_UnretrievableComponentClass : GameComponentClass
{
};

class SP_UnretrievableComponent : GameComponent
{
	[Attribute("0")]
	bool b_ShouldRetrieve;
	
	bool ShouldRetrieve()
	{
		return b_ShouldRetrieve;
	}
};
class SP_PermadeathRespawnHandlerComponentClass: SCR_AutomaticRespawnHandlerComponentClass
{
};

class SP_PermadeathRespawnHandlerComponent : SCR_AutomaticRespawnHandlerComponent
{
	[Attribute(UIWidgets.CheckBox, desc: "If true, it will override any previously set game over type with selected one down bellow")];
	protected bool		m_bOverrideGameOverType;
	
	[Attribute("1", UIWidgets.ComboBox, "Game Over Type", "", ParamEnumArray.FromEnum(EGameOverTypes))];
	protected EGameOverTypes			m_eOverriddenGameOverType;
	
	[Attribute("", UIWidgets.Coords, params: "inf inf inf purpose=coords space=world", desc: "")]
	vector m_FIAStorageWorldPosition;
	
	[Attribute("", UIWidgets.Coords, params: "inf inf inf purpose=coords space=world", desc: "")]
	vector m_BanditStorageWorldPosition;
	
	[Attribute("", UIWidgets.Coords, params: "inf inf inf purpose=coords space=world", desc: "")]
	vector m_RespawnWorldPosition;
	
	[Attribute("", UIWidgets.Coords, params: "inf inf inf purpose=coords space=world", desc: "")]
	vector m_RespawnWorldPositionBandit;
	
	[Attribute("")]
	ResourceName m_StoragePreset;
	
	[Attribute("")]
	ResourceName m_SpawnpointFIA;
	
	[Attribute("")]
	ResourceName m_SpawnpointSPEIRA;
	
	[Attribute("")]
	ResourceName m_SpawnpointBandit;
	
	[Attribute()]
	int m_iLives;
	
	IEntity DeathStorageFIA;
	
	IEntity DeathStorageBandit;
	
	bool m_bDonewithspanpoints = false;
	
	override void OnPlayerKilled(int playerId, IEntity player, IEntity killer)
	{
		FactionAffiliationComponent factcompKiller = FactionAffiliationComponent.Cast(killer.FindComponent(FactionAffiliationComponent));
		FactionKey FactKiller = factcompKiller.GetAffiliatedFaction().GetFactionKey();
		if (m_SpawnpointFIA && m_SpawnpointSPEIRA && m_bDonewithspanpoints == false)
		{
			SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			SCR_Faction BanditFaction = SCR_Faction.Cast(factionManager.GetFactionByKey("BANDITS"));
			SCR_Faction SPEIRAFaction = SCR_Faction.Cast(factionManager.GetFactionByKey("SPEIRA"));
			factionManager.SetFactionsFriendly(BanditFaction, SPEIRAFaction);
			if (FactKiller == "BANDITS")
			{
				EntitySpawnParams spawnParams = EntitySpawnParams();
				spawnParams.TransformMode = ETransformMode.WORLD;
				spawnParams.Transform[3] = m_RespawnWorldPositionBandit;
				Resource spawnSPEIRA = Resource.Load(m_SpawnpointSPEIRA);
				GetGame().SpawnEntityPrefab(spawnSPEIRA, GetGame().GetWorld(), spawnParams);
			}
			else
			{
				EntitySpawnParams spawnParams = EntitySpawnParams();
				spawnParams.TransformMode = ETransformMode.WORLD;
				spawnParams.Transform[3] = m_RespawnWorldPosition;
				Resource spawnSPEIRA = Resource.Load(m_SpawnpointSPEIRA);
				GetGame().SpawnEntityPrefab(spawnSPEIRA, GetGame().GetWorld(), spawnParams);
			}
			m_bDonewithspanpoints = true;
		}
		
		
		if (m_iLives != 0)
		{
			SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
			array<IEntity> items = new array<IEntity>();
			inv.GetItems(items);
			if (items && DeathStorageFIA && DeathStorageBandit)
			{
				InventoryStorageManagerComponent stocomp;
				SP_DialogueComponent DiagComp = SP_DialogueComponent.Cast(GetGameMode().FindComponent(SP_DialogueComponent));
				if (FactKiller == "BANDITS")
				{
					stocomp = InventoryStorageManagerComponent.Cast(DeathStorageBandit.FindComponent(InventoryStorageManagerComponent));
					DiagComp.DoAnouncerDialogue("The bandits mistekenly atacked you, after IDing you the brought you back to their hideout and healed your wounds.");
				}
				else
				{
					stocomp = InventoryStorageManagerComponent.Cast(DeathStorageFIA.FindComponent(InventoryStorageManagerComponent));
					DiagComp.DoAnouncerDialogue("The local FIA has saved you from death. Your items are stored on the chest next to the bed.");
				}
				
				InventoryStorageManagerComponent stocompKiller = InventoryStorageManagerComponent.Cast(killer.FindComponent(InventoryStorageManagerComponent));
				foreach (IEntity item : items)
				{
					SP_UnretrievableComponent Unretr = SP_UnretrievableComponent.Cast(item.FindComponent(SP_UnretrievableComponent));
					if (!Unretr)
					{
						inv.TryMoveItemToStorage(item, stocomp.FindStorageForItem(item));
					}
					else
					{
						inv.TryMoveItemToStorage(item, stocompKiller.FindStorageForItem(item));
					}
				}
			}
			m_iForcedLoadout = 1;
			
			super.OnPlayerKilled(playerId, player, killer);
			m_iLives = m_iLives - 1;
			return;
		}
		
		SCR_GameModeSFManager manager = SCR_GameModeSFManager.Cast(GetGame().GetGameMode().FindComponent(SCR_GameModeSFManager));
		if (!manager)
			return;
		
		if (m_bOverrideGameOverType)
			manager.SetMissionEndScreen(m_eOverriddenGameOverType);
		
		manager.Finish();

		
	}
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		if (m_SpawnpointBandit)
		{
			EntitySpawnParams spawnParams = EntitySpawnParams();
			spawnParams.TransformMode = ETransformMode.WORLD;
			spawnParams.Transform[3] = m_RespawnWorldPositionBandit;
			Resource spawnBANDIT = Resource.Load(m_SpawnpointBandit);
			GetGame().SpawnEntityPrefab(spawnBANDIT, GetGame().GetWorld(), spawnParams);
		}
		if (m_SpawnpointFIA)
		{
			EntitySpawnParams spawnParams = EntitySpawnParams();
			spawnParams.TransformMode = ETransformMode.WORLD;
			spawnParams.Transform[3] = m_RespawnWorldPosition;
			Resource spawnFIA = Resource.Load(m_SpawnpointBandit);
			GetGame().SpawnEntityPrefab(spawnFIA, GetGame().GetWorld(), spawnParams);
		}
		if (m_StoragePreset)
		{
			EntitySpawnParams spawnParams = EntitySpawnParams();
			spawnParams.TransformMode = ETransformMode.WORLD;
			spawnParams.Transform[3] = m_FIAStorageWorldPosition;
			Resource storage = Resource.Load(m_StoragePreset);
			DeathStorageFIA = GetGame().SpawnEntityPrefab(storage, GetGame().GetWorld(), spawnParams);
			EntitySpawnParams spawnParamsBand = EntitySpawnParams();
			spawnParamsBand.TransformMode = ETransformMode.WORLD;
			spawnParamsBand.Transform[3] = m_BanditStorageWorldPosition;
			DeathStorageBandit = GetGame().SpawnEntityPrefab(storage, GetGame().GetWorld(), spawnParamsBand);
			
		}
	}
}