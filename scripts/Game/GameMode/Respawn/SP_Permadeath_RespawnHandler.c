modded enum EGameOverTypes
{
	DEATH,
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
	vector m_StorageWorldPosition;
	
	[Attribute("", UIWidgets.Coords, params: "inf inf inf purpose=coords space=world", desc: "")]
	vector m_RespawnWorldPosition;
	
	[Attribute("")]
	ResourceName m_StoragePreset;
	
	[Attribute("")]
	ResourceName m_SpawnpointFIA;
	
	[Attribute("")]
	ResourceName m_SpawnpointSPEIRA;
	
	[Attribute()]
	int m_iLives;
	
	IEntity DeathStorage;
	
	override void OnPlayerKilled(int playerId, IEntity player, IEntity killer)
	{
		if (m_iLives != 0)
		{
			SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
			array<IEntity> items = new array<IEntity>();
			inv.GetItems(items);
			if (items && DeathStorage)
			{
				InventoryStorageManagerComponent stocomp = InventoryStorageManagerComponent.Cast(DeathStorage.FindComponent(InventoryStorageManagerComponent));
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
		if (m_SpawnpointFIA && m_SpawnpointSPEIRA)
		{
			EntitySpawnParams spawnParams = EntitySpawnParams();
			spawnParams.TransformMode = ETransformMode.WORLD;
			spawnParams.Transform[3] = m_RespawnWorldPosition;
			Resource spawnFIA = Resource.Load(m_SpawnpointFIA);
			Resource spawnSPEIRA = Resource.Load(m_SpawnpointSPEIRA);
			GetGame().SpawnEntityPrefab(spawnFIA, GetGame().GetWorld(), spawnParams);
			GetGame().SpawnEntityPrefab(spawnSPEIRA, GetGame().GetWorld(), spawnParams);
		}
		EntitySpawnParams spawnParams = EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform[3] = m_StorageWorldPosition;
		Resource storage = Resource.Load(m_StoragePreset);
		
		DeathStorage = GetGame().SpawnEntityPrefab(storage, GetGame().GetWorld(), spawnParams);
		
	}
}