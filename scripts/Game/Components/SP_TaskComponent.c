class SP_Task: ScriptAndConfig
{
	void Init(IEntity Character, IEntity Player){};
};
class SP_BountyTask: SP_Task
{
	string OwnerName;
	IEntity Target;
	string TargetName;
	string TargetLocation;
	FactionKey m_Faction;
	void GetInfo(out string OName, out IEntity target, out string DName, out string TLoc, out FactionKey faction)
	{
		OName = OwnerName;
		DName = TargetName;
		target = Target;
		TLoc = TargetLocation;
		faction = m_Faction;
	};
	void SetInfo(string OName, IEntity target, string DName, string TLoc, FactionKey faction)
	{
		OwnerName = OName;
		TargetName = DName;
		Target = target;
		TargetLocation = TLoc;
		m_Faction = faction;
	}
	
	
};
class SP_DeliverTask: SP_Task
{
	string OwnerName;
	string DeliveryName;
	string DeliveryLocation;
	
	ResourceName m_ItemToGive;
	vector m_SpawnOffset;
	
	void GetInfo(out string OName, out string DName, out string DLoc)
	{
		OName = OwnerName;
		DName = DeliveryName;
		DLoc = DeliveryLocation;
	};
	void SetInfo(string OName, string DName, string DLoc)
	{
		OwnerName = OName;
		DeliveryName = DName;
		DeliveryLocation = DLoc;
	}
	override void Init(IEntity Character, IEntity Player)
	{
		AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		
		SP_AIDirector MyDirector;
		FactionAffiliationComponent FC = FactionAffiliationComponent.Cast(Character.FindComponent(FactionAffiliationComponent));
		SCR_Faction myfact = SCR_Faction.Cast(FC.GetAffiliatedFaction());
		
		SP_AIDirector OrDirector = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		OrDirector.GetDirectorOccupiedByFriendly(myfact, MyDirector);
		string keyunused;
		string key = MyDirector.GetMajorityHolder(keyunused).GetFactionKey();
		
		CharacterIdentityComponent CharID = CharacterIdentityComponent.Cast(Character.FindComponent(CharacterIdentityComponent));
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(Character.FindComponent(SCR_CharacterRankComponent));
		string OName = CharRank.GetCharacterRankName(Character) + " " + CharID.GetIdentity().GetName() + " " + CharID.GetIdentity().GetSurname();
		IEntity CharToDeliverTo;
		MyDirector.GetRandomUnitByFKey(key, CharToDeliverTo);
		string LocName = MyDirector.GetCharacterLocation(CharToDeliverTo);
		CharacterIdentityComponent CharID2 = CharacterIdentityComponent.Cast(CharToDeliverTo.FindComponent(CharacterIdentityComponent));
		string DName = CharRank.GetCharacterRankName(CharToDeliverTo) + " " + CharID2.GetIdentity().GetName() + " " + CharID2.GetIdentity().GetSurname();
		SetInfo(OName, DName, LocName);
	};

};
class SP_TaskManagerComponentClass: ScriptComponentClass
{
};
class SP_TaskManagerComponent: ScriptComponent
{
	[Attribute()]
	float m_fTaskGenTime;
	
	protected float m_fTaskRespawnTimer;
	//----------------------------------------------------------------------------------------------------------------//
	protected ref map<int, ref SP_Task> TaskMap;
	int tasknum;
	//----------------------------------------------------------------------------------------------------------------//
	override void EOnInit(IEntity owner)
	{
		TaskMap = new map<int, ref SP_Task>();
	}
	void CreateTask(typename TaskType)
	{
		SP_Task Task = SP_Task.Cast(TaskType.Spawn());
		Task.Init()
		TaskMap.Insert(tasknum, Task);
		tasknum = tasknum + 1;
	}
	//----------------------------------------------------------------------------------------------------------------//
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
		SetEventMask(owner, EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		m_fTaskRespawnTimer += timeSlice;
		 if(m_fTaskRespawnTimer > m_fTaskGenTime)
		{
			int index = Math.RandomInt(0, 1);
			typename Task;
			if(index == 0)
			{
				Task = SP_DeliverTask;
			}
			if(index == 1)
			{
				Task = SP_BountyTask;
			}
			CreateTask(Task);
		}
		
	}
}
//----------------------------------------------------------------------------------------------------------------//

