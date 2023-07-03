class SP_Task: ScriptAndConfig
{
	//-------------------------------------------------//
	//Character wich created the task
	IEntity TaskOwner;
	//-------------------------------------------------//
	//Target of the task (kill task, deliver task etc... not necesarry on retrieve task)
	IEntity TaskTarget;
	//-------------------------------------------------//
	string TaskDesc;
	//-------------------------------------------------//
	string TaskDiag;
	//-------------------------------------------------//
	ResourceName reward;
	//-------------------------------------------------//
	int m_iRewardAmount;
	//-------------------------------------------------//
	protected ETaskState e_State = ETaskState.EMPTY;
	//-------------------------------------------------//
	//Characters assigned to this task
	ref array <IEntity> a_TaskAssigned = new ref array <IEntity>();
	//-------------------------------------------------//
	typename GetClassName()
	{
		return SP_Task;
	}
	void DeleteLeftovers()
	{
	};
	bool Init()
	{
		CalculateReward();
		return true;
	};
	bool CheckCharacter(IEntity Owner)
	{
		SP_RequestManagerComponent ReqMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		array<ref SP_Task> tasks = new array<ref SP_Task>();
		ReqMan.GetCharTasks(Owner, tasks);
		if(tasks.Count() >= ReqMan.m_fTaskPerCharacter)
		{
			return false;
		}
		array<ref SP_Task> sametasks = new array<ref SP_Task>();
		ReqMan.GetCharTasksOfSameType(Owner, sametasks, GetClassName());
		if(sametasks.Count() >= ReqMan.m_fTaskOfSameTypePerCharacter)
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
		if (senderFaction == "RENEGADE")
		{
			return false;
		};
		return true;
	};
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
	void CalculateReward()
	{
		m_iRewardAmount = Math.RandomInt(2, 4);
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REWARD);
		array<SCR_EntityCatalogEntry> Mylist = new array<SCR_EntityCatalogEntry>();
		RequestCatalog.GetEntityListWithLabel(EEditableEntityLabel.ITEMTYPE_CURRENCY, Mylist);
		reward = Mylist.GetRandomElement().GetPrefab();
	};
	bool GiveReward(IEntity Target)
	{
		if (reward)
		{
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = vector.Zero;
			InventoryStorageManagerComponent TargetInv = InventoryStorageManagerComponent.Cast(Target.FindComponent(InventoryStorageManagerComponent));
			array<IEntity> Rewardlist = new array<IEntity>();
			Resource RewardRes = Resource.Load(reward);
			int Movedamount;
			for (int j = 0; j < m_iRewardAmount; j++)
				Rewardlist.Insert(GetGame().SpawnEntityPrefab(RewardRes, Target.GetWorld(), params));
			for (int i, count = Rewardlist.Count(); i < count; i++)
			{
				if(TargetInv.TryInsertItem(Rewardlist[i]) == false)
				{
					return false;
				}
				Movedamount += 1;
			}
			SCR_CharacterIdentityComponent id = SCR_CharacterIdentityComponent.Cast(Target.FindComponent(SCR_CharacterIdentityComponent));
			id.AdjustCharRep(5);
			SCR_HintManagerComponent.GetInstance().ShowCustom(Movedamount.ToString() + " " + "watches added to your wallet, and your reputation has improved");
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