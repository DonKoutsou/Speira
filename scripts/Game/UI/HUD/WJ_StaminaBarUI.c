class WJ_StaminaBarUI: SCR_InfoDisplay
{
	private ProgressBarWidget m_wStaminaBar = null;
	private SCR_CharacterControllerComponent m_cCharacterController = null;

	void OnStaminaChange(float value)
	{
		if (!m_wStaminaBar)
		{
			m_wStaminaBar = ProgressBarWidget.Cast(m_wRoot.FindWidget("m_wStaminaBar"));
			if (!m_wStaminaBar) return;
		};
		
		m_wStaminaBar.SetCurrent(value);
	}
	
	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);

		m_wStaminaBar = ProgressBarWidget.Cast(m_wRoot.FindWidget("m_wStaminaBar"));
		IEntity player = GetGame().GetPlayerController();
		if (!player) {
			Print("no player found");
			return;
		}
		
		m_cCharacterController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
		if (m_cCharacterController) Print("Found character controllerc omponent")
	}
	
	override event void UpdateValues(IEntity owner, float timeSlice)
	{
		if (!m_cCharacterController)
		{
			IEntity player = SCR_PlayerController.GetLocalControlledEntity();
			if (!player) {
				return;
			}
		
			m_cCharacterController = SCR_CharacterControllerComponent.Cast(player.FindComponent(SCR_CharacterControllerComponent));
			if (!m_cCharacterController) return;
		};
		
		OnStaminaChange(m_cCharacterController.GetStamina());
	}
}