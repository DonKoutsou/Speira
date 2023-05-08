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
	[Attribute("50", UIWidgets.ComboBox, "Character rank, used to identify character dialogue. Compared to rank in SCR_CharacterRankComponent", category:"CharacterInfo", ParamEnumArray.FromEnum(SCR_ECharacterRank))]
	private SCR_ECharacterRank m_sCharacterRank;
	//Factions this archetype is for
	[Attribute("FactionKey", "Faction the player needs to be to get access to this archetype", category:"CharacterInfo")]
	private ref array<string> m_aArchetypeFactionMatch;
	//------------------------------------------------------------------//
	//Different configuration containing dialogue texts
	[Attribute(desc: "Filtered selection test")]
	private ref array<ref SP_DialogueBranch> DialogueBranch;
	//------------------------------------------------------------------//
	//Map to be filled with all the configurations on Init
	protected ref map<int, ref SP_DialogueBranch> DialogueBranchMap;
    protected SP_DialogueComponent DiagComp;
	SP_DialogueArchetype OriginalArchetype;
	IEntity TalkingCharacter;
	bool IsCharacterBranched;
	int BranchedID;
	array<string> GetArchtypeFactionMatch()
	{
		return m_aArchetypeFactionMatch;
	}
	//------------------------------------------------------------------//
	//Bool returning IsCharacterBranched, used to check if character is branched. True = Branched - False = Unbranched
	bool IsCharacterBranched()
	{
		return IsCharacterBranched;
	}
	//------------------------------------------------------------------//
	//Branches Archetype, setting IsCharacterBranched to true if its not already, and sets ID of branch that dialogue should follow, if branch 0 gets branched we will want to look there for dialogue, this is where the ID is used.
	//If dialogue branch 1 id branched, when we go look for dialogue in the archetype will will only look inside branch 1.
	void BranchArchetype(int Branch)
	{
		if (IsCharacterBranched == false)
		{
			IsCharacterBranched = true;
			BranchedID = Branch;
		}
	}
	//------------------------------------------------------------------//
	//Unbranches Archetype, setting IsCharacterBranched to false if its not already, resets branch ID
	void UnBranchArchetype()
	{
		if (IsCharacterBranched == true)
		{
			IsCharacterBranched = false;
			BranchedID = null;
		}
	}
	//------------------------------------------------------------------//
	//Returns BranchID
	int GetBranchedID()
	{
		return BranchedID;
	}
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
	SCR_ECharacterRank GetArchetypeTemplateRank()
	{
		return m_sCharacterRank;
	}
	//------------------------------------------------------------------//
	//Mapping all configrations existing uder this character archetype
	//When using ArchetypeTemplate to create new archetype for specific AI, it is Initialised
	void Init(IEntity Character)
	{
		OriginalArchetype = this;
		DialogueBranchMap = new map<int, ref SP_DialogueBranch>();
		for (int i = 0, count = DialogueBranch.Count(); i < count; i++)
        {
			//using 2 values to create key, branch and stage IDs
			int key = (i);
        	DialogueBranchMap.Insert(key, DialogueBranch[i]);
        }
		TalkingCharacter = Character;
	}
	//------------------------------------------------------------------//
	//Find Branch using Branch ID
	//If character is branched we look for a branch using our BranchedID that we got once this Archetype got branched and we keep looking deeper using GetCurrentDialogueBranch
	SP_DialogueBranch GetDialogueBranch(int BranchKey)
    {
        SP_DialogueBranch branch;
		if (IsCharacterBranched == true)
		{
			if (!DialogueBranchMap.Find(BranchedID, branch))
			{
	        	return null;
	    	}
			return branch.GetCurrentDialogueBranch(TalkingCharacter, BranchKey);
		}
		if (!DialogueBranchMap.Find(BranchKey, branch))
			{
	        	return null;
	    	}
		if(branch)
		{
			return branch;
		}
        return null;
    }
	//------------------------------------------------------------------//
	//Find correct branch and get dialogue text from it
	string GetDialogueText(int BranchID)
	{
		string m_stexttoshow;
		SP_DialogueBranch branch = GetDialogueBranch(BranchID);
		if (branch)
		{
		m_stexttoshow = branch.GetDialogueText(TalkingCharacter);
		}
		else
		{
			m_stexttoshow = STRING_EMPTY;
		}
		return m_stexttoshow;
	}
	//------------------------------------------------------------------//
	//Find correct branch using current stage and take action title from it 
	string GetActionTitle(int BranchID, IEntity Character, IEntity Player)
	{
		string m_sActionTitle;
		SP_DialogueBranch Branch = GetDialogueBranch(BranchID);
		if (Branch)
		{
			m_sActionTitle = Branch.GetActionText(Character, Player);
		}
		else
		{
			m_sActionTitle = STRING_EMPTY;
		}
		return m_sActionTitle;
	}
	//------------------------------------------------------------------//
	//Find branch using branchID and increment its stage
	bool IncrementStage(int BranchID, int incrementamount)
	{	
		SP_DialogueBranch branch = GetDialogueBranch(BranchID);
		branch.IncrementBranchStage(incrementamount);
		return true;
	}
	//------------------------------------------------------------------//
		array<ref SP_DialogueBranch> GetDialogueBranchArray()
	{
		return DialogueBranch;
	}
	//------------------------------------------------------------------//
	map<int, ref SP_DialogueBranch> GetDialogueBranchMap()
	{
		return DialogueBranchMap;
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
        DialogueBranch = original.GetDialogueBranchArray();
		DialogueBranchMap = original.GetDialogueBranchMap();
		}
    }
};