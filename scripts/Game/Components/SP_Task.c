class SP_Task: ScriptAndConfig
{
	IEntity TaskOwner;
	IEntity TaskTarget;
	string TaskDesc;
	string TaskDiag;
	ref array <IEntity> a_TaskAssigned = new ref array <IEntity>();
	
	bool Init(){};
	
	void SetInfo(IEntity Owner, IEntity Target)
	{
		TaskOwner = Owner;
		TaskTarget = Target;
	}
	bool CheckIfCharacterIsTarget(IEntity Character)
	{
		if (Character == TaskTarget)
		{
			return true;
		}
		return false;
	}
	bool CheckIfCharacterIsOwner(IEntity Character)
	{
		if (Character == TaskOwner)
		{
			return true;
		}
		return false;
	}
	string GetTaskDescription()
	{
		return TaskDesc;
	}
	string GetTaskDiag()
	{
		return TaskDiag;
	}
	IEntity GetOwner()
	{
		return TaskOwner;
	}
	void AddAssignedCharacter(IEntity Character)
	{
		a_TaskAssigned.Insert(Character);
	}
	bool IsCharacterAssigned(IEntity Character)
	{
		if(a_TaskAssigned.Contains(Character))
		{
			return true;
		}
		return false;
	}

};