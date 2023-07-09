//------------------------------------------------------------------------------------------------------------//
[BaseContainerProps(configRoot:true)]
class SP_NavigateTask: SP_Task
{
	//----------------------------------------------------------------------------------//
	[Attribute(defvalue: "20")]
	int m_iRewardAverageAmount;
	//----------------------------------------------------------------------------------//
	[Attribute(defvalue: "20", desc: "Distance from target at wich task can be considered ready to deliver")]
	int m_iSuccessDistance;
	//------------------------------------------------------------------------------------------------------------//
	//In navigate task we look for random owner
	override bool FindOwner(out IEntity Owner)
	{
		SP_AIDirector MyDirector = SP_AIDirector.AllDirectors.GetRandomElement();
		if (!MyDirector.GetRandomUnit(Owner))
		{
			return false;
		}
		if(Owner)
		{
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Target needs to be from same faction
	override bool FindTarget(out IEntity Target)
	{
		AIControlComponent comp = AIControlComponent.Cast(GetOwner().FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector MyDirector = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		SP_AIDirector NewDirector;
		FactionManager factionManager = FactionManager.Cast(GetGame().GetFactionManager());
		string keyunused;
		Faction DirectorFaction = MyDirector.GetMajorityHolder(keyunused);
		if (!DirectorFaction)
		{
			return false;
		}
		FactionKey key = DirectorFaction.GetFactionKey();
		SCR_Faction myFaction = SCR_Faction.Cast(factionManager.GetFactionByKey(key));
		if (!MyDirector.GetDirectorOccupiedByFriendly(myFaction, NewDirector))
		{
			return false;
		}
		if (MyDirector == NewDirector)
		{
			return false;
		}
		if (!NewDirector.GetRandomUnitByFKey(key, Target))
		{
			return false;
		}
		if (Target == GetOwner())
		{
			return false;
		}
		if(Target)
		{
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//Its ready to deliver when targer and owner are closer than m_iSuccessDistance
	override bool ReadyToDeliver(IEntity TalkingChar, IEntity Assignee)
	{
		float dis = vector.Distance(TaskTarget.GetOrigin(), TalkingChar.GetOrigin());
		if(!m_iSuccessDistance)
		{
			SP_RequestManagerComponent ReqMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			SP_NavigateTask tasksample = SP_NavigateTask.Cast(ReqMan.GetTaskSample(SP_NavigateTask));
			m_iSuccessDistance = tasksample.GetSuccessDistance();
		}
		if(dis <= m_iSuccessDistance)
		{
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	//overriding AssignReward to apply the average attribute from SP_RequestManagerComponent
	override bool AssignReward()
	{
		EEditableEntityLabel RewardLabel;
		int index = Math.RandomInt(0,2);
		if(index == 0)
		{
			RewardLabel = EEditableEntityLabel.ITEMTYPE_CURRENCY;
			SP_RequestManagerComponent ReqMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
			if(!ReqMan)
			{
				return false;
			}
			SP_NavigateTask tasksample = SP_NavigateTask.Cast(ReqMan.GetTaskSample(SP_NavigateTask));
			if(!tasksample)
			{
				return false;
			}
			m_iRewardAverageAmount = tasksample.GetRewardAverage();
			if(m_iRewardAverageAmount)
			{
				m_iRewardAmount = Math.Floor(Math.RandomFloat(m_iRewardAverageAmount/2, m_iRewardAverageAmount + m_iRewardAverageAmount/2));
			}
			else
			{
				m_iRewardAmount = Math.RandomInt(5, 15)
			}
			
		}
		if(index == 1)
		{
			RewardLabel = EEditableEntityLabel.ITEMTYPE_WEAPON;
			m_iRewardAmount = 1;
		}
		SCR_EntityCatalogManagerComponent Catalog = SCR_EntityCatalogManagerComponent.GetInstance();
		if(!Catalog)
			{
				Print("Cant find catalog, task creation failed in Assign reward");
				return false;
			}
		SCR_EntityCatalog RewardsCatalog = Catalog.GetEntityCatalogOfType(EEntityCatalogType.REWARD);
		if(!RewardsCatalog)
			{
				Print("Rewards missing from entity catalog");
				return false;
			}
		array<SCR_EntityCatalogEntry> Mylist = new array<SCR_EntityCatalogEntry>();
		RewardsCatalog.GetEntityListWithLabel(RewardLabel, Mylist);
		SCR_EntityCatalogEntry entry = Mylist.GetRandomElement();
		reward = entry.GetPrefab();
		if(!reward)
			{
				return false;
			}
		return true;
	};
	//------------------------------------------------------------------------------------------------------------//
	void GetInfo(out string OName, out string DName, out string OLoc, out string DLoc)
	{
		if (!TaskOwner || !TaskTarget)
		{
			return;
		}
		SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
		AIControlComponent comp = AIControlComponent.Cast(TaskTarget.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector Director = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(TaskOwner.FindComponent(SCR_CharacterRankComponent));
		OName = CharRank.GetCharacterRankName(TaskOwner) + " " + Diag.GetCharacterName(TaskOwner);
		DName = CharRank.GetCharacterRankName(TaskTarget) + " " + Diag.GetCharacterName(TaskTarget);
		OLoc = Director.GetCharacterLocation(TaskOwner);
		DLoc = Director.GetCharacterLocation(TaskTarget);
	};
	//------------------------------------------------------------------------------------------------------------//
	override void CreateDescritions()
	{
		string OName;
		string DName;
		string DLoc;
		string OLoc;
		GetInfo(OName, DName, OLoc, DLoc);
		TaskDesc = string.Format("Navigate %1, to %2's location. %2 is located on %3. Reward is %4 %5", OName, DName, DLoc, m_iRewardAmount, FilePath.StripPath(reward));
		TaskDiag = string.Format("I am looking for someone to help me navigate to %1 on %2. Come find me on %3. Reward is %4 %5", DName, DLoc, OLoc, m_iRewardAmount, FilePath.StripPath(reward));
		TaskTitle = string.Format("Navigate: escort %1 to %2's location", OName, DName);
	};
	//------------------------------------------------------------------------------------------------------------//
	override void UpdateState()
	{
		SCR_CharacterDamageManagerComponent DmgComp = SCR_CharacterDamageManagerComponent.Cast(TaskOwner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (DmgComp.IsDestroyed())
		{
			if(m_TaskMarker)
			{
				m_TaskMarker.Fail(true);
				delete m_TaskMarker;
			}
			e_State = ETaskState.FAILED;
			return;
		}
	}
	//------------------------------------------------------------------------------------------------------------//
	//to complete task need to assign owner to new group and get rid of WP
	override bool CompleteTask(IEntity Assignee)
	{
		if (GiveReward(Assignee))
		{
			m_TaskMarker.Finish(true);
			delete m_TaskMarker;
			SP_DialogueComponent Diag = SP_DialogueComponent.Cast(SP_GameMode.Cast(GetGame().GetGameMode()).GetDialogueComponent());
			//get task owner group and dissband it.
			AIControlComponent comp = AIControlComponent.Cast(TaskOwner.FindComponent(AIControlComponent));
			AIAgent agent = comp.GetAIAgent();
			SCR_AIGroup group = SCR_AIGroup.Cast(agent.GetParentGroup());
			AIWaypoint originalwp = group.GetCurrentWaypoint();
			group.CompleteWaypoint(originalwp);
			delete originalwp;
			group.RemoveAgent(agent);
			if(group)
			{
				delete group;
			}
			
			//get group of target
			AIControlComponent Tcomp = AIControlComponent.Cast(TaskTarget.FindComponent(AIControlComponent));
			AIAgent Tagent = Tcomp.GetAIAgent();
			SCR_AIGroup Tgroup = SCR_AIGroup.Cast(Tagent.GetParentGroup());
			
			//add owner
			Tgroup.AddAgent(agent);
			if (m_TaskMarker)
			{
				m_TaskMarker.Finish(true);
				delete m_TaskMarker;
			}
			SP_FactionManager factman = SP_FactionManager.Cast(GetGame().GetFactionManager());
			factman.OnTaskCompleted(this, Assignee);
			e_State = ETaskState.COMPLETED;
			m_Copletionist = Assignee;
			SCR_HintManagerComponent.GetInstance().ShowCustom(string.Format("%1 stopped following you", Diag.GetCharacterName(TaskOwner)));
			return true;
		}
		return false;
	};
	//------------------------------------------------------------------------------------------------------------//
	override typename GetClassName(){return SP_NavigateTask;};
	//------------------------------------------------------------------------------------------------------------//
	int GetRewardAverage()
	{
		if (m_iRewardAverageAmount)
		{
			return m_iRewardAverageAmount;
		}
		return null;
	};
	//------------------------------------------------------------------------------------------------------------//
	int GetSuccessDistance()
	{
		if (m_iSuccessDistance)
		{
			return m_iSuccessDistance;
		}
		return null;
	};
	//------------------------------------------------------------------------------------------------------------//
};