//------------------------------------------------------------------------------------------------
class SP_FactionManagerClass: SCR_FactionManagerClass
{
};

//------------------------------------------------------------------------------------------------
class SP_FactionManager : SCR_FactionManager
{
	void AddFaction(SCR_Faction NewFaction)
	{
		array<string> ancestors;
		if (m_aAncestors.Find(NewFaction.GetFactionKey(), ancestors))
			NewFaction.SetAncestors(ancestors);
				
		m_SortedFactions.Insert(NewFaction.GetOrder(), NewFaction);
				
		NewFaction.InitializeFaction();
	}
	void DuplicateFaction(SCR_Faction OriginalFaction)
	{
		SCR_Faction newfaction = new SCR_Faction();
	}
};
