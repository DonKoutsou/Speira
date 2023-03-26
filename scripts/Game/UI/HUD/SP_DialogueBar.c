class SP_DialogueBarUI: SCR_InfoDisplay
{
	private TextWidget m_sDialogueText = null;
	private SCR_CharacterControllerComponent m_cCharacterController = null;

	void OnTalk(string value)
	{
		m_sDialogueText.SetText(value);
	}
}