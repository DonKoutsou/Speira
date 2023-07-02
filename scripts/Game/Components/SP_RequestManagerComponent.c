class SP_RequestManagerComponentClass : ScriptComponentClass
{
};
class SP_RequestManagerComponent : ScriptComponent
{
	[Attribute()]
	float m_fTaskGenTime;
	
	[Attribute(defvalue: "20")]
	int m_iMinTaskAmount;
	
	[Attribute(defvalue: "3", desc: "Max amount of tasks a character can be requesting at the same time")]
	int m_fTaskPerCharacter;
	
	
	protected float m_fTaskRespawnTimer;
	//----------------------------------------------------------------------------------------------------------------//
	static ref array<ref SP_Task> TaskMap = null;
	int tasknum;
	//----------------------------------------------------------------------------------------------------------------//
	override void EOnInit(IEntity owner)
	{
		if(!TaskMap)
			TaskMap = new array<ref SP_Task>();
	}
	bool CharHasTask(IEntity Char)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CharacterIsOwner(Char) == true)
			{
				return true;
			}
		}
		return false;
	}
	bool CharIsTarget(IEntity Char)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CharacterIsTarget(Char) == true)
			{
				return true;
			}
		}
		return false;
	}
	void UpdateCharacterTasks(IEntity Char)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CharacterIsOwner(Char) == true)
			{
				task.UpdateState();
			}
		}
	}
	bool CreateTask(typename TaskType)
	{
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		SP_Task Task = SP_Task.Cast(TaskType.Spawn());
		if(Task.Init())
		{
			IEntity Owner = Task.GetOwner();
			array<ref SP_Task> tasks = new array<ref SP_Task>();
			GetCharTasks(Owner, tasks);
			if(tasks.Count() >= m_fTaskPerCharacter)
			{
				return false;
			}
			string charname = Diag.GetCharacterName(Owner);
			string charrank = Diag.GetCharacterRankName(Owner);
			if(charname == " " || charrank == " ")
			{
				return false;
			}
			FactionKey senderFaction = Diag.GetCharacterFaction(Owner).GetFactionKey();
			BaseChatChannel Channel;
			switch (senderFaction)
			{
				case "FIA":
					Channel = Diag.m_ChatChannelFIA;
				break;
				case "USSR":
					Channel = Diag.m_ChatChannelUSSR;
				break;
				case "BANDITS":
					Channel = Diag.m_ChatChannelBANDITS;
				break;
				case "SPEIRA":
					Channel = Diag.m_ChatChannelSPEIRA;
				break;
				case "US":
					Channel = Diag.m_ChatChannelUS;
				break;
				case "RENEGADE":
					{
					return false;
					}
				break;
			}
			if (TaskMap.Count() > m_iMinTaskAmount)
			{
				Diag.SendText(Task.GetTaskDiag(), Channel, 0, charname, charrank);
			}
			TaskMap.Insert(Task);
			return true;
		}
		return false;
	}
	void GetCharTasks(IEntity Char,out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CharacterIsOwner(Char) == true)
			{
				tasks.Insert(task);
			}
		}
	}
	void GetCharTargetTasks(IEntity Char,out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CharacterIsTarget(Char) == true)
			{
				tasks.Insert(task);
			}
		}
	}
	void ClearTasks()
	{
		//for (int i = TaskMap.Count() - 1; i >= 0; i--)
		//{
		//	if (TaskMap[i].GetState() == ETaskState.FAILED || TaskMap[i].GetState() == ETaskState.COMPLETED) 
		//	{
		//		TaskMap.Remove(i);
		//	}
		//}
		
		for (int i = 0, c = TaskMap.Count(); i < c;)
	    {
			if (TaskMap[i].GetState() == ETaskState.FAILED || TaskMap[i].GetState() == ETaskState.COMPLETED) 
			{
				TaskMap.Remove(TaskMap.Find(TaskMap[i]));
				c--;
				continue;
			}
			i++;
		}
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
		if (TaskMap.Count() < m_iMinTaskAmount)
		{
			int index = Math.RandomInt(0, 2);
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
			return;
		}
		m_fTaskRespawnTimer += timeSlice;
		if(m_fTaskRespawnTimer > m_fTaskGenTime)
		{
			int index = Math.RandomInt(0, 2);
			typename Task;
			if(index == 0)
			{
				Task = SP_DeliverTask;
			}
			if(index == 1)
			{
				Task = SP_BountyTask;
			}
			if(CreateTask(Task))
			{
				m_fTaskRespawnTimer = 0;
			}
			else
			{
				m_fTaskRespawnTimer -= 1;
			}
		}
	}
	void ~SP_RequestManagerComponent()
	{
		TaskMap.Clear();
	};
};
modded enum EWeaponType
{
	WT_KNIFE
}
modded enum SCR_EArsenalItemType
{
	FOOD = 262200,
	DRINK = 262600
};