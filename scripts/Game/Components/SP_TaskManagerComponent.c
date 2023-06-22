class SP_TaskManagerComponentClass : ScriptComponentClass
{
};
class SP_TaskManagerComponent : ScriptComponent
{
	[Attribute()]
	float m_fTaskGenTime;
	
	protected float m_fTaskRespawnTimer;
	//----------------------------------------------------------------------------------------------------------------//
	protected ref array<ref SP_Task> TaskMap;
	int tasknum;
	//----------------------------------------------------------------------------------------------------------------//
	override void EOnInit(IEntity owner)
	{
		TaskMap = new array<ref SP_Task>();
	}
	bool CharHasTask(IEntity Char)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CheckIfCharacterIsOwner(Char) == true)
			{
				return true;
			}
		}
		return false;
	}
	void CreateTask(typename TaskType)
	{
		SP_Task Task = SP_Task.Cast(TaskType.Spawn());
		TaskMap.Insert(Task);
		Task.Init();
		tasknum = tasknum + 1;
	}
	void GetCharTasks(IEntity Char,out array<ref SP_Task> tasks)
	{
		foreach (SP_Task task : TaskMap)
		{
			if(task.CheckIfCharacterIsOwner(Char) == true)
			{
				tasks.Insert(task);
			}
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
			CreateTask(Task);
			m_fTaskRespawnTimer = 0;
		}
		
	}
};
//----------------------------------------------------------------------------------------------------------------//


