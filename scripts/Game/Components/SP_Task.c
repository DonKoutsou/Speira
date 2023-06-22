class SP_Task: ScriptAndConfig
{
	IEntity TaskOwner;
	IEntity TaskTarget;
	
	string TaskDesc;
	
	void Init(){};
	
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
};