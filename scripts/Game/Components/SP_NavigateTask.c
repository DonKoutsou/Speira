class SP_NavigateTask: SP_Task
{
	[Attribute(defvalue: "20")]
	int m_iRewardAverageAmount;
	
	[Attribute(defvalue: "20")]
	int SuccessDistance;
	
	IEntity Destination;
	
	override bool FindOwner(out IEntity Owner)
	{
		SP_AIDirector MyDirector = SP_AIDirector.AllDirectors.GetRandomElement();
		FactionManager factionsMan = FactionManager.Cast(GetGame().GetFactionManager());
		string keyunused;
		Faction Fact = MyDirector.GetMajorityHolder(keyunused);
		if (!Fact)
		{
			return false;
		}
		FactionKey key = Fact.GetFactionKey();
		SCR_Faction myfact = SCR_Faction.Cast(factionsMan.GetFactionByKey(key));
		if (!MyDirector.GetRandomUnitByFKey(key, Owner))
		{
			return false;
		}
		if(Owner)
		{
			return true;
		}
		return false;
	};
	override bool FindTarget(out IEntity Target)
	{
		AIControlComponent comp = AIControlComponent.Cast(GetOwner().FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector MyDirector = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		//-----------------------------------------------------------------//
		FactionManager factionsMan = FactionManager.Cast(GetGame().GetFactionManager());
		string keyunused;
		Faction Fact = MyDirector.GetMajorityHolder(keyunused);
		if (!Fact)
		{
			return false;
		}
		FactionKey key = Fact.GetFactionKey();
		SCR_Faction myfact = SCR_Faction.Cast(factionsMan.GetFactionByKey(key));
		if (!MyDirector.GetDirectorOccupiedByFriendly(myfact, Destination))
		{
			return false;
		}
		if(Destination)
		{
			return true;
		}
		return false;
	};
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		float dis = vector.Distance(Destination.GetOrigin(), TalkingChar.GetOrigin());
		if(dis <= SuccessDistance)
		{
			return true;
		}
		return false;
	};
}