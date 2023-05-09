//Dialogue Branch is used to store a menu entry. In any dialogue branch a multiple stages can be concifgured.
//In Dialogue branch the dialogues progession can be configured, at any stage a Multiple choice config can be introduces to create another branch.
//Each branch tracks its own stage
[BaseContainerProps(configRoot:true),  DialogueBranchConfigTitleAttribute()]
class SP_DialogueBranch
{
	//------------------------------------------------------------------//
	[Attribute(desc: "Dialogue Stage, Depending on the stage of the branch the apropriate stage will be selected. Stage = 0 means first entry etc...")]
	ref array<ref DialogueStage> 						m_BranchStages;
	ref DialogueBranchInfo 								BranchInfoConfig;
	protected ref map<string, ref DialogueBranchInfo> 	BranchInfoConfigMap;
	IEntity 											TalkingCharacter;
	//------------------------------------------------------------------//
	//Text that is going to be used as title for the action
	void OnPerform(IEntity Character, IEntity Player)
	{
		DialogueBranchInfo Conf = LocateConfig(Character);
		if(m_BranchStages.Count() >= Conf.GetDialogueBranchStage())
		{
			m_BranchStages[Conf.GetDialogueBranchStage()].Perform(Character, Player);
		}
	};
	//------------------------------------------------------------------//
	bool CanBePerformed(IEntity Character, IEntity Player)
	{
		DialogueBranchInfo Conf = LocateConfig(Character);
		return m_BranchStages[Conf.GetDialogueBranchStage()].CanBePerformed(Character, Player);
	};
	//------------------------------------------------------------------//
	bool CanBeShown(IEntity Character, IEntity Player)
	{
		DialogueBranchInfo Conf = LocateConfig(Character);
		return m_BranchStages[Conf.GetDialogueBranchStage()].CanBeShown(Character, Player);
	}
	//------------------------------------------------------------------//
	string GetActionText(IEntity Character, IEntity Player)
	{
		string ActText;
		TalkingCharacter = Character;
		DialogueBranchInfo Conf = LocateConfig(TalkingCharacter);
		if(m_BranchStages.Count() >= Conf.GetDialogueBranchStage())
		{
			ActText = m_BranchStages[Conf.GetDialogueBranchStage()].GetActionText(Character, Player);
		}
		return ActText;
	}
	//------------------------------------------------------------------//
	//Text that is going to be sent in the chat
	string GetDialogueText(IEntity Character, IEntity Player)
	{
		string DiagText;
		TalkingCharacter = Character;
		DialogueBranchInfo Conf = LocateConfig(TalkingCharacter);
		if(m_BranchStages.Count() >= Conf.GetDialogueBranchStage())
		{
		DiagText = m_BranchStages[Conf.GetDialogueBranchStage()].GetDialogueText(Character, Player);
		}
		return DiagText;
	}
	//------------------------------------------------------------------//
	//Increments stage of this branch
	void IncrementBranchStage(int incrementamount)
	{
		LocateConfig(TalkingCharacter).IncrementStage(incrementamount);
	}
	//------------------------------------------------------------------//
	//Checks if a SP_MultipleChoiceConfig exists in the current DialogueStage
	bool CheckIfStageBranches()
	{
		int currentstage = LocateConfig(TalkingCharacter).GetDialogueBranchStage();
		DialogueStage Diagstage;
		if (m_BranchStages.Count() >= currentstage)
		{
			Diagstage = m_BranchStages[currentstage];
		}
		
		if(Diagstage && Diagstage.CheckIfStageCanBranch() == true)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	//------------------------------------------------------------------//
	void CauseBranch(int BranchID)
	{
		LocateConfig(TalkingCharacter).CauseBranch(BranchID);
	}
	//------------------------------------------------------------------//
	bool CheckIfBranched(IEntity Character)
	{
		return LocateConfig(Character).CheckifBranched();
	}
	//------------------------------------------------------------------//
	DialogueBranchInfo GetParent()
	{
		DialogueBranchInfo Parent = LocateConfig(TalkingCharacter).GetParentConfig();
		if (Parent)
		{
			return Parent;
		}
		else
		{
			return null;
		}
	}
	//------------------------------------------------------------------//
	DialogueStage GetDialogueStage()
	{
		DialogueBranchInfo Conf = LocateConfig(TalkingCharacter);
		if(m_BranchStages.Count() >= Conf.GetDialogueBranchStage())
		{
			return m_BranchStages[Conf.GetDialogueBranchStage()];
		}
		return null;
	}
	bool CheckNextStage(int stageID)
	{
		if (m_BranchStages.Count() >= stageID + 1)
		{
			return true;
		}
		return false;
	}
	//------------------------------------------------------------------//
	//Return the current Branch. Meaning, it keeps looking deeper for a branch that doesent branch further (IsBranchBranched = false)
	SP_DialogueBranch GetCurrentDialogueBranch(IEntity Character, int BranchID)
	{
		TalkingCharacter = Character;
		SP_DialogueBranch branch;
		//Get the config for this dialogue branch
		DialogueBranchInfo Conf = LocateConfig(TalkingCharacter);
		int DiagStage = Conf.GetDialogueBranchStage();
		branch = this;
		if (Conf.CheckifBranched() == true)
		{

			int BranchedID = Conf.GetBranchedID();
			bool BranchState = m_BranchStages[DiagStage].GetBranch(BranchedID).CheckIfBranched(Character);
			if (BranchState == true)
			{
				branch = m_BranchStages[DiagStage].GetBranch(BranchedID);
				if (branch)
				{
					branch = branch.GetCurrentDialogueBranch(Character, BranchID);
				}
				
			}
			else
			{
				branch = m_BranchStages[DiagStage].GetBranch(BranchID);
				if(branch)
				{
					branch = branch.GetCurrentDialogueBranch(Character, BranchID);
				}
			}
		}
		return branch;
	}
	//------------------------------------------------------------------//
	DialogueBranchInfo LocateConfig(IEntity Character)
	{
		if(!BranchInfoConfigMap)
		{
			BranchInfoConfigMap = new map<string, ref DialogueBranchInfo>();
		}
		DialogueBranchInfo config;
		SCR_BaseGameMode GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		SP_DialogueComponent DiagComp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		string key = DiagComp.GetCharacterName(Character);
		if (BranchInfoConfigMap.Find(key, config))
		{
			return config;
		}
		else
		{
			DialogueBranchInfo configNew = CopyConfig(BranchInfoConfig);
			BranchInfoConfigMap.Insert(key, configNew);
			return configNew;
		}
	}
	//------------------------------------------------------------------//
	DialogueBranchInfo CopyConfig(DialogueBranchInfo OriginalConfig)
	{
		DialogueBranchInfo DiagConfigCopy = new DialogueBranchInfo(OriginalConfig, true);
		return DiagConfigCopy;
	}
	//------------------------------------------------------------------//
	void InheritData(SP_DialogueArchetype Archetype, DialogueBranchInfo Config, IEntity Char)
	{
		LocateConfig(Char).ParentConfig = Config;
		LocateConfig(Char).OriginalArchetype = Archetype;
	}
};
//Used as text container 
[BaseContainerProps(configRoot:true), SCR_BaseContainerCustomTitleField("ActionText", "DialogueText")]
class DialogueBranchInfo
{
	//------------------------------------------------------------------//
	//Archetype wich this cofig belongs to
	SP_DialogueArchetype OriginalArchetype;
	//------------------------------------------------------------------//
	//SP_MultipleChoiceConfig that come before this one in a hierarchy. Used for DoBackDialogue in SP_DialogueComponent
	DialogueBranchInfo ParentConfig;
	//------------------------------------------------------------------//
	//Branching info for this config. Once config gets branched the branch that caused it has its ID saved in BranchBranchID
	bool IsBranchBranched;
	int BranchBranchID
	int DialogueBranchStage;
	//------------------------------------------------------------------//
	void DialogueBranchInfo(DialogueBranchInfo original, bool isNew = false)
    {
    };
	void IncrementStage(int amount)
	{
		DialogueBranchStage = DialogueBranchStage + 1;
	}
	int GetDialogueBranchStage()
	{
		return DialogueBranchStage;
	}
	bool CheckifBranched()
	{
		return  IsBranchBranched;
	}
	void CauseBranch(int BranchID)
	{
		if(IsBranchBranched == false)
		{
			IsBranchBranched = true;
			BranchBranchID = BranchID;
		}
	}
	int GetBranchedID()
	{
		return BranchBranchID;
	}
	DialogueBranchInfo GetParentConfig()
	{
		return ParentConfig;
	}
	void Unbranch()
	{
		if (IsBranchBranched == true)
		{
			IsBranchBranched = false;
		}
	}
}
class DialogueBranchConfigTitleAttribute : BaseContainerCustomTitle
{
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		array <ref DialogueStage> Stages;
		source.Get("m_BranchStages", Stages);
		string texttoshow;
		for (int i, count = Stages.Count(); i < count; i++)
		{
			if (Stages[i].CheckIfStageCanBranch() == true)
			texttoshow = "Branches at stage" + " " + i;
			
		}
		title = string.Format("Branch" + " " + texttoshow);
		return true;
	}
};