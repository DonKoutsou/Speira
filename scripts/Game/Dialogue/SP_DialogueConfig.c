[BaseContainerProps(configRoot: true)]
class SP_CharacterArchetype
{
	[Attribute()]
	protected int m_sCharacterID;
	[Attribute()]
	protected ref array<ref SP_DialogueConfig> DialogueConfig;
	protected SP_DialogueComponent DiagComp;
	static SP_CharacterArchetype CharacterArchetype;
	protected int GlobalDialogueStage = 0;
	
	
	bool GetCharacterID()
	{
		return m_sCharacterID;
	}
	bool IncrementStage(int BranchID, int incrementamount)
	{
		
		for (int i, count = DialogueConfig.Count(); i < count; i++)
		{
			//if (DialogueConfig[i].GetDialogueStageKey() == Math.Min(GlobalDialogueStage, ActStageID))
			if (DialogueConfig[i].GetDialogueStageKey() == GlobalDialogueStage)
			{
				if (DialogueConfig[i].GetDialogueBranchKey() == BranchID)
				{
					DialogueConfig[i].IncrementConfStage(incrementamount);
					
				}
					
			};
			
		}
		GlobalDialogueStage = GlobalDialogueStage + incrementamount;
		
		return false;
	}
	int GetDiagStage()
	{
		return GlobalDialogueStage;
	}
	string GetDialogueText(int BranchID, int ActStageID)
	{
		string m_stexttoshow;
		for (int i, count = DialogueConfig.Count(); i < count; i++)
		{
			//if (DialogueConfig[i].GetDialogueStageKey() == Math.Min(GlobalDialogueStage, ActStageID))
			if (DialogueConfig[i].GetDialogueBranchKey() == BranchID)
				{
					m_stexttoshow = DialogueConfig[i].GetDialogueText(GlobalDialogueStage);
					
				}
			
			
		}
		
		return m_stexttoshow;
	}
	string GetActionTitle(int BranchID, int ActStageID)
	{
		string m_sActionTitle;
		for (int i, count = DialogueConfig.Count(); i < count; i++)
		{
			if (DialogueConfig[i].GetDialogueBranchKey() == BranchID)
				{
					m_sActionTitle = DialogueConfig[i].GetActionText(GlobalDialogueStage);
					
				}
			//if (DialogueConfig[i].GetDialogueStageKey() == Math.Min(GlobalDialogueStage, ActStageID))
				
		}
		return m_sActionTitle;
	}
};
[BaseContainerProps(), SP_CharacterArchetype()]
class SP_DialogueConfig: ScriptAndConfig
{
	
	[Attribute( defvalue: "", desc: "DialogueStageKey, defines at wich stage of the dialogue this config will be used", category: "Dialogue",  )]			//TODO: make config, memory
	protected int m_iDialogueStageKey;
	[Attribute( defvalue: "", desc: "DialogueText", category: "Dialogue",  )]			//TODO: make config, memory
	protected ref array<ref string> m_sDialogueText;
	[Attribute( defvalue: "", desc: "DialogueText", category: "Dialogue",  )]			//TODO: make config, memory
	protected ref array<ref string> m_sActionText;
	[Attribute( defvalue: "", desc: "DialogueBranchKey, Key used to define wich action should use this config", category: "Dialogue",  )]			//TODO: make config, memory
	protected int m_iDialogueBranchKey;
	
	//configuration needs to have a defined stage at wich branching happens. 
	
	//! Returns the identifier of this context or string.Empty if none.
	int GetDialogueStageKey()
	{
		return m_iDialogueStageKey;
	}
	string GetDialogueText(int stageid)
	{
		string diagtext;
		if (stageid >= m_iDialogueStageKey)
		{
			for (int i, count = m_sDialogueText.Count(); i < count; i++)
			
			if (i == stageid)
			{
				diagtext = m_sDialogueText[i];
			}
			return diagtext;
		}
		else 
			return STRING_EMPTY;
	}
	int GetDialogueBranchKey()
	{
		return m_iDialogueBranchKey;
	}
	string GetActionText(int stageid)
	{
		string actiontext;
		for (int i, count = m_sActionText.Count(); i < count; i++)
		if (i == stageid)
		{
			actiontext = m_sActionText[i];
		}
		return actiontext;
	}
	bool IncrementConfStage(int IncrementAmount)
	{
		m_iDialogueStageKey = m_iDialogueStageKey + IncrementAmount;
		return true;
	}
	
};
[BaseContainerProps(), SP_CharacterArchetype()]
class SP_BranchingDialogueConfig: SP_DialogueConfig
{
	[Attribute( defvalue: "", desc: "ResponseDialogueText", category: "Dialogue",  )]			//TODO: make config, memory
	protected ref array<ref string> m_sResponseDialogueText;
	
	bool isbranchingtime(int stageID)
	{
		
	}
};