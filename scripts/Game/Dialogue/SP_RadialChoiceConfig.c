//SP_RadialChoiceConfig is used to store the text that is going to be used in radial menu in dialogue.
//Also  marks the config it is atatched to as branching dialogue so when that action is performed radial menu will open intead of progressing dialogue

[BaseContainerProps(configRoot: true)]
class SP_RadialChoiceConfig: ScriptAndConfig
{
	//Behavior that this choice is going to use once performed. //eg. "Accept&close" takes the selection made by player and then closes radial menu, while "Stay" keep the menu open so player can keep using it
	[Attribute("", UIWidgets.ComboBox, "What is going to be the behavior of radial menu choise", "", ParamEnumArray.FromEnum(EChoiseBehavior) )]
	protected EChoiseBehavior ChoiseBehavior;
	//Class containing both action and dialogue text for each radial menu entry
	[Attribute(defvalue: "", desc: "Action Title and Dialogue Text for Radial menu choice", category: "Dialogue")]
	protected ref array<ref DialogueTextConfig> m_RadialChoiceTexts;
	//Get action title besed on ID provided, id grow as choice text is added, if 1 choice text exists is should have id 0, if 2 then first 0 second 1
	string GetActionText(int TextID)
    {
        if (TextID >= 0 && TextID < m_RadialChoiceTexts.Count()) 
        {
            return m_RadialChoiceTexts[TextID].ActionText;
        }
        else 
        {
            return STRING_EMPTY;
        }
    }
	//Get dialogue besed on ID provided
	string GetDialogueText(int TextID)
    {
        if (TextID >= 0 && TextID < m_RadialChoiceTexts.Count()) 
        {
            return m_RadialChoiceTexts[TextID].DialogueText;
        }
        else 
        {
            return STRING_EMPTY;
        }
    }
	//Getter for choice behavior Enum
	EChoiseBehavior GetChoiseBehavior()
	{
		return ChoiseBehavior;
	}
	
};

class DialogueTextConfig: ScriptAndConfig
{
	[Attribute(defvalue: "", desc: "Action Title", category: "Dialogue")]
	string ActionText;
	[Attribute(defvalue: "", desc: "Dialogue Text", category: "Dialogue")]
    string DialogueText;
}