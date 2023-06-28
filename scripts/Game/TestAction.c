class TestAction : ScriptedUserAction
{
	override event void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		AIControlComponent ContComp = AIControlComponent.Cast(pOwnerEntity.FindComponent(AIControlComponent));
		AIAgent Agent = ContComp.GetAIAgent().GetParentGroup();

		AICommunicationComponent mailbox = Agent.GetCommunicationComponent();
		if (!mailbox)
			return;
		IEntity targetEntity = GetGame().GetPlayerController().GetControlledEntity();
		
		SCR_AIMessage_GroupAttack msg = new SCR_AIMessage_GroupAttack();
		msg.SetReceiver(Agent);
		msg.m_fPriorityLevel = 10000;
		msg.SetObject(targetEntity);
		
		msg.m_TargetInfo = new SCR_AITargetInfo(targetEntity, targetEntity.GetOrigin(), 10);	
		mailbox.RequestBroadcast(msg, Agent);
	}
}