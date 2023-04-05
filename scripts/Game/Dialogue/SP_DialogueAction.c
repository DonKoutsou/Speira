class SP_DialogueAction : ScriptedUserAction
{
	//------------------------------------------------------------------//
	[Attribute(defvalue: "0", desc: "Branch wich this action belongs to. Configs with same branch will be used on this action.", category: "Dialogue")]
	int ActionBranchID;
	[Attribute(defvalue: "1", desc: "Increment Amount, by how much should stage be progressed", category: "Dialogue")]
	protected int m_bIncrementAmount;
	//------------------------------------------------------------------//
	protected SP_DialogueComponent DiagComp;
	protected SCR_BaseGameMode GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	protected IEntity Owner;
	//------------------------------------------------------------------//
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		if (DiagComp)
		{
			//DoDialogue function on dialogue component, sending all gathered data from action
			DiagComp.DoDialogue(pOwnerEntity, pUserEntity, ActionBranchID, m_bIncrementAmount);
		}
		return;
	}
	//------------------------------------------------------------------//
	//if string that comes back from config is empty means that dialogue is finished so hide action
	override bool CanBeShownScript(IEntity user)
	{
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		string outName;
		if (DiagComp)
		{
			outName = DiagComp.GetActionName(ActionBranchID, Owner);	
		}
		if (outName == STRING_EMPTY)
			{
				return false;
			}
			else
				return true;
	}
	//------------------------------------------------------------------//
	//looks for the name of the action in the dialogue config
	override event bool GetActionNameScript(out string outName)
	{
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		
		
		if (DiagComp)
		{
			outName = DiagComp.GetActionName(ActionBranchID, Owner);
			
		}
		if (outName == STRING_EMPTY)
			{
				return false;
			}
			else
				return true;
	}
	//------------------------------------------------------------------//
	override event void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		Owner = IEntity.Cast(GetOwner());
	}
	//------------------------------------------------------------------//
}