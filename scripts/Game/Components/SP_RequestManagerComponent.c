class SP_RequestManagerComponentClass : ScriptComponentClass
{
};
class SP_RequestManagerComponent : ScriptComponent
{
	[Attribute()]
	float m_fTaskGenTime;
	
	[Attribute(defvalue: "20", desc:"Tasks will be continiusly created until this number is met, after that askRespawnTimer will have to pass for a task to be created")]
	int m_iMinTaskAmount;
	
	[Attribute(defvalue: "3", desc: "Max amount of tasks a character can be requesting at the same time")]
	int m_fTaskPerCharacter;
	
	[Attribute(defvalue: "2", desc: "Max amount of tasks of sametype a character can be requesting at the same time")]
	int m_fTaskOfSameTypePerCharacter;
	
	[Attribute(desc: "Type of tasks to spawn")]
	ref array <ref SP_Task> m_TasksToSpawn;
	
	[Attribute(defvalue: "60", desc: "Task garbage mamager kinda. Completed task are added to their own list, failed tasks are deleted")]
	float m_fTaskClearTime;
	
	protected float m_fTaskRespawnTimer;
	protected float m_fTaskClearTimer;
	//------------------------------------------------------------------------------------------------------------//
	static ref array<ref SP_Task> TaskMap = null;
	static ref array<ref SP_Task> CompletedTaskMap = null;
	//------------------------------------------------------------------------------------------------------------//
	void ~SP_RequestManagerComponent(){TaskMap.Clear();};
	//------------------------------------------------------------------------------------------------------------//
	override void EOnInit(IEntity owner)
	{
		if(!TaskMap)
			TaskMap = new array<ref SP_Task>();
		if(!CompletedTaskMap)
			CompletedTaskMap = new array<ref SP_Task>();
	}
	SP_Task GetTaskSample(typename tasktype)
	{
		foreach (SP_Task task : m_TasksToSpawn)
		{
			if(task.GetClassName() == tasktype)
				{
					return task;
				}
		}
		return null;
	}
	//------------------------------------------------------------------------------------------------------------//
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
	//------------------------------------------------------------------------------------------------------------//
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
	//------------------------------------------------------------------------------------------------------------//
	void UpdateCharacterTasks(IEntity Char)
	{
		if(!Char)
		{
			return;
		}
		foreach (SP_Task task : TaskMap)
		{
			if(task.CharacterIsOwner(Char) == true)
			{
				task.UpdateState();
			}
			if(task.CharacterIsTarget(Char) == true)
			{
				task.UpdateState();
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	bool CreateTask(typename TaskType)
	{
		if(!TaskType)
		{
			return false;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		SP_Task Task = SP_Task.Cast(TaskType.Spawn());
		if(Task.Init())
		{
			IEntity Owner = Task.GetOwner();
			if(Owner)
			{
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
				};
				string charname = Diag.GetCharacterName(Owner);
				string charrank = Diag.GetCharacterRankName(Owner);
				if (TaskMap.Count() > m_iMinTaskAmount)
				{
					Diag.SendText(Task.GetTaskDiag(), Channel, 0, charname, charrank);
				}
			}
			TaskMap.Insert(Task);
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
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
	int GetInProgressTaskCount()
	{
		array<ref SP_Task> tasks = new array<ref SP_Task>();
		foreach (SP_Task task : TaskMap)
		{
			if(task.GetState() == ETaskState.UNASSIGNED || task.GetState() == ETaskState.ASSIGNED)
			{
				tasks.Insert(task);
			}
		}
		return tasks.Count();
	}
	//------------------------------------------------------------------------------------------------------------//
	void GetCharTasksOfSameType(IEntity Char,out array<ref SP_Task> tasks, typename tasktype)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CharacterIsOwner(Char) == true)
			{
				if(task.GetClassName() == tasktype)
				{
					tasks.Insert(task);
				}
				
			}
			if(task.CharacterIsTarget(Char) == true)
			{
				if(task.GetClassName() == tasktype)
				{
					tasks.Insert(task);
				}
				
			}
		}
	}
	void GetRescueTask(IEntity Char,out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.GetClassName() == SP_RescueTask)
			{
				SP_RescueTask resctask = SP_RescueTask.Cast(task);
				if(resctask.GetCharsToResc().Contains(Char))
					tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	void GetTasksOfSameType(out array<ref SP_Task> tasks, typename tasktype)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.GetClassName() == tasktype)
			{
				tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	void GetCharTargetTasks(IEntity Char, out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CharacterIsTarget(Char) == true)
			{
				tasks.Insert(task);
			}
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	void ClearTasks()
	{
		int removed;
		for (int i = TaskMap.Count() - 1; i >= 0; i--)
		{
			if (TaskMap[i].GetState() == ETaskState.FAILED) 
			{
				TaskMap.Remove(i);
				removed += 1;
			}
			if (TaskMap[i].GetState() == ETaskState.COMPLETED) 
			{
				OnTaskCompleted(TaskMap[i]);
				removed += 1;
			}
		}
		Print(string.Format("Cleanup finished, %1 tasks got cleared", removed))
		/*foreach (SP_Task task : TaskMap)
	    {
				if (task.GetState() == ETaskState.FAILED) 
				{
					TaskMap.Remove(TaskMap.Find(task));
					c--;
					continue;
				}
				if (task.GetState() == ETaskState.COMPLETED) 
				{
					OnTaskCompleted(task);
				}
				i++;
			}
		*/
	}
	//------------------------------------------------------------------------------------------------------------//
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
		SetEventMask(owner, EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	//------------------------------------------------------------------------------------------------------------//
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (GetInProgressTaskCount() < m_iMinTaskAmount)
		{
			typename Task = m_TasksToSpawn.GetRandomElement().GetClassName();
			CreateTask(Task);
		}
		else
		{
			m_fTaskRespawnTimer += timeSlice;
			if(m_fTaskRespawnTimer > m_fTaskGenTime)
			{
				typename Task;
				if(m_TasksToSpawn)
				{
					Task = m_TasksToSpawn.GetRandomElement().GetClassName();
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
		m_fTaskClearTimer += timeSlice;
		if(m_fTaskClearTimer > m_fTaskClearTime)
		{
			m_fTaskClearTimer = 0;
			ClearTasks();
		}
	};
	//------------------------------------------------------------------------------------------------------------//
	void OnTaskCompleted(SP_Task Task)
	{
		TaskMap.Remove(TaskMap.Find(Task));
		CompletedTaskMap.Insert(Task);
	};
};
//------------------------------------------------------------------------------------------------------------//
modded enum EWeaponType
{
	WT_KNIFE
}
//------------------------------------------------------------------------------------------------------------//
modded enum SCR_EArsenalItemType
{
	FOOD = 262200,
	DRINK = 262600,
	RADIO = 262800,
	COMPASS = 263000,
	FLASHLIGHT = 263500,
	MAP = 270000,
	CURRENCY = 280000,
};
//------------------------------------------------------------------------------------------------------------//
modded enum SCR_EArsenalItemMode
{
	GADGET = 128
	
};