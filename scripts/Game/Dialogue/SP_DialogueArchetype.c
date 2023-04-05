//Dialogue archetype should hold all the possible dialogue a character could hold.
//A character archetype can be used by alot of different characters (eg. Seting dialogue for all low ranking officers)
//but 1 character cant/shouldn't use multiple character archetypes. (eg. Character has 1 name 1 faction 1 rank. once any of the identifiers change his dialogue will change too)
[BaseContainerProps(configRoot: true)]
class SP_DialogueArchetype: ScriptAndConfig
{
	//------------------------------------------------------------------//
	//Dialogue Identifier
	[Attribute("50", UIWidgets.ComboBox, "ID of what type of identifier is going to be used", "", ParamEnumArray.FromEnum(EArchetypeIdentifier) )]
	private EArchetypeIdentifier ArchetypeIdentifier;
	//Character Name
	[Attribute("", UIWidgets.Object, "Character full name, used to identify character dialogue. Compared to name in identitycomponent", category:"CharacterInfo")]
	private string m_sCharacterName;
	//Character Faction
	[Attribute("", UIWidgets.Object, "Character faction, used to identify character dialogue. Compared to factionkey in FactionAffiliationComponent", category:"CharacterInfo")]
	private FactionKey m_sCharacterFaction;
	//Character Rank
	[Attribute("50", UIWidgets.ComboBox, "Character rank, used to identify character dialogue. Compared to rank in SCR_CharacterRankComponent", category:"CharacterInfo", ParamEnumArray.FromEnum(ECharacterRank))]
	private ECharacterRank m_sCharacterRank;
	//------------------------------------------------------------------//
	//Different configuration containing dialogue texts
	[Attribute()]
	private ref array<ref SP_DialogueConfig> DialogueConfig;
	//Configuration of existing branches and their initial state
	[Attribute()]
	private ref array<int> CharacterDiagBranchStage;
	//------------------------------------------------------------------//
	//Map to be filled with all the configurations on Init
	protected ref map<int, ref SP_DialogueConfig> DialogueConfigMap;
    protected SP_DialogueComponent DiagComp;
	//------------------------------------------------------------------//
	//Dialogue identifier to be used for this archetype, can be set to something generic and provide its dialogues to a variety of entities
	EArchetypeIdentifier GetIdentifier()
	{
		return ArchetypeIdentifier;
	}
	//------------------------------------------------------------------//
	//Character Name set for this Archetype, relevant only if ArchetypeIdentifier is looking for it
	string GetArchetypeTemplateName()
	{
		return m_sCharacterName;
	}
	//------------------------------------------------------------------//
	//Character Faction set for this Archetype, relevant only if ArchetypeIdentifier is looking for it
	FactionKey GetArchetypeTemplateFaction()
	{
		return m_sCharacterFaction;
	}
	//------------------------------------------------------------------//
	//Character Rank set for this Archetype, relevant only if ArchetypeIdentifier is looking for it
	ECharacterRank GetArchetypeTemplateRank()
	{
		return m_sCharacterRank;
	}
	array<ref SP_DialogueConfig> GetDialogueConfigArray()
	{
		return DialogueConfig;
	}
	array<int> GetCharacterDiagBranchStageArray()
	{
		return CharacterDiagBranchStage;
	}
	//------------------------------------------------------------------//
	//Mapping all configrations existing uder this character archetype
	void Init()
	{
		DialogueConfigMap = new map<int, ref SP_DialogueConfig>();
		for (int i = 0, count = DialogueConfig.Count(); i < count; i++)
        {
			//using 2 values to create key, branch and stage IDs
			int key = (DialogueConfig[i].GetDialogueStageKey() << 16) | (DialogueConfig[i].GetDialogueBranchKey());
        	DialogueConfigMap.Insert(key, DialogueConfig[i]);
        }
	}
	//------------------------------------------------------------------//
	//Find the Config you are looking for using the map made above
	SP_DialogueConfig GetDialogueConfig(int StageKey, int BranchKey)
    {
		int key = (StageKey << 16) | (BranchKey);
        SP_DialogueConfig config;
		if (!DialogueConfigMap.Find(key, config))
		{
        	return null;
    	}
		
        return config;
    }
	//------------------------------------------------------------------//
	//Find the Config you are looking for using the map made above using the current stage
	SP_DialogueConfig GetDialogueConfigLite(int BranchKey)
    {
		int StageKey = CharacterDiagBranchStage[BranchKey];
		int key = (StageKey << 16) | (BranchKey);
        SP_DialogueConfig config;
		if (!DialogueConfigMap.Find(key, config))
		{
        	return null;
    	}
		
        return config;
    }
	//------------------------------------------------------------------//
	//Checks if a SP_RadialChoiceConfig is hooked on this config, used to initiate radial menu insead of completing dialogue
	bool CheckIfDialogueBranches(SP_DialogueConfig DialogueConfiguration)
	{
		
		if (DialogueConfiguration.GetRadialChoiceConfig() == null)
		{
			return false;
		}
		else
			return true;
	}
	//------------------------------------------------------------------//
	//Check if IsInfluanceGlobal is true wich means that all branches should progress, if not progres only the branch provided
	bool IncrementStage(int BranchID, int incrementamount)
	{	
		SP_DialogueConfig config = GetDialogueConfig(CharacterDiagBranchStage[BranchID], BranchID);
		if (config != null && config.IsInfluanceGlobal() == true)
		{
			for (int i = 0, count = CharacterDiagBranchStage.Count(); i < count; i++)
        	{	
				CharacterDiagBranchStage[i] = CharacterDiagBranchStage[i] + 1;	
        	}
		}
		else
			CharacterDiagBranchStage[BranchID] = CharacterDiagBranchStage[BranchID] + 1;
		return true;
	}
	//------------------------------------------------------------------//
	//resets stage of branches
	bool ResetStage(int BranchID)
	{	
		for (int i = 0, count = CharacterDiagBranchStage.Count(); i < count; i++)
        	{	
				CharacterDiagBranchStage[i] = 1;	
        	}
		return true;
	}
	//------------------------------------------------------------------//
	//Find correct config using currect stage and take dialogue text from it 
	string GetDialogueText(int BranchID)
	{
		string m_stexttoshow;
		for (int i = 0, count = CharacterDiagBranchStage.Count(); i < count; i++)
		{
			SP_DialogueConfig config = GetDialogueConfig(CharacterDiagBranchStage[BranchID], BranchID);
			if (config != null)
				{
					m_stexttoshow = config.GetDialogueText(CharacterDiagBranchStage[BranchID]);
				}
		}	
		return m_stexttoshow;
	}
	//------------------------------------------------------------------//
	//Find correct config using current stage and take action title from it 
	string GetActionTitle(int BranchID)
	{
		string m_sActionTitle;
		for (int i = 0, count = CharacterDiagBranchStage.Count(); i < count; i++)
		{
			SP_DialogueConfig config = GetDialogueConfig(CharacterDiagBranchStage[BranchID], BranchID);
			if (config != null)
				{
					m_sActionTitle = config.GetActionText(CharacterDiagBranchStage[BranchID]);
				}
		}
		return m_sActionTitle;
	}
	//------------------------------------------------------------------//
	// constructor for when creating new Archetype
	void SP_DialogueArchetype(SP_DialogueArchetype original, bool isNew = false)
    {
		if (isNew) 
		{
        ArchetypeIdentifier = original.GetIdentifier();
        m_sCharacterName = original.GetArchetypeTemplateName();
        m_sCharacterRank = original.GetArchetypeTemplateRank();
        m_sCharacterFaction = original.GetArchetypeTemplateFaction();
        DialogueConfig = original.GetDialogueConfigArray();
		CharacterDiagBranchStage = new array<int>;
        CharacterDiagBranchStage.Copy(original.GetCharacterDiagBranchStageArray());
		}
    }
};