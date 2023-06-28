class SP_Task: ScriptAndConfig
{
	IEntity TaskOwner;
	IEntity TaskTarget;
	string TaskDesc;
	string TaskDiag;
	protected ETaskState e_State = ETaskState.EMPTY;
	ref array <IEntity> a_TaskAssigned = new ref array <IEntity>();
	
	bool Init(){};
	ETaskState GetState()
	{
		return e_State;
	}
	void SetInfo(IEntity Owner, IEntity Target)
	{
		TaskOwner = Owner;
		TaskTarget = Target;
	}
	
	bool SetupTaskEntity()
	{
		return true;
	};
	
	bool CharacterIsTarget(IEntity Character)
	{
		if (Character == TaskTarget)
		{
			return true;
		}
		return false;
	}
	
	bool CharacterIsOwner(IEntity Character)
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
	
	void AssignCharacter(IEntity Character)
	{
		a_TaskAssigned.Insert(Character);
		if(a_TaskAssigned.Count() > 0 && e_State == ETaskState.EMPTY)
		{
			e_State = ETaskState.ASSIGNED;
		}
	}
	
	bool CharacterAssigned(IEntity Character)
	{
		if(a_TaskAssigned.Contains(Character))
		{
			return true;
		}
		return false;
	}
	void UpdateState()
	{
	};
	bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		return false;
	};
	bool CompleteTask(IEntity Assignee)
	{
	};
	bool GiveReward(IEntity Target)
	{
		InventoryStorageManagerComponent TargetInv = InventoryStorageManagerComponent.Cast(Target.FindComponent(InventoryStorageManagerComponent));
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = vector.Zero;
		Resource res = Resource.Load("{6E932B6B724F4AE7}prefabs/Currency/Watch_Currency.et");
		if (res)
		{
				array<IEntity> Reward = new array<IEntity>();
				for (int j = 0; j < 10; j++)
					Reward.Insert(GetGame().SpawnEntityPrefab(res, Target.GetWorld(), params));
				for (int i, count = Reward.Count(); i < count; i++)
				{
					if(TargetInv.TryInsertItem(Reward[i]) == false)
					{
						return false;
					}
				}
				return true;
		}
		return false;
	};

};
enum ETaskState
{
	UNASSIGNED,
	ASSIGNED,
	COMPLETED,
	FAILED,
	EMPTY
}