//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true)]
class SP_Task
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
	IEntity m_Copletionist;
	//------------------------------------------------------------------------------------------------------------//
	bool Init()
	{
		//-------------------------------------------------//
		//first look for owner cause targer is usually derived from owner faction/location etc...
		if (!FindOwner(TaskOwner))
		{
			return false;
		}
		//-------------------------------------------------//
		//function to fill to check ckaracter
		if(!CheckCharacter(TaskOwner))
		{
			return false;
		}
		//-------------------------------------------------//
		if (!FindTarget(TaskTarget))
		{
			return false;
		}
		//-------------------------------------------------//
		//function to fill if task needs an entity, eg package for delivery
		if (!SetupTaskEntity())
		{
			DeleteLeftovers();
			return false;
		}
		//-------------------------------------------------//
		if (!AssignReward())
		{
			DeleteLeftovers();
			return false;
		}
		//-------------------------------------------------//
		CreateDescritions();
		e_State = ETaskState.UNASSIGNED;
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	bool FindOwner(out IEntity Owner){return true;};
	//------------------------------------------------------------------------------------------------------------//
	bool CheckCharacter(IEntity Owner)
	{
		SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(Owner.FindComponent(SCR_CharacterDamageManagerComponent));
		if(dmg.GetIsUnconscious())
		{
			return false;
		}
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
	//------------------------------------------------------------------------------------------------------------//
	bool CharacterIsOwner(IEntity Character)
	{
		if (Character == TaskOwner)
		{
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	IEntity GetOwner(){return TaskOwner;};
	IEntity GetTarget(){return TaskTarget;};
	//------------------------------------------------------------------------------------------------------------//
	bool FindTarget(out IEntity Target){return true;};
	//------------------------------------------------------------------------------------------------------------//
	bool CharacterIsTarget(IEntity Character)
	{
		if (Character == TaskTarget)
		{
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	bool SetupTaskEntity(){return true;};
	//------------------------------------------------------------------------------------------------------------//
	bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee){return true;};
	//------------------------------------------------------------------------------------------------------------//
	bool AssignReward()
	{
		EEditableEntityLabel RewardLabel;
		int index = Math.RandomInt(0,2);
		if(index == 0)
		{
			RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
			m_iRewardAmount = 10;
		}
		if(index == 1)
		{
			RewardLabel = EEditableEntityLabel.ITEMTYPE_WEAPON;
			m_iRewardAmount = 1;
		}
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_EntityCatalog RequestCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REWARD);
		array<SCR_EntityCatalogEntry> Mylist = new array<SCR_EntityCatalogEntry>();
		RequestCatalog.GetEntityListWithLabel(RewardLabel, Mylist);
		SCR_EntityCatalogEntry entry = Mylist.GetRandomElement();
		reward = entry.GetPrefab();
		return true;
	};
	void CreateDescritions(){};
	//------------------------------------------------------------------------------------------------------------//
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
			SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("%1 %2 added to your inventory, and your reputation has improved", Movedamount.ToString(), FilePath.StripPath(reward)));
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	bool CompleteTask(IEntity Assignee)
	{
		if (GiveReward(Assignee))
		{
			e_State = ETaskState.COMPLETED;
			m_Copletionist = Assignee;
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	ETaskState GetState(){return e_State;};
	//------------------------------------------------------------------------------------------------------------//
	string GetTaskDescription(){return TaskDesc;}
	//------------------------------------------------------------------------------------------------------------//
	string GetTaskDiag(){return TaskDiag;}
	//------------------------------------------------------------------------------------------------------------//
	void AssignCharacter(IEntity Character)
	{
		a_TaskAssigned.Insert(Character);
		if(a_TaskAssigned.Count() > 0 && e_State == ETaskState.UNASSIGNED)
		{
			e_State = ETaskState.ASSIGNED;
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	bool CharacterAssigned(IEntity Character)
	{
		if(a_TaskAssigned.Contains(Character))
		{
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------------------------------------------------//
	void UpdateState(){};
	//------------------------------------------------------------------------------------------------------------//
	typename GetClassName(){return SP_Task;}
	//------------------------------------------------------------------------------------------------------------//
	void DeleteLeftovers(){};
	//------------------------------------------------------------------------------------------------------------//
};
//------------------------------------------------------------------------------------------------------------//
enum ETaskState
{
	UNASSIGNED,
	ASSIGNED,
	COMPLETED,
	FAILED,
	EMPTY
}