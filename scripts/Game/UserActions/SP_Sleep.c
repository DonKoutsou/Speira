class SP_SleepAction : ScriptedUserAction
{
	[Attribute()]
	float m_Sleepamount;
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		TimeAndWeatherManagerEntity TnWManager = GetGame().GetTimeAndWeatherManager();
		int TimeToWakeUP = TnWManager.GetTime().m_iHours + m_Sleepamount;
		int DayToWakeUp = TnWManager.GetDay();
		if(TimeToWakeUP > 24)
		{
			TimeToWakeUP = DayToWakeUp - 24;
			DayToWakeUp = DayToWakeUp + 1;
			TnWManager.SetTimeOfTheDay(TimeToWakeUP, true);
			TnWManager.SetDate(TnWManager.GetYear(), TnWManager.GetMonth(), DayToWakeUp, true);
		}
		else
		{
			TnWManager.SetTimeOfTheDay(TimeToWakeUP, true);
		}
		
	}
	
}