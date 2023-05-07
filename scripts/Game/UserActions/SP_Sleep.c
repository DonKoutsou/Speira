class SP_SleepAction : ScriptedUserAction
{
	[Attribute()]
	float m_Sleepamount;
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		TimeAndWeatherManagerEntity TnWManager = GetGame().GetTimeAndWeatherManager();
		TimeContainer currenttime = TnWManager.GetTime();
		int TimeToSet = currenttime.m_iHours + m_Sleepamount;
		if (TimeToSet > 24)
		{
			TimeToSet = TimeToSet - 24;
		}
		TnWManager.SetTimeOfTheDay(TimeToSet, true);
	}
	
}