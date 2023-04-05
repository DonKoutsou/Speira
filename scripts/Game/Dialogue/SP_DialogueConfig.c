//Dialogue Configuration Entry, contains dialogue and action name for specified branch and stage.
//If the dialogue branch ID and stage ID match this dialogue it should show up on the specified character archetype.
[BaseContainerProps(configRoot: true)]
class SP_DialogueConfig: ScriptAndConfig
{
	//------------------------------------------------------------------//
	//Action title
	[Attribute(defvalue: "", desc: "ActionText", category: "Dialogue")]
	protected string m_sActionText;
	//Dialogue text that's going to be sent to chat
	[Attribute(defvalue: "", desc: "DialogueText", category: "Dialogue")]
	protected string m_sDialogueText;
	//Branch that this action should show up in
	[Attribute( defvalue: "", desc: "DialogueBranchKey, Key used to define wich action should use this config", category: "Dialogue",  )]			//TODO: make config, memory
	protected int m_iDialogueBranchKey;
	//Stage at wich this configuration becomes relevant
	[Attribute(defvalue: "", desc: "DialogueStageKey, defines at wich stage of the dialogue this config will be used", category: "Dialogue",  )]			//TODO: make config, memory
	protected int m_iDialogueStageKey;
	//if this configuration should progress the stage of all the branches on their character archetype
	[Attribute(defvalue: "", desc: "Should this dialogue progress all branches when executed?", category: "Dialogue",  )]
	protected bool m_bGlobalStageInfluance;
	[Attribute()]
	protected ref SP_RadialChoiceConfig m_RadialChoiceConfig;
	//------------------------------------------------------------------//
	//Stage key is used to identify when this dialogue is relevant
	int GetDialogueStageKey()
	{
		return m_iDialogueStageKey;
	}
	//------------------------------------------------------------------//
	//Branch key is used to identify wich branch this config belongs to
	int GetDialogueBranchKey()
	{
		return m_iDialogueBranchKey;
	}
	//------------------------------------------------------------------//
	//Text that is going to be used as title for the action
	string GetActionText(int stageid)
	{
		string actiontext;
		actiontext = m_sActionText;
		return actiontext;
	}
	//------------------------------------------------------------------//
	//Text that is going to be sent to chat
	string GetDialogueText(int stageid)
	{
		string diagtext;
		diagtext = m_sDialogueText;
		return diagtext;
	}
	//------------------------------------------------------------------//
	//Boolean about if this configuration should progress all branches when used
	bool IsInfluanceGlobal()
	{
		return m_bGlobalStageInfluance;
	}
	//------------------------------------------------------------------//
	//Getter for radial configuration file marking this config as branching and that it should open radial menu
	SP_RadialChoiceConfig GetRadialChoiceConfig()
	{
		if(m_RadialChoiceConfig == null)
		{
			return null;
		}
		else
			return m_RadialChoiceConfig;
		
	}
};
