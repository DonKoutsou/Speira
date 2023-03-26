class SP_DialogueComponentClass: SCR_BaseGameModeComponentClass
{
};


class SP_DialogueComponent: SCR_BaseGameModeComponent
{
	[Attribute( defvalue: "", desc: "Description of the task", category: "Task",  )]			//TODO: make config, memory
	protected ref array<ref SP_CharacterArchetype> m_CharacterArchetypeList;
	
	protected string m_DialogTexttoshow;
	protected BaseChatChannel m_ChatChannel;
	protected int senderId;
	protected string senderName;
	protected SP_CharacterArchetype m_Character;
	protected SCR_BaseGameMode GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	SP_DialogueComponent DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
	int DiagCount;
	void SendText(IEntity pOwnerEntity, IEntity pUserEntity, BaseChatChannel ChatChannel, int CharID, int BranchID, int StageID)
	{
		
		for (int i, count = m_CharacterArchetypeList.Count(); i < count; i++)
		{
			if (m_CharacterArchetypeList[i].GetCharacterID() == CharID)
			{
				m_DialogTexttoshow = m_CharacterArchetypeList[i].GetDialogueText(BranchID, StageID);
				
			}
			
		}
		
		SCR_CharacterIdentityComponent IDcomp = SCR_CharacterIdentityComponent.Cast(pOwnerEntity.FindComponent(SCR_CharacterIdentityComponent));
		
		senderName = IDcomp.GetCharacterFullName();
		SCR_ChatPanelManager.GetInstance().ShowDiagMessage(m_DialogTexttoshow, ChatChannel, senderId, senderName);
	}
	string GetActionName(int CharID, int BranchID, int StageID)
	{
		string m_sActionName;
		
		for (int i, count = m_CharacterArchetypeList.Count(); i < count; i++)
		{
			if (m_CharacterArchetypeList[i].GetCharacterID() == CharID)
			{
				m_sActionName = m_CharacterArchetypeList[i].GetActionTitle(BranchID, StageID);
			}
		}
		return m_sActionName;
	}
	int GetDiagStage(int CharID)
	{
		int m_iDiagStage;
		
		for (int i, count = m_CharacterArchetypeList.Count(); i < count; i++)
		{
			if (m_CharacterArchetypeList[i].GetCharacterID() == CharID)
			{
				m_iDiagStage = m_CharacterArchetypeList[i].GetDiagStage();
			}
		}
		return m_iDiagStage;
	}
	bool IncrementDiagStage(int CharID, int BranchID, int incrementamount)
	{
		
		for (int i, count = m_CharacterArchetypeList.Count(); i < count; i++)
		{
			if (m_CharacterArchetypeList[i].GetCharacterID() == CharID)
			{
				m_CharacterArchetypeList[i].IncrementStage(BranchID, incrementamount);
			}
			
		}
		return false;
	}
	// bool isdiagbranched ? 

	
};