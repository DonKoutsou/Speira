[EntityEditorProps(category: "GameScripted/GameMode", description: "Takes care of player penalties, kicks, bans etc.", color: "0 0 255 255")]
class SP_PlayerPenaltyComponentClass: SCR_PlayerPenaltyComponentClass
{
};

//------------------------------------------------------------------------------------------------
class SP_PlayerPenaltyComponent: SCR_PlayerPenaltyComponent
{
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
		
		if (m_iFriendlyAIKillPenalty == 0 && m_iFriendlyPlayerKillPenalty == 0)
			return;
		
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
		//SCR_PlayerPenaltyData playerPenaltyData = GetPlayerPenaltyData(killerPlayerId);
		
		//if (!playerPenaltyData)
		//	return;
		
		//if (victimPlayerId == 0)
		//	playerPenaltyData.AddPenaltyScore(m_iFriendlyAIKillPenalty);
		//else
		//	playerPenaltyData.AddPenaltyScore(m_iFriendlyPlayerKillPenalty);
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
}