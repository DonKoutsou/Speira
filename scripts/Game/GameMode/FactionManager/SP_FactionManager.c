class SP_FactionManagerClass : SCR_FactionManagerClass
{
};
class SP_FactionManager : SCR_FactionManager
{
	[Attribute("5", desc: "applies input value as negative, if value is 5 it will apply a -5 penalty")]
	int m_FactionFriendlyKillRepPenalty;
	
	[Attribute("20", desc: "applies input value as negative, if value is 5 it will apply a -5 penalty")]
	int m_CharacterFriendlyKillRepPenalty;
	
	[Attribute("5")]
	int m_FactionTaskCompleteRepBonus;
	
	[Attribute("5")]
	int m_CharacterTaskCompleteRepBonus;
	
	private SP_GameMode m_GameMode;
	
	ref array<Faction> m_Factions;
	
	void ~SP_FactionManager()
	{
		if(m_Factions)
		{
			m_Factions.Clear();
		}
	};
	
	EFactionRelationState GetFactionRelationState(SCR_Faction Faction1, SCR_Faction Faction2)
	{
		int relation1;
		Faction1.GetFactionRep(Faction2, relation1);
		int relation2;
		Faction2.GetFactionRep(Faction1, relation2);
		if (relation1 > -50 && relation2 > -50)
		{
			if (relation1 > 50 && relation2 > 50)
			{
				return EFactionRelationState.FRIENDLY;
			}
			return EFactionRelationState.OKWITH;
		}
		return EFactionRelationState.ENEMY;
	};
	void OnFriendlyKill(IEntity Victim, IEntity Killer)
	{
		FactionAffiliationComponent FactionComp = FactionAffiliationComponent.Cast(Killer.FindComponent(FactionAffiliationComponent));
		FactionAffiliationComponent FactionCompVictim = FactionAffiliationComponent.Cast(Victim.FindComponent(FactionAffiliationComponent));
		SCR_Faction instigator = SCR_Faction.Cast(FactionComp.GetAffiliatedFaction());
		SCR_Faction Afflicted = SCR_Faction.Cast(FactionCompVictim.GetAffiliatedFaction());
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(Killer.FindComponent(SCR_CharacterIdentityComponent));
		if (EntityUtils.IsPlayer(Killer))
		{
			SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("You caused issues between %1 and %2, your reputation has worsened", instigator.GetFactionKey(), Afflicted.GetFactionKey()));
		}
		if (id.AdjustCharRep(-m_CharacterFriendlyKillRepPenalty))
		{
			FactionComp.SetAffiliatedFactionByKey("RENEGADE");
			if (EntityUtils.IsPlayer(Killer))
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom("Your reputation has fallen to much and your faction has expeled you. You'll be treated as renegade from now on");
			}
		}
		Afflicted.AdjustRelation(instigator, -m_FactionFriendlyKillRepPenalty);
		UpdateFactionRelations();
	};
	
	void OnSameFactionKill(IEntity Victim, IEntity Killer)
	{
		FactionAffiliationComponent FactionComp = FactionAffiliationComponent.Cast(Killer.FindComponent(FactionAffiliationComponent));
		SCR_Faction instigator = SCR_Faction.Cast(FactionComp.GetAffiliatedFaction());
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(Killer.FindComponent(SCR_CharacterIdentityComponent));
		if (id.AdjustCharRep(-m_CharacterFriendlyKillRepPenalty))
		{
			FactionComp.SetAffiliatedFactionByKey("RENEGADE");
			if (EntityUtils.IsPlayer(Killer))
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom("Your reputation has fallen to much and your faction has expeled you. You'll be treated as renegade from now on");
			}
		}
		else
		{
			if (EntityUtils.IsPlayer(Killer))
			{
				SCR_HintManagerComponent.GetInstance().ShowCustom("Killed unit of your own faction, your reputation is worsened by alot");
			}
		}
	}
	void OnTaskCompleted(IEntity TaskOwner, IEntity Assignee)
	{
		if(TaskOwner == Assignee)
		{
			return;
		}
		FactionAffiliationComponent FactionCompAssignee = FactionAffiliationComponent.Cast(Assignee.FindComponent(FactionAffiliationComponent));
		FactionAffiliationComponent FactionCompOwner = FactionAffiliationComponent.Cast(TaskOwner.FindComponent(FactionAffiliationComponent));
		SCR_Faction instigatorFaction = SCR_Faction.Cast(FactionCompAssignee.GetAffiliatedFaction());
		SCR_Faction OwnerFaction = SCR_Faction.Cast(FactionCompOwner.GetAffiliatedFaction());
		SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(Assignee.FindComponent(SCR_CharacterIdentityComponent));
		id.AdjustCharRep(m_CharacterTaskCompleteRepBonus);
		OwnerFaction.AdjustRelation(instigatorFaction, m_FactionTaskCompleteRepBonus);
		UpdateFactionRelations();
	};
	void UpdateFactionRelations()
	{
		foreach (Faction fact : m_Factions)
		{
			SCR_Faction SCRFact = SCR_Faction.Cast(fact);
			foreach (Faction checkfact : m_Factions)
			{
				SCR_Faction SCRCheckFact = SCR_Faction.Cast(checkfact);
				int relation;
				SCRFact.GetFactionRep(SCRCheckFact, relation);
				if(relation > -50)
				{
					SetFactionsFriendly(SCRFact, SCRCheckFact);
				}
				else if(relation <= -50)
				{
					SetFactionsHostile(SCRFact, SCRCheckFact);
				}
			}
		};
	};
	override void EOnInit(IEntity owner)
	{
		if (!m_GameMode)
			m_GameMode = SP_GameMode.Cast(GetGame().GetGameMode());
		if(!m_Factions)
		{
			m_Factions = new array<Faction>();
			GetFactionsList(m_Factions);
		}
			
	};
};
//------------------------------------------------------------------------------------------------------------//
enum EFactionRelationState
{
	ENEMY,
	OKWITH,
	FRIENDLY
}
