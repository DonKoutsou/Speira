class SP_RequestManagerComponentClass : ScriptComponentClass
{
};
class SP_RequestManagerComponent : ScriptComponent
{
	[Attribute()]
	float m_fTaskGenTime;
	
	[Attribute(defvalue: "3", desc: "Max amount of tasks a character can be requesting at the same time")]
	int m_fTaskPerCharacter;
	
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
					return false;
				break;
			}
			TaskMap.Insert(Task);
			Diag.SendText(Task.GetTaskDiag(), Channel, 0, charname, charrank);
			return true;
		}
		return false;
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
};