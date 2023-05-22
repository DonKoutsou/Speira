 class SCR_AIDecoTestFireIsLit : DecoratorTestScripted
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