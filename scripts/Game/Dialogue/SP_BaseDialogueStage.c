[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStage
{
	//------------------------------------------------------------------//
	[Attribute(defvalue: "Action Text", desc: "Action Title", category: "Dialogue")]
	string ActionText;
	//------------------------------------------------------------------//
	[Attribute(defvalue: "Dialogue Text", desc: "Dialogue Text", category: "Dialogue")]
    string DialogueText;
	//------------------------------------------------------------------//
	[Attribute(desc: "Dialogue Branch, if present will cause branch to split instead of progressing its stage. When a branch splits, the dialogue system will only look in the entries of this branch only")]
	ref array<ref SP_DialogueBranch> m_Branch;
	//------------------------------------------------------------------//
	string m_sCantBePerformedReason = "(Cant Be Performed)";
	//------------------------------------------------------------------//
	//Get action text from DialogueStage
	void Perform(IEntity Character, IEntity Player)
	{
	};
	//------------------------------------------------------------------//
	bool CanBePerformed(IEntity Character, IEntity Player)
	{
		return true;
	}
	//------------------------------------------------------------------//
	bool CanBeShown(IEntity Character, IEntity Player)
	{
		return true;
	}
	void SetCannotPerformReason(string reason)
	{
		m_sCantBePerformedReason = reason;
	}
	string GetCannotPerformReason()
	{
		return m_sCantBePerformedReason;
	}
	//------------------------------------------------------------------//
	string GetActionText(IEntity Character, IEntity Player)
	{
		if (CanBeShown(Character, Player) == false)
		{
		 return STRING_EMPTY;
		}
		if (CanBePerformed(Character, Player) == false)
		{
			string acttext = ActionText + " " + m_sCantBePerformedReason;
			return acttext;
		}
	 	return ActionText;
	}
	//------------------------------------------------------------------//
	void InheritData(SP_DialogueArchetype Archetype, DialogueBranchInfo Config, IEntity Char)
	{
		if(m_Branch && m_Branch.Count() > 0)
			{
				foreach (int i, SP_DialogueBranch DiagBranch: m_Branch)
				{
					DiagBranch.InheritData(Archetype, Config,Char )
				}
			}
	}
	//------------------------------------------------------------------//
	bool CheckIfStageCanBranch()
	{
		if (m_Branch.Count() > 0)
		{
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------//
	SP_DialogueBranch GetBranch(int BranchID)
	{
		if(m_Branch.Count() >= BranchID + 1)
		{
			return m_Branch[BranchID];
		}
		return null;
	}
	//------------------------------------------------------------------//
	//Get action text from SP_MultipleChoiceConfig, text key chooses the entry. TextKey = 0 will take the first entry in SP_MultipleChoiceConfig
	string GetStageBranchActionText(int TextKey, IEntity Character, IEntity Player)
	{
		string ActText
		if(m_Branch[TextKey])
		{
			ActText = m_Branch[TextKey].GetActionText(Character, Player);	
		}
		else
		{
			ActText = STRING_EMPTY;
		}
		return ActText;
	}
	//------------------------------------------------------------------//
	//Get dialogue text from DialogueStage
	string GetDialogueText(IEntity Character, IEntity Player)
	{
	 return DialogueText;
	}
	//------------------------------------------------------------------//
	//Get dialogue text from SP_MultipleChoiceConfig, text key chooses the entry. TextKey = 0 will take the first entry in SP_MultipleChoiceConfig
	string GetStageBranchDialogueText(int TextKey, IEntity Character, IEntity Player)
	{
		string DiagText;
		if(m_Branch[TextKey])
		{
			DiagText = m_Branch[TextKey].GetDialogueText(Character, Player);
		}
		else
		{
			DiagText = STRING_EMPTY;
		}
	 	return DiagText;
	}
}
//---------------------------------------------------------------------------------------------------//
class DialogueStageTitleAttribute : BaseContainerCustomTitle
{
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		// Get ActionName
		string actionName;
		source.Get("ActionText", actionName);

		// Get selected behavior from EChoiseBehavior enum
		string branchesStr;
		array<ref SP_DialogueBranch> branches;
		source.Get("m_Branch", branches);
		if (branches.Count() > 0)
		{
			branchesStr = "Stage Branches"
		}
		else
		{
			branchesStr = "Stage Increments"
		}
		title = string.Format("Stage: %1 - %2", actionName, branchesStr);
		return true;
	}
}