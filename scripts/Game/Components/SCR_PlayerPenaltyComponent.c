[EntityEditorProps(category: "GameScripted/GameMode", description: "Takes care of player penalties, kicks, bans etc.", color: "0 0 255 255")]
class SCR_PlayerPenaltyComponentClass: SCR_BaseGameModeComponentClass
{
};

//------------------------------------------------------------------------------------------------
class SCR_PlayerPenaltyComponent: SCR_BaseGameModeComponent
{
	[Attribute("0", desc: "Penalty score for killing a friendly player.")]
	protected int m_iFriendlyPlayerKillPenalty;
	
	[Attribute("0", desc: "Penalty score for killing a friendly AI.")]
	protected int m_iFriendlyAIKillPenalty;
	
	//[Attribute("0", desc: "Penalty score limit for a kick from the match.")]
	//protected int m_iKickPenaltyLimit;
	
	[Attribute("0", desc: "Ban duration after a kick (in seconds, -1 for a session-long ban).")]
	protected int m_iBanDuration;
	
	[Attribute("0", desc: "How often penalty score substraction happens (in seconds).")]
	protected int m_iPenaltySubstractionPeriod;
	
	[Attribute("0", desc: "How many penalty points get substracted after each substraction period.")]
	protected int m_iPenaltySubstractionPoints;
	
	protected static SCR_PlayerPenaltyComponent s_Instance;
	protected static const int EVALUATION_PERIOD = 1000;
	
	protected RplComponent m_RplComponent;
	protected ref array<ref SCR_PlayerPenaltyData> m_aPlayerPenaltyData = {};
	protected ref SCR_PlayerPenaltyDSSessionCallback m_Callback;
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerConnected(int playerId)
	{
		if (IsProxy())
			return;
		
		SCR_PlayerPenaltyData playerPenaltyData = GetPlayerPenaltyData(playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnControllableDestroyed(IEntity entity, IEntity instigator)
	{
		if (instigator == GetGame().GetPlayerController().GetControlledEntity())
		{
			if (entity && instigator)
			{
				FactionAffiliationComponent FactionComp = FactionAffiliationComponent.Cast(instigator.FindComponent(FactionAffiliationComponent));
				FactionAffiliationComponent FactionCompVictim = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
				if (FactionComp && FactionCompVictim)
				{
					FactionKey KillerKey = FactionComp.GetAffiliatedFaction().GetFactionKey();
					FactionKey VictimKey = FactionCompVictim.GetAffiliatedFaction().GetFactionKey();
					SP_DialogueComponent DiagComp = SP_DialogueComponent.Cast(GetGameMode().FindComponent(SP_DialogueComponent));
					if (KillerKey == "SPEIRA" && VictimKey == "USSR")
					{
						DiagComp.DoAnouncerDialogue("Killed unit of the soviet faction, you will be considered as part of the FIA from now on");
						FactionComp.SetAffiliatedFactionByKey("FIA");
					}
					else if (KillerKey == "SPEIRA" && VictimKey == "FIA")
					{
						DiagComp.DoAnouncerDialogue("Killed unit of the FIA faction, you will be considered Renegade from now on");
						FactionComp.SetAffiliatedFactionByKey("RENEGADE");
					}
					else if (KillerKey == VictimKey)
					{
						DiagComp.DoAnouncerDialogue("Killed unit of your own faction, you've gone Renegade");
						FactionComp.SetAffiliatedFactionByKey("RENEGADE");
					}
				}
				
			}
		}
		if (IsProxy())
			return;
		
		//if (m_iFriendlyAIKillPenalty == 0 && m_iFriendlyPlayerKillPenalty == 0)
		//	return;
		
		if (!instigator)
			return;
		
		if (entity == instigator)
			return;
		
		SCR_ChimeraCharacter victimChar = SCR_ChimeraCharacter.Cast(entity);
		
		if (!victimChar)     
			return;
		
		SCR_ChimeraCharacter killerChar;
		
		// Instigator is a vehicle, find the driver
		if (instigator.IsInherited(Vehicle))
		{
			killerChar = GetInstigatorFromVehicle(instigator)
		}
		else
		{
			// Check if the killer is a regular soldier on foot
			killerChar = SCR_ChimeraCharacter.Cast(instigator);
			
			// If all else fails, check if the killer is in a vehicle turret
			if (!killerChar)
				killerChar = GetInstigatorFromVehicle(instigator, true)
		}
		
		if (!killerChar || entity == killerChar)
			return;
		
		int killerPlayerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(killerChar);
		
		//if (killerPlayerId == 0)
		//	return;
		
		//if (!killerChar.GetFaction().IsFactionFriendly(victimChar.GetFaction()))
		//	return;
		
		//int victimPlayerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(victimChar);
		
		//if (victimPlayerId == 0)
		//	AddPenaltyScore(killerPlayerId, m_iFriendlyAIKillPenalty);
		//else
		//	AddPenaltyScore(killerPlayerId, m_iFriendlyPlayerKillPenalty);
		
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(entity.FindComponent(SCR_InventoryStorageManagerComponent));
		array<IEntity> items = new array<IEntity>();
		inv.GetItems(items);
		InventoryStorageManagerComponent stocompKiller = InventoryStorageManagerComponent.Cast(instigator.FindComponent(InventoryStorageManagerComponent));
		foreach (IEntity item : items)
		{
		SP_UnretrievableComponent Unretr = SP_UnretrievableComponent.Cast(item.FindComponent(SP_UnretrievableComponent));
			if (Unretr)
				{
					inv.TryMoveItemToStorage(item, stocompKiller.FindStorageForItem(item));
				}
		}
			
	}
	
	//------------------------------------------------------------------------------------------------
	static SCR_PlayerPenaltyComponent GetInstance()
	{
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnBanResult(int iPlayerId, bool triggered, int currentValue, int triggerValue)
	{
		if (triggered)
			KickPlayer(iPlayerId, m_iBanDuration, SCR_PlayerManagerKickReason.FRIENDLY_FIRE);
	}
	
	//------------------------------------------------------------------------------------------------
	static SCR_ChimeraCharacter GetInstigatorFromVehicle(IEntity veh, bool gunner = false)
	{
		BaseCompartmentManagerComponent compartmentManager = BaseCompartmentManagerComponent.Cast(veh.FindComponent(BaseCompartmentManagerComponent));
			
		if (!compartmentManager)
			return null;
		
		array<BaseCompartmentSlot> compartments = new array <BaseCompartmentSlot>();
		
		for (int i = 0, cnt = compartmentManager.GetCompartments(compartments); i < cnt; i++)
		{
			BaseCompartmentSlot slot = compartments[i];
			
			if ((!gunner && slot.Type() == PilotCompartmentSlot) || (gunner && slot.Type() == TurretCompartmentSlot))
				return SCR_ChimeraCharacter.Cast(slot.GetOccupant());
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	protected SCR_PlayerPenaltyData GetPlayerPenaltyData(int playerId)
	{
		if (IsProxy())
			return null;
		
		SCR_PlayerPenaltyData playerPenaltyData;

		// Check if the client is reconnecting
		for (int i = 0, cnt = m_aPlayerPenaltyData.Count(); i < cnt; i++)
		{
			if (m_aPlayerPenaltyData[i].GetPlayerId() == playerId)
			{
				playerPenaltyData = m_aPlayerPenaltyData[i];
				break;
			}
		}
		
		// Client reconnected, return saved data
		if (playerPenaltyData)
			return playerPenaltyData;
		
		// Check validity of playerId before registering new data
		PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(playerId);
		
		if (!pc)
		{
			Print(string.Format("SCR_PlayerPenaltyComponent: No player with playerId %1 found.", playerId), LogLevel.ERROR);
			return null;
		}
		
		// First connection, register new data
		playerPenaltyData = new SCR_PlayerPenaltyData;
		playerPenaltyData.SetPlayerId(playerId);
		m_aPlayerPenaltyData.Insert(playerPenaltyData);
		
		return playerPenaltyData;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void EvaluatePlayerPenalties()
	{
		for (int i = 0, cnt = m_aPlayerPenaltyData.Count(); i < cnt; i++)
		{
			SCR_PlayerPenaltyData playerPenaltyData = m_aPlayerPenaltyData[i];
			
			// Periodically forgive a portion of penalty score, don't go below zero
			if (playerPenaltyData.GetPenaltyScore() > 0 && playerPenaltyData.GetNextPenaltySubstractionTimestamp() < Replication.Time())
			{
				int forgivenScore;
				
				if (m_iPenaltySubstractionPoints > playerPenaltyData.GetPenaltyScore())
					forgivenScore = playerPenaltyData.GetPenaltyScore();
				else
					forgivenScore = m_iPenaltySubstractionPoints;
				
				playerPenaltyData.AddPenaltyScore(-forgivenScore);
			}
			
			// Check penalty limit for kick / ban
			// Moved to backend callback OnBanResult()
			/*int playerId = playerPenaltyData.GetPlayerId();
			
			// Player is not connected
			if (!GetGame().GetPlayerManager().GetPlayerController(playerId))
				continue;
			
			// Player is host
			if (playerId == SCR_PlayerController.GetLocalPlayerId())
				continue;
			
			if (m_iKickPenaltyLimit > 0 && playerPenaltyData.GetPenaltyScore() >= m_iKickPenaltyLimit)
			{
				// TODO: Use callback from backend instead
				KickPlayer(playerId, m_iBanDuration, SCR_PlayerManagerKickReason.FRIENDLY_FIRE);
				continue;
			}*/
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void AddPenaltyScore(int playerId, int points)
	{
		BackendApi beApi = GetGame().GetBackendApi();
		
		if (beApi && points > 0)
			beApi.PlayerBanEvent("Trolling", "FriendlyFire", points, playerId);
		
		SCR_PlayerPenaltyData playerPenaltyData = GetPlayerPenaltyData(playerId);
		
		if (!playerPenaltyData)
			return;
		
		playerPenaltyData.AddPenaltyScore(points);
	}
	
	//------------------------------------------------------------------------------------------------
	void KickPlayer(int playerId, int duration, SCR_PlayerManagerKickReason reason)
	{		
		GetGame().GetPlayerManager().KickPlayer(playerId, reason, duration);
		
		SCR_PlayerPenaltyData playerPenaltyData = GetPlayerPenaltyData(playerId);
		
		if (playerPenaltyData)
			playerPenaltyData.AddPenaltyScore(-playerPenaltyData.GetPenaltyScore());
	}
	
	//------------------------------------------------------------------------------------------------
	int GetPenaltySubstractionPeriod()
	{
		return m_iPenaltySubstractionPeriod * 1000;	// Converting s to ms
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsProxy()
	{
		return (m_RplComponent && m_RplComponent.IsProxy());
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		if (IsProxy())
			return;
		
		if (m_iBanDuration < m_iPenaltySubstractionPeriod)
			Print("SCR_PlayerPenaltyComponent: Ban duration is shorter than Penalty substraction period. This will cause the player to remain banned until their penalty is substracted.", LogLevel.WARNING);
		
		if (!GetGame().InPlayMode())
			return;
		
		s_Instance = this;
		m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		GetGame().GetCallqueue().CallLater(EvaluatePlayerPenalties, EVALUATION_PERIOD, true);
		
		BackendApi beApi = GetGame().GetBackendApi();
		
		if (beApi)
		{
			m_Callback = new SCR_PlayerPenaltyDSSessionCallback(this);
			beApi.SetSessionCallback(m_Callback);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ~SCR_PlayerPenaltyComponent()
	{
		if (m_aPlayerPenaltyData)
		{
			m_aPlayerPenaltyData.Clear();
			m_aPlayerPenaltyData = null;
		}
	}
};

//------------------------------------------------------------------------------------------------
class SCR_PlayerPenaltyData
{
	protected int m_iPlayerId;
	protected float m_fBannedUntil;
	protected int m_iPenaltyScore;
	protected float m_fNextPenaltySubstractionTimestamp;
	protected bool m_bWasKicked;
	protected SCR_PlayerManagerKickReason m_eKickReason = SCR_PlayerManagerKickReason.DISRUPTIVE_BEHAVIOUR;
	
	//------------------------------------------------------------------------------------------------
	void SetPlayerId(int playerId)
	{
		m_iPlayerId = playerId;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetPlayerId()
	{
		return m_iPlayerId;
	}
	
	//------------------------------------------------------------------------------------------------
	void AddPenaltyScore(int points)
	{
		m_iPenaltyScore += points;
		
		// Start the timer on penalty substraction when player was penalized while the timer was stopped
		if ((points > 0 && m_fNextPenaltySubstractionTimestamp < Replication.Time()) || (points < 0 && m_iPenaltyScore > 0))
			m_fNextPenaltySubstractionTimestamp = Replication.Time() + SCR_PlayerPenaltyComponent.GetInstance().GetPenaltySubstractionPeriod();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetPenaltyScore()
	{
		return m_iPenaltyScore;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetNextPenaltySubstractionTimestamp(float timestamp)
	{
		m_fNextPenaltySubstractionTimestamp = timestamp;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetNextPenaltySubstractionTimestamp()
	{
		return m_fNextPenaltySubstractionTimestamp;
	}
};