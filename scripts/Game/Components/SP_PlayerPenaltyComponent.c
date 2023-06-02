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
				SCR_FactionManager FMan = SCR_FactionManager.Cast(GetGame().GetFactionManager());
				FactionAffiliationComponent FactionComp = FactionAffiliationComponent.Cast(instigator.FindComponent(FactionAffiliationComponent));
				FactionAffiliationComponent FactionCompVictim = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
				if (FactionComp && FactionCompVictim)
				{
					SCR_Faction KillerF = SCR_Faction.Cast(FactionComp.GetAffiliatedFaction());
					SCR_Faction VictimF = SCR_Faction.Cast(FactionCompVictim.GetAffiliatedFaction());
					if(VictimF.IsFactionFriendly(KillerF) == true)
					{
						SP_DialogueComponent DiagComp = SP_DialogueComponent.Cast(GetGameMode().FindComponent(SP_DialogueComponent));
						if (KillerF.GetFactionKey() == "SPEIRA" && VictimF.GetFactionKey() == "USSR")
						{
							DiagComp.DoAnouncerDialogue("Killed unit of the soviet faction, soviets will atack you from now on");
							FMan.SetFactionsHostile(KillerF, VictimF);
						}
						else if (KillerF.GetFactionKey() == "SPEIRA" && VictimF.GetFactionKey() == "FIA")
						{
							DiagComp.DoAnouncerDialogue("Killed unit of the FIA faction, FIA will atack you from now on");
							FMan.SetFactionsHostile(KillerF, VictimF);
						}
						else if (KillerF.GetFactionKey() == "SPEIRA" && VictimF.GetFactionKey() == "US")
						{
							DiagComp.DoAnouncerDialogue("Killed unit of the US faction, US will atack you from now on");
							FMan.SetFactionsHostile(KillerF, VictimF);
						}
						else if (KillerF == VictimF)
						{
							DiagComp.DoAnouncerDialogue("Killed unit of your own faction, you've gone Renegade");
							FactionComp.SetAffiliatedFactionByKey("RENEGADE");
						}
					}
				}
				
			}
		}
		if (IsProxy())
			return;
		
		//if (m_iFriendlyAIKillPenalty == 0 && m_iFriendlyPlayerKillPenalty == 0)
		//	return;
		if (entity)
		{
		SP_CharacterAISmartActionComponent SmartComp = SP_CharacterAISmartActionComponent.Cast(entity.FindComponent(SP_CharacterAISmartActionComponent));
				if(SmartComp)
				{
					SmartComp.SetActionAccessible(true);
				}
		}
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
		//SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(entity.FindComponent(SCR_InventoryStorageManagerComponent));
		//array<IEntity> items = new array<IEntity>();
		//inv.GetItems(items);
		//InventoryStorageManagerComponent stocompKiller = InventoryStorageManagerComponent.Cast(instigator.FindComponent(InventoryStorageManagerComponent));
		//foreach (IEntity item : items)
		//{
		//SP_UnretrievableComponent Unretr = SP_UnretrievableComponent.Cast(item.FindComponent(SP_UnretrievableComponent));
		//	if (Unretr)
		//		{
		//			inv.TryMoveItemToStorage(item, stocompKiller.FindStorageForItem(item));
		//		}
		//}
	}
}