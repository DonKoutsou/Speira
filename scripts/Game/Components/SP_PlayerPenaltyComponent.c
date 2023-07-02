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
		if (instigator && EntityUtils.IsPlayer(instigator))
		{
			if (entity)
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
						if(KillerF != VictimF)
						{
							VictimF.AdjustRelation(KillerF, -5);
							SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(instigator.FindComponent(SCR_CharacterIdentityComponent));
							string text;
							text = string.Format("You caused issues between %1 and %2, your reputation has worsened", KillerF.GetFactionKey(), VictimF.GetFactionKey());
							DiagComp.DoAnouncerDialogue(text);
							if(id.AdjustCharRep(-20))
							{
								DiagComp.DoAnouncerDialogue("Your reputation has fallen to much and your faction has expeled you. You'll be treated as renegade from now on");
								FactionComp.SetAffiliatedFactionByKey("RENEGADE");
							}
						}
						else if (KillerF == VictimF)
						{
							SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(instigator.FindComponent(SCR_CharacterIdentityComponent));
							if(id.AdjustCharRep(-20))
							{
								DiagComp.DoAnouncerDialogue("Your reputation has fallen to much and your faction has expeled you. You'll be treated as renegade from now on");
								FactionComp.SetAffiliatedFactionByKey("RENEGADE");
							}
							else
							{
								DiagComp.DoAnouncerDialogue("Killed unit of your own faction, your reputation is worsened by alot");
							}
						}
					}
				}
			}
		}
		SP_RequestManagerComponent ReqMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		if(ReqMan)
		{
			ReqMan.UpdateCharacterTasks(entity);
			ReqMan.UpdateCharacterTasks(instigator);
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
		
	}
	override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
	{
		super.OnPlayerSpawned(playerId, controlledEntity);
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(controlledEntity.FindComponent(SCR_CharacterIdentityComponent));
		id.SetCharacterRep(10);
	}
}