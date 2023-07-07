class SP_GameModeClass: SCR_BaseGameModeClass
{
};


class SP_GameMode : SCR_BaseGameMode
{
	[Attribute(UIWidgets.CheckBox, desc: "If true, it will override any previously set game over type with selected one down bellow")];
	protected bool		m_bOverrideGameOverType;
	
	[Attribute("1", UIWidgets.ComboBox, "Game Over Type", "", ParamEnumArray.FromEnum(EGameOverTypes))];
	protected EGameOverTypes			m_eOverriddenGameOverType;
	
	[Attribute("1")]
	int m_iLives;
	
	protected SP_DialogueComponent m_DialogueComponent;
	protected SP_RequestManagerComponent m_RequestManagerComponent;
	protected SCR_FactionManager m_factionManager;
	
	SP_DialogueComponent GetDialogueComponent()
	{
		return m_DialogueComponent;
	}
	
	SP_RequestManagerComponent GetRequestManagerComponent()
	{
		return m_RequestManagerComponent;
	}
	SCR_FactionManager GetFactionManager()
	{
		return m_factionManager;
	}
	override void EOnInit(IEntity owner)
	{
		// Set Test Game Flags
		#ifdef WORKBENCH
			if (GetGame().GetWorldEntity() && !GetGame().AreGameFlagsObtained())
			{
				GetGame().SetGameFlags(m_eTestGameFlags, false);
			}
		#endif

		// Find required components
        m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		m_pRespawnSystemComponent = SCR_RespawnSystemComponent.Cast(owner.FindComponent(SCR_RespawnSystemComponent));
		m_pRespawnHandlerComponent = SCR_RespawnHandlerComponent.Cast(owner.FindComponent(SCR_RespawnHandlerComponent));
		m_RespawnTimerComponent = SCR_RespawnTimerComponent.Cast(owner.FindComponent(SCR_RespawnTimerComponent));
		m_ScoringSystemComponent = SCR_BaseScoringSystemComponent.Cast(owner.FindComponent(SCR_BaseScoringSystemComponent));
		m_pGameModeHealthSettings = SCR_GameModeHealthSettings.Cast(owner.FindComponent(SCR_GameModeHealthSettings));
		m_DialogueComponent = SP_DialogueComponent.Cast(owner.FindComponent(SP_DialogueComponent));
		m_RequestManagerComponent = SP_RequestManagerComponent.Cast(owner.FindComponent(SP_RequestManagerComponent));
		m_factionManager = SCR_FactionManager.Cast(owner.FindComponent(SCR_FactionManager));

		if (!m_RplComponent)
			Print("SCR_BaseGameMode is missing RplComponent! Game functionality might be broken!", LogLevel.ERROR);
		if (!m_pRespawnSystemComponent)
			Print("SCR_BaseGameMode is missing SCR_RespawnSystemComponent! Respawning functionality might be broken!", LogLevel.ERROR);
		if (!m_pRespawnHandlerComponent)
			Print("SCR_BaseGameMode is missing SCR_RespawnHandlerComponent! Respawning functionality might be broken!", LogLevel.ERROR);

		if (!m_aAdditionalGamemodeComponents)
			m_aAdditionalGamemodeComponents = new ref array<SCR_BaseGameModeComponent>();

		array<Managed> additionalComponents = new array<Managed>();
		int count = owner.FindComponents(SCR_BaseGameModeComponent, additionalComponents);

		m_aAdditionalGamemodeComponents.Clear();
		for (int i = 0; i < count; i++)
		{
			SCR_BaseGameModeComponent comp = SCR_BaseGameModeComponent.Cast(additionalComponents[i]);
			m_aAdditionalGamemodeComponents.Insert(comp);
		}

		// Find and sort state components
		array<Managed> stateComponents = new array<Managed>();
		int stateCount = owner.FindComponents(SCR_BaseGameModeStateComponent, stateComponents);
		for (int i = 0; i < stateCount; i++)
		{
			SCR_BaseGameModeStateComponent stateComponent = SCR_BaseGameModeStateComponent.Cast(stateComponents[i]);
			SCR_EGameModeState state = stateComponent.GetAffiliatedState();
			// Invalid state
			if (state < 0)
			{
				Print("Skipping one of SCR_BaseGameStateComponent(s), invalid affiliated state!", LogLevel.ERROR);
				continue;
			}

			if (m_mStateComponents.Contains(state))
			{
				string stateName = SCR_Enum.GetEnumName(SCR_EGameModeState, state);
				Print("Skipping one of SCR_BaseGameStateComponent(s), duplicate component for state: " + stateName + "!", LogLevel.ERROR);
				continue;
			}

			m_mStateComponents.Insert(state, stateComponent);
		}
	}
	protected override void OnControllableDestroyed(IEntity entity, IEntity instigator)
	{
		super.OnControllableDestroyed(entity, instigator);
		
		
		//compare factions
		//apply penalties and adjust faction affiliation
		if(entity && instigator)
		{
			FactionAffiliationComponent FactionComp = FactionAffiliationComponent.Cast(instigator.FindComponent(FactionAffiliationComponent));
			FactionAffiliationComponent FactionCompVictim = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
			if (FactionComp && FactionCompVictim)
			{
				SCR_Faction KillerF = SCR_Faction.Cast(FactionComp.GetAffiliatedFaction());
				SCR_Faction VictimF = SCR_Faction.Cast(FactionCompVictim.GetAffiliatedFaction());
				if(VictimF.IsFactionFriendly(KillerF) == true)
				{
					if(KillerF != VictimF)
					{
						VictimF.AdjustRelation(KillerF, -5);
						SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(instigator.FindComponent(SCR_CharacterIdentityComponent));
						if (EntityUtils.IsPlayer(instigator))
						{
							SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("You caused issues between %1 and %2, your reputation has worsened", KillerF.GetFactionKey(), VictimF.GetFactionKey()));
						}
						if(id.AdjustCharRep(-20))
						{
							if (EntityUtils.IsPlayer(instigator))
							{
								SCR_HintManagerComponent.GetInstance().ShowCustom("Your reputation has fallen to much and your faction has expeled you. You'll be treated as renegade from now on");
								FactionComp.SetAffiliatedFactionByKey("RENEGADE");
							}
						}
					}
					else if (KillerF == VictimF)
					{
						SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(instigator.FindComponent(SCR_CharacterIdentityComponent));
						if(id.AdjustCharRep(-20))
						{
							if (EntityUtils.IsPlayer(instigator))
							{
								SCR_HintManagerComponent.GetInstance().ShowCustom("Your reputation has fallen to much and your faction has expeled you. You'll be treated as renegade from now on");
								FactionComp.SetAffiliatedFactionByKey("RENEGADE");
							}
						}
						else
						{
							if (EntityUtils.IsPlayer(instigator))
							{
								SCR_HintManagerComponent.GetInstance().ShowCustom("Killed unit of your own faction, your reputation is worsened by alot");
							}
						}
					}
				}
			}
		}
		
		if(m_RequestManagerComponent)
		{
			m_RequestManagerComponent.UpdateCharacterTasks(entity);
			m_RequestManagerComponent.UpdateCharacterTasks(instigator);
		}
		if (entity)
		{
			SP_CharacterAISmartActionComponent SmartComp = SP_CharacterAISmartActionComponent.Cast(entity.FindComponent(SP_CharacterAISmartActionComponent));
				if(SmartComp)
				{
					SmartComp.SetActionAccessible(true);
				}
		}
	}

	protected override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
	{
		super.OnPlayerSpawned(playerId, controlledEntity);
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(controlledEntity.FindComponent(SCR_CharacterIdentityComponent));
		id.SetCharacterRep(10);
	}
	
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
};