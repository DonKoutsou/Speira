 class SCR_AIDecoTestFireIsLit : DecoratorTestScripted
{
	private SCR_FireplaceComponent m_FireplaceComponent;
	
	//------------------------------------------------------------------------------------------------
	protected override bool TestFunction(AIAgent agent, IEntity controlled)
	{	
		if (controlled)
		{	
			m_FireplaceComponent = SCR_FireplaceComponent.Cast(controlled.FindComponent(SCR_FireplaceComponent));
			if (!m_FireplaceComponent)
				return false;
			bool light = m_FireplaceComponent.IsOn();
			return light;
		}
		return false;
	}
};
 class SCR_AIDecoTestRadioIsLit : DecoratorTestScripted
{
	private RadioBroadcastComponent m_RadioComponent;
	
	//------------------------------------------------------------------------------------------------
	protected override bool TestFunction(AIAgent agent, IEntity controlled)
	{	
		if (controlled)
		{	
			m_RadioComponent = RadioBroadcastComponent.Cast(controlled.FindComponent(RadioBroadcastComponent));
			if (!m_RadioComponent)
				return false;
			bool radio = m_RadioComponent.GetState();
			return radio;
		}
		return false;
	}
};
 class SCR_AIDecoTestLightIsLit : DecoratorTestScripted
{
	private SCR_BaseInteractiveLightComponent m_LightComponent;
	
	//------------------------------------------------------------------------------------------------
	protected override bool TestFunction(AIAgent agent, IEntity controlled)
	{	
		if (controlled)
		{	
			m_LightComponent = SCR_BaseInteractiveLightComponent.Cast(controlled.FindComponent(SCR_BaseInteractiveLightComponent));
			if (!m_LightComponent)
				return false;
			bool light = m_LightComponent.IsOn();
			return light;
		}
		return false;
	}
};