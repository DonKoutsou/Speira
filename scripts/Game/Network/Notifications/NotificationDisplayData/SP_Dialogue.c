[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ENotification, "m_NotificationKey")]
class SP_NotificationDialogue : SCR_NotificationPlayer
{
	override string GetText(SCR_NotificationData data)
	{
		string m_sDialogueText
		
		data.SetNotificationTextEntries(m_sDialogueText);
		return super.GetText(data,);
	}
	
};