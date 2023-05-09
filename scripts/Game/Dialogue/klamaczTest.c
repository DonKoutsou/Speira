class SCR_AIConverseBehavior : SCR_AIBehaviorBase
{
	protected ref SCR_BTParam<vector> m_vPosition = new SCR_BTParam<vector>("Position");
	protected ref SCR_BTParam<float> m_fDuration = new SCR_BTParam<float>("Duration");	// Initialize in derived class
	protected ref SCR_BTParam<float> m_fRadius = new SCR_BTParam<float>("Radius");		// Initialize in derived class
	//protected ref SCR_BTParam<bool> m_bUseBinoculars = new SCR_BTParam<bool>("UseBinoculars"); // Initialize in derived class
	
	protected float m_fDeleteActionTime_ms;	// Initialize in derived class by InitTimeout()
	
	bool m_bActiveConversation = false;
	
	//------------------------------------------------------------------------------------------------------------------------
	void InitParameters(vector position)
	{
		m_vPosition.Init(this, position);
		m_fDuration.Init(this, 5000);
		m_fRadius.Init(this, 0);
		//m_bUseBinoculars.Init(this, false);
	}
	
	// posWorld - position to observe
	void SCR_AIConverseBehavior(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, vector posWorld)
	{
		InitParameters(posWorld);
		if (!utility)
			return;
				
		m_sBehaviorTree = "{082A95ACDC49DBAB}AI/BehaviorTrees/klamaczTest.bt";
		m_bAllowLook = true; // Disable standard looking
		m_bResetLook = true;
		m_bUniqueInActionQueue = true;
		
		m_bActiveConversation = true;
	}
	
	void SetActiveConversation(bool state)
	{
		m_bActiveConversation = state;
	}
	
	override float Evaluate()
	{
		// Fail action if timeout has been reached
		//float currentTime_ms = GetGame().GetWorld().GetWorldTime();
		//if (currentTime_ms > m_fDeleteActionTime_ms)
		//{
		//	Fail();
		//	return 0;
		//}
		//return m_fPriority;
		if (m_bActiveConversation)
			return 100;
		
		Fail();
		return 0;
	}
	
	void InitTimeout(float timeout_s)
	{
		float currentTime_ms = GetGame().GetWorld().GetWorldTime(); // Milliseconds!
		m_fDeleteActionTime_ms = currentTime_ms + 10 * timeout_s;
	}
};

class SCR_AIGetConverseParameters : SCR_AIGetActionParameters
{
	static ref TStringArray s_aVarsOut = (new SCR_AIConverseBehavior(null, null, Vector(0,0,0))).GetPortNames();
	override TStringArray GetVariablesOut()
	{
		return s_aVarsOut;
	}
	
	protected override bool VisibleInPalette() { return true; }
};

class SCR_ConverseAction : ScriptedUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// how to tell AI to run behavior
		AIControlComponent comp = AIControlComponent.Cast(pOwnerEntity.FindComponent(AIControlComponent));
		if (!comp)
			return;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return;
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return;
		
		SCR_AIConverseBehavior action = new SCR_AIConverseBehavior(utility, null, pUserEntity.GetOrigin());
		utility.AddAction(action);

	}
	override bool CanBeShownScript(IEntity user)
	{
		return true;
	}	
		
	void StopConversation(IEntity pOwnerEntity)
	{
		// how to tell AI to run behavior
		AIControlComponent comp = AIControlComponent.Cast(pOwnerEntity.FindComponent(AIControlComponent));
		if (!comp)
			return;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return;
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return;
		
		utility.SetStateAllActionsOfType(SCR_AIConverseBehavior, EAIActionState.FAILED, true);
		
		// or
		SCR_AIConverseBehavior action = SCR_AIConverseBehavior.Cast(utility.FindActionOfType(SCR_AIConverseBehavior));
		action.SetActiveConversation(false);
	}
};