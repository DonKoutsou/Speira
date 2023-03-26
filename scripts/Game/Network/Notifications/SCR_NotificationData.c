/*!
Saves data related to the notification system. 
Data saved: ENotification id, float m_fTime, notification string parameters, A link to SCR_NotificationDisplayData, m_FactionRelatedColor if applicable and ints to store data
See specific SCR_NotificationDisplayData classes to know which parameters should be send
*/
class SCR_NotificationData
{
	//--- Local params
	protected ENotification m_iID;
	protected float m_fNotificationTimeLeft = -1;
	protected int m_iPrevPositionUpdated = -1;
	 string notificationEntry1, notificationEntry2, notificationEntry3, notificationEntry4, notificationEntry5;
	protected bool m_bIsSplitNotification;
	protected ENotificationColor m_FactionRelatedColor = ENotificationColor.NEUTRAL;
	protected ENotificationColor m_FactionRelatedTextColor = ENotificationColor.NEUTRAL;
	protected ENotificationColor m_FactionRelatedTextLeftColor = ENotificationColor.NEUTRAL;
	protected ENotificationColor m_FactionRelatedTextRightColor = ENotificationColor.NEUTRAL;
	protected SCR_NotificationDisplayData m_DisplayData;
	protected static string m_sDialogueText;
	
	//--- Replicated params
	protected int m_iParam1;
	protected int m_iParam2;
	protected int m_iParam3;
	protected int m_iParam4;
	protected int m_iParam5;
	
	protected vector m_vPosition = vector.Zero;
	protected ENotificationReceiver m_iNotificationReceiverType;
	
	/*!
	Set notification's meta data: Faction color and initial position data
	\param id Notification ID
	\param notificationText saved text of notification
	\param notificationTarget target of notification for editor
	\param location target of notification for editor
	*/
	void SetMeta(int id, SCR_NotificationDisplayData displayData)
	{
		m_iID = id;

		//Set how much time left before notification will be deleted from data
		m_fNotificationTimeLeft = SCR_NotificationsComponent.NOTIFICATION_DELETE_TIME;
		
		m_DisplayData = displayData;
		
		if (m_DisplayData)
		{
			if (!m_DisplayData.m_info)
			{
				Print("(" + typename.EnumToString(ENotification, GetID()) + ") SCR_NotificationDisplayData has no UIInfo assigned! This will cause errors!", LogLevel.ERROR); 
				m_DisplayData.m_info = new SCR_UINotificationInfo;
			} 
			
			//Set any initial local data such as initial notification position and faction related notification color
			m_DisplayData.SetInitialDisplayData(this);
		}	
	}
	
	/*!
	Set notification ID
	\return Notification ID
	*/
	int GetID()
	{
		return m_iID;
	}
	static void SetDialogueText(string dialoguetext)
	{
		dialoguetext = m_sDialogueText;
	}
	string GetDialogueText()
	{
		return m_sDialogueText;
	}
	
	/*!
	Get who recieves the notification and how, like local only, GM only, Local GM only
	\return ENotificationReceiver which holds the notification receiver type
	*/
	ENotificationReceiver GetNotificationReceiverType()
	{
		return m_iNotificationReceiverType;
	}
	
	/*!
	Get how much time is left before notification data is deleted from SCR_NotificationsComponent array
	\return Time in miliseconds
	*/
	float GetNotificationTimeLeft()
	{
		return m_fNotificationTimeLeft;
	}
	
	/*!
	Update the time left for the notification. It will be removed from the data in the Notification component once it reaches 0
	\param timeSlice amount removed from time left
	\return returns true if the time left reaches 0 or less
	*/
	bool UpdateNotificationData(float timeSlice)
	{
		m_fNotificationTimeLeft -= timeSlice;
		
		//Update Position ever second the notification data exists
		if (m_DisplayData && m_DisplayData.GetNotificationUIInfo().GetEditorSetPositionData() == ENotificationSetPositionData.AUTO_SET_AND_UPDATE_POSITION)
		{
			if (m_iPrevPositionUpdated != (int)m_fNotificationTimeLeft)
			{
				m_iPrevPositionUpdated = (int)m_fNotificationTimeLeft;
				m_DisplayData.SetPosition(this);
			}
		}
		
		//No time left so delete
		return (m_fNotificationTimeLeft <= 0);
	}
	
	/*!
	Get the display text for the notification
	\return string display text
	*/
	string GetText()
	{
		if (m_DisplayData)
		{
			return m_DisplayData.GetText(this);
		}
		else 
		{
			Print("(" + typename.EnumToString(ENotification, GetID()) + ") SCR_NotificationDisplayData has no m_DisplayData assigned!", LogLevel.ERROR); 
			return  typename.EnumToString(ENotification, GetID());
		}
	}
	
	/*!
	Set the notification entry texts
	\param entry1
	\param entry2
	\param entry3
	\param entry4
	\param entry5
	*/
	void SetNotificationTextEntries(string entry1 = string.Empty, string entry2 = string.Empty, string entry3 = string.Empty, string entry4 = string.Empty, string entry5 = string.Empty)
	{
		if (entry1 != string.Empty)
			notificationEntry1 = entry1;
		if (entry2 != string.Empty)
			notificationEntry2 = entry2;
		if (entry3 != string.Empty)
			notificationEntry3 = entry3;
		if (entry4 != string.Empty)
			notificationEntry4 = entry4;
		if (entry5 != string.Empty)
			notificationEntry5 = entry5;
	}
	
	/*!
	Set the notification entry texts
	\param out entry1
	\param out entry2
	\param out entry3
	\param out entry4
	\param out entry5
	*/
	void GetNotificationTextEntries(out string entry1, out string entry2, out string entry3, out string entry4, out string entry5)
	{
		entry1 = notificationEntry1;
		entry2 = notificationEntry2;
		entry3 = notificationEntry3;
		entry4 = notificationEntry4;
		entry5 = notificationEntry5;
	}
	

	/*!
	Set the faction color. 
	Faction color is used to set the color of notification that can have faction variant colors depending on the target of the notification.
	So friendly player died will have a diffrent color then enemy player died
	\param factionPositiveNegativeColor the new faction color to set
	*/
	void SetFactionRelatedColor(ENotificationColor factionRelatedColor)
	{
		m_FactionRelatedColor = factionRelatedColor;
	}
	
	/*!
	Set the faction color for text. 
	Faction color is used to set the color of notification that can have faction variant colors depending on the target of the notification.
	So friendly player died will have a diffrent color then enemy player died
	\param factionPositiveNegativeColor the new faction color to set for text
	*/
	void SetFactionRelatedTextColor(ENotificationColor factionRelatedColor)
	{
		m_FactionRelatedTextColor = factionRelatedColor;
	}
	
	/*!
	Set the faction color for splitNotifications. 
	Faction color is used to set the color of notification that can have faction variant colors depending on the targets (param1 and param2) of the notification.
	So friendly player died will have a diffrent color then enemy player died
	\param factionRelatedColorLeft the new faction color to set for left text
	\param factionRelatedColorRight the new faction color to set for right text
	*/
	void SetSplitFactionRelatedColor(ENotificationColor factionRelatedColorLeft, ENotificationColor factionRelatedColorRight)
	{
		m_FactionRelatedTextLeftColor = factionRelatedColorLeft;
		m_FactionRelatedTextRightColor = factionRelatedColorRight;
	}
	
	/*!
	Get the saved faction color. 
	Used to set the color of notification that can have faction variant colors depending on the target of the notification.
	So friendly player died will have a diffrent color then enemy player died
	\return m_factionColor
	*/
	ENotificationColor GetFactionRelatedColor()
	{
		return m_FactionRelatedColor;
	}
	
	/*!
	Get the saved faction text color. 
	Used to set the color of notification that can have faction variant colors depending on the target of the notification.
	So friendly player died will have a diffrent color then enemy player died
	\return faction related text color
	*/
	ENotificationColor GetFactionRelatedTextColor()
	{
		return m_FactionRelatedTextColor;
	}
	
		
	/*!
	Get the saved faction text color. 
	Used to set the color of notification that can have faction variant colors depending on the target of the notification.
	So friendly player died will have a diffrent color then enemy player died
	\return faction related text color
	*/
	void GetSplitFactionRelatedColor(out ENotificationColor leftTextColor, out ENotificationColor rightTextColor)
	{
		leftTextColor = m_FactionRelatedTextLeftColor;
		rightTextColor = m_FactionRelatedTextRightColor;
	}
	
	/*!
	Get the specific linked SCR_NotificationDisplayData type to get display data and position
	\return SCR_NotificationDisplayData DisplayData
	*/
	SCR_NotificationDisplayData GetDisplayData()
	{
		return m_DisplayData;
	}
	
	/*!
	Get position of the notification.
	\param[out] position
	*/
	void GetPosition(out vector position)
	{
		position = m_vPosition;
	}
	
	/*!
	Get parameters of the notification.
	\param[out] param1
	\param[out] param2
	\param[out] param3
	\param[out] param4
	\param[out] param5
	*/
	void GetParams(out int param1, out int param2 = 0, out int param3 = 0, out int param4 = 0, out int param5 = 0)
	{
		param1 = m_iParam1;
		param2 = m_iParam2;
		param3 = m_iParam3;
		param4 = m_iParam4;
		param5 = m_iParam5;
	}

	/*!
	Log information about the notification.
	*/
	void Log()
	{
		PrintFormat("%1\nID: %2\nParams: %3, %4, %5, %6, %7", this, typename.EnumToString(ENotification, m_iID), m_iParam1, m_iParam2, m_iParam3, m_iParam4, m_iParam5);
	}
	
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx hint, ScriptBitSerializer packet) 
	{
		snapshot.Serialize(packet, 36);
	}
	static bool Decode(ScriptBitSerializer packet, ScriptCtx hint, SSnapSerializerBase snapshot) 
	{
		return snapshot.Serialize(packet, 36);
	}
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx hint) 
	{
		return lhs.CompareSnapshots(rhs, 36);
	}
	static bool PropCompare(SCR_NotificationData prop, SSnapSerializerBase snapshot, ScriptCtx hint) 
	{
		return snapshot.Compare(prop.m_iParam1, 4) 
			&& snapshot.Compare(prop.m_iParam2, 4) 
			&& snapshot.Compare(prop.m_iParam3, 4) 
			&& snapshot.Compare(prop.m_iParam4, 4) 
			&& snapshot.Compare(prop.m_iParam5, 4)
			&& snapshot.Compare(prop.m_vPosition, 12)
			&& snapshot.Compare(prop.m_iNotificationReceiverType, 4);
	}
	static bool Extract(SCR_NotificationData prop, ScriptCtx hint, SSnapSerializerBase snapshot) 
	{
		snapshot.SerializeBytes(prop.m_iParam1, 4);
		snapshot.SerializeBytes(prop.m_iParam2, 4);
		snapshot.SerializeBytes(prop.m_iParam3, 4);
		snapshot.SerializeBytes(prop.m_iParam4, 4);
		snapshot.SerializeBytes(prop.m_iParam5, 4);
		snapshot.SerializeBytes(prop.m_vPosition, 12);
		snapshot.SerializeBytes(prop.m_iNotificationReceiverType, 4);
		return true;
	}
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx hint, SCR_NotificationData prop) 
	{
		snapshot.SerializeBytes(prop.m_iParam1, 4);
		snapshot.SerializeBytes(prop.m_iParam2, 4);
		snapshot.SerializeBytes(prop.m_iParam3, 4);
		snapshot.SerializeBytes(prop.m_iParam4, 4);
		snapshot.SerializeBytes(prop.m_iParam5, 4);
		snapshot.SerializeBytes(prop.m_vPosition, 12);
		snapshot.SerializeBytes(prop.m_iNotificationReceiverType, 4);
		return true;
	}
	
	/*!
	Set position of the notification.
	\param position
	*/
	void SetPosition(vector position)
	{
		m_vPosition = position;
	}
	
	/*!
	Sets the Parameters
	\param receiverType, a general type of who will will receive the notification such as: GM only, local only, etc
	\param param1
	\param param2
	\param param3
	\param param4
	\param param5
	*/
	void SetParameters(ENotificationReceiver receiverType, int param1 = 0, int param2 = 0, int param3 = 0, int param4 = 0, int param5 = 0)
	{
		m_iNotificationReceiverType = receiverType;
		m_iParam1 = param1;
		m_iParam2 = param2;
		m_iParam3 = param3;
		m_iParam4 = param4;
		m_iParam5 = param5;
	}
	void SetParametersNSt(ENotificationReceiver receiverType, string dialoguetext, int param1 = 0, int param2 = 0, int param3 = 0, int param4 = 0, int param5 = 0)
	{
		m_iNotificationReceiverType = receiverType;
		notificationEntry1 = dialoguetext;
		m_iParam1 = param1;
		
	}
	
};