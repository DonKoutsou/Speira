class SP_DialogueAction : ScriptedUserAction
{	

	[Attribute()]
	ref BaseChatChannel m_ChatChannel;
	[Attribute()]
	int CharID;
	[Attribute()]
	int ActionBranchID;
	
	int ActionStageID = 0;
	
	protected int atcionstoshow;
	protected SP_DialogueComponent DiagComp;
	protected SCR_BaseGameMode GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		//bool isbranching dialogue ? if false continue
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		if (DiagComp)
		{
			DiagComp.SendText(pOwnerEntity, pUserEntity, m_ChatChannel, CharID, ActionBranchID, ActionStageID);
			DiagComp.IncrementDiagStage(CharID, ActionBranchID, 1);
			ActionStageID = ActionStageID + 1;
		}
		
		return;
	}
	
	override bool CanBeShownScript(IEntity user)
	{
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		if (DiagComp.GetDiagStage(CharID) >= ActionStageID)
			{
				return true;
			}
		else
			return false;
	}
	override event bool GetActionNameScript(out string outName)
	{
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		
		if (DiagComp)
		{
			outName = DiagComp.GetActionName(CharID, ActionBranchID, ActionStageID);
			return true;
		}
		else
			return false;
	}
	
}