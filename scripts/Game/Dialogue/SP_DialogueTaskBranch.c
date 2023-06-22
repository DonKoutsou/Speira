
[BaseContainerProps(configRoot:true),  DialogueBranchConfigTitleAttribute()]
class SP_DialogueTaskBranch : SP_DialogueBranch
{
	override bool CheckIfStageBranches(IEntity Character)
	{
		SP_TaskManagerComponent taskman = SP_TaskManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_TaskManagerComponent));
		return taskman.CharHasTask(Character);
	}
	override void CauseBranch(int BranchID, IEntity Character)
	{
		DialogueBranchInfo Conf = LocateConfig(Character);
		SP_TaskManagerComponent taskman = SP_TaskManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_TaskManagerComponent));
		array<ref SP_Task> Tasks;
		taskman.GetCharTasks(Character, Tasks);
		foreach(SP_Task task : Tasks)
		{
			SP_BountyTask Btask = SP_BountyTask.Cast(task);
			SP_DeliverTask Dtask = SP_DeliverTask.Cast(task);
			if(Btask)
			{
				DialogueStageTakeBountyAction stage = DialogueStageTakeBountyAction.Cast(CreateStage(E_TASKTYPE.BOUNTY));
				stage.SetActionText(Btask.GetTaskDescription());
				stage.InheritData(Conf.OriginalArchetype, Conf, Character);
			}
			if(Dtask)
			{
				DialogueStageTakePackageAction stage = DialogueStageTakePackageAction.Cast(CreateStage(E_TASKTYPE.DELIVER));
				stage.SetActionText(Dtask.GetTaskDescription());
				stage.InheritData(Conf.OriginalArchetype, Conf, Character);
			}
		}
		Conf.CauseBranch(BranchID);
	}
	DialogueStage CreateStage(E_TASKTYPE task)
	{
		DialogueStage S_NewStage = new DialogueStage;
		if(task == E_TASKTYPE.DELIVER)
		{
				S_NewStage = DialogueStageTakePackageAction.Cast(S_NewStage);
				m_BranchStages.Insert(S_NewStage);
		}
		if(task == E_TASKTYPE.BOUNTY)
		{
				S_NewStage = DialogueStageTakeBountyAction.Cast(S_NewStage);
				m_BranchStages.Insert(S_NewStage);
		}
		return S_NewStage;
	};
	override void GetDialogueText(IEntity Character, IEntity Player, out string DiagText)
	{
		DialogueBranchInfo Conf = LocateConfig(Character);
		
		int Bstage;
		Conf.GetDialogueBranchStage(Bstage);
		if(m_BranchStages.Count() >= Bstage)
		{
			DiagText = m_BranchStages[Bstage].GetStageDialogueText(Character, Player);
		}
	}
	
};
enum E_TASKTYPE
{
	DELIVER,
	BOUNTY
}