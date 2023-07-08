//------------------------------------------------------------------//
class SP_GameModeClass: SCR_BaseGameModeClass
{
};
//------------------------------------------------------------------//
class SP_GameMode : SCR_BaseGameMode
{
	//------------------------------------------------------------------//
	[Attribute(UIWidgets.CheckBox, desc: "If true, it will override any previously set game over type with selected one down bellow")];
	protected bool		m_bOverrideGameOverType;
	//------------------------------------------------------------------//
	[Attribute("1", UIWidgets.ComboBox, "Game Over Type", "", ParamEnumArray.FromEnum(EGameOverTypes))];
	protected EGameOverTypes			m_eOverriddenGameOverType;
	//------------------------------------------------------------------//
	[Attribute("1")]
	int m_iLives;
	//------------------------------------------------------------------//
	protected SP_DialogueComponent m_DialogueComponent;
	protected SP_RequestManagerComponent m_RequestManagerComponent;
	protected SP_FactionManager m_factionManager;
	//------------------------------------------------------------------//
	SP_DialogueComponent GetDialogueComponent()
	{
		return m_DialogueComponent;
	}
	//------------------------------------------------------------------//
	SP_RequestManagerComponent GetRequestManagerComponent()
	{
		return m_RequestManagerComponent;
	}
	//------------------------------------------------------------------//
	SP_FactionManager GetFactionManager()
	{
		return m_factionManager;
	}
	//------------------------------------------------------------------//
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		m_DialogueComponent = SP_DialogueComponent.Cast(owner.FindComponent(SP_DialogueComponent));
		m_RequestManagerComponent = SP_RequestManagerComponent.Cast(owner.FindComponent(SP_RequestManagerComponent));
		m_factionManager = SP_FactionManager.Cast(GetGame().GetFactionManager());
	}
	//------------------------------------------------------------------//
	protected override void OnControllableDestroyed(IEntity entity, IEntity instigator)
	{
		super.OnControllableDestroyed(entity, instigator);
		//compare factions
		//apply penalties and adjust faction affiliation
		if(entity && instigator)
		{
			//----------------------------------------------//
			//affiliation components
			FactionAffiliationComponent FactionComp = FactionAffiliationComponent.Cast(instigator.FindComponent(FactionAffiliationComponent));
			FactionAffiliationComponent FactionCompVictim = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
			if (FactionComp && FactionCompVictim)
			{
				//----------------------------------------------//
				//actual faction
				SCR_Faction KillerF = SCR_Faction.Cast(FactionComp.GetAffiliatedFaction());
				SCR_Faction VictimF = SCR_Faction.Cast(FactionCompVictim.GetAffiliatedFaction());
				//----------------------------------------------//
				//compare factions and apply penalties
				if (VictimF.IsFactionFriendly(KillerF) == true)
				{
					if (KillerF != VictimF)
					{
						m_factionManager.OnFriendlyKill(entity, instigator);
					}
					else if (KillerF == VictimF)
					{
						m_factionManager.OnSameFactionKill(entity, instigator);
					}
				}
			}
		}
		//------------------------------------------------------------------//
		//update tasks
		if(m_RequestManagerComponent)
		{
			m_RequestManagerComponent.UpdateCharacterTasks(entity);
			m_RequestManagerComponent.UpdateCharacterTasks(instigator);
		}
		//------------------------------------------------------------------//
		//mark them lootable
		if (entity)
		{
			SP_CharacterAISmartActionComponent SmartComp = SP_CharacterAISmartActionComponent.Cast(entity.FindComponent(SP_CharacterAISmartActionComponent));
				if(SmartComp)
				{
					SmartComp.SetActionAccessible(true);
				}
		}
	}
	//------------------------------------------------------------------//
	protected override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
	{
		super.OnPlayerSpawned(playerId, controlledEntity);
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(controlledEntity.FindComponent(SCR_CharacterIdentityComponent));
		id.SetCharacterRep(10);
	}
	//------------------------------------------------------------------//
	protected override void OnPlayerKilled(int playerId, IEntity player, IEntity killer)
	{
		super.OnPlayerKilled(playerId, player, killer);
		m_iLives -= 1;
		if(m_iLives == 0)
		{
			SCR_GameModeSFManager manager = SCR_GameModeSFManager.Cast(GetGame().GetGameMode().FindComponent(SCR_GameModeSFManager));
			if (!manager)
				return;
			
			if (m_bOverrideGameOverType)
				manager.SetMissionEndScreen(m_eOverriddenGameOverType);
			
			manager.Finish();
		}
	}
	//------------------------------------------------------------------//
};
//------------------------------------------------------------------//
modded enum EGameOverTypes
{
	DEATH = 3000,
	BROTHERFOUND = 3001,
}
//------------------------------------------------------------------//