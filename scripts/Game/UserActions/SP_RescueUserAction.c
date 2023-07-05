class SP_RescueAction : ScriptedUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SP_DialogueComponent diag = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		FactionKey senderFaction = diag.GetCharacterFaction(pOwnerEntity).GetFactionKey();
		BaseChatChannel Channel;
		switch (senderFaction)
			{
				case "FIA":
					Channel = diag.m_ChatChannelFIA;
				break;
				case "USSR":
					Channel = diag.m_ChatChannelUSSR;
				break;
				case "BANDITS":
					Channel = diag.m_ChatChannelBANDITS;
				break;
				case "SPEIRA":
					Channel = diag.m_ChatChannelSPEIRA;
				break;
				case "US":
					Channel = diag.m_ChatChannelUS;
				break;
			}
		SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_CharacterDamageManagerComponent));
		dmg.SetPermitUnconsciousness(true, true);
		dmg.UpdateConsciousness();
		SP_RequestManagerComponent requestman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		array<ref SP_Task> MyTasks = new array<ref SP_Task>();
		requestman.GetRescueTask(pOwnerEntity, MyTasks);
		for (int i, count = MyTasks.Count(); i < count; i++)
		{
			SP_RescueTask resctask = SP_RescueTask.Cast(MyTasks[i]);
			resctask.OnCharacterRescued(pOwnerEntity);
			if(MyTasks[i].ReadyToDeliver(pOwnerEntity, pUserEntity))
			{
				if(MyTasks[i].CompleteTask(pUserEntity))
				{
					diag.SendText("Thanks for helping us, here is your reward", Channel, 0, diag.GetCharacterName(pOwnerEntity), diag.GetCharacterRankName(pOwnerEntity));
					return;
				}
			}
			diag.SendText("Thanks for the help, please help the others", Channel, 0, diag.GetCharacterName(pOwnerEntity), diag.GetCharacterRankName(pOwnerEntity));
		}
	}
	override bool CanBeShownScript(IEntity user)
	{
		IEntity owner = IEntity.Cast(GetOwner());
		SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
		if(dmg.GetIsUnconscious() == false)
		{
			return false;
		}
		SP_RequestManagerComponent requestman = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		array<ref SP_Task> MyTasks = new array<ref SP_Task>();
		requestman.GetRescueTask(owner, MyTasks);
		if(MyTasks.Count() > 0)
		{
			return true;
		}
		return false;
	};
};