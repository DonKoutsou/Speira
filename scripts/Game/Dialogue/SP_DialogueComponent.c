class SP_DialogueComponentClass: ScriptComponentClass
{
};
class SP_DialogueComponent: ScriptComponent
{
	//----------------------------------------------------------------------------------------------------------------//
	[Attribute()]
	protected ref array<ref SP_DialogueArchetype> m_CharacterArchetypeList;
	
	[Attribute()]
	ref BaseChatChannel m_ChatChannel;
	//----------------------------------------------------------------------------------------------------------------//
	// Character Info
	protected SCR_CharacterIdentityComponent CharIDComp;
	protected SCR_CharacterRankComponent CharRankComp;
	protected FactionAffiliationComponent FactComp;
	protected ECharacterRank senderRank;
	protected FactionKey senderFaction;
	protected string senderName;
	protected int senderId;
	IEntity PlayerCharacter;
	IEntity TalkingCharacter;
	//----------------------------------------------------------------------------------------------------------------//
	//Dialogue System
	protected EArchetypeIdentifier Archid;
	protected ref map<string, ref SP_DialogueArchetype> DialogueArchetypeMap;
    protected SP_DialogueArchetype DiagArch;
	protected string m_DialogTexttoshow;
	//Stage state???
	protected int GlobalDiagStage;
	SCR_RadialMenuComponent RadComp;
	SP_RadialMenuDiags RadMenuDiags;
	SCR_BaseGameMode GameMode;
	SP_DialogueConfig DialogueConfig;
	SP_RadialChoiceConfig RadialConfig;
	
	
	//----------------------------------------------------------------------------------------------------------------//
	//DoDialogue used in the dialogue action, initiates the whole process and doese all the required operations(incrementing stage, sending text)
	void DoDialogue(IEntity Character, IEntity Player, int BranchID, int IncrementAmount)
	{
		//Get name of character that will send message to chat
		senderName = GetCharacterName(Character);
		int senderID = Character.GetID();
		//Find our Dialogue Archetype by matching it to characters info
		DiagArch = LocateCharacterArchetype(Character);
		//Get dialogue text from Archetype
		m_DialogTexttoshow = DiagArch.GetDialogueText(BranchID);
		
		//Check if dialogue config has SP_RadialChoiceConfig atatched to it, meaning that is should launch the radial menu isntead of completing ActionsTuple
		//Makes sure to return before incrementing stage to avoid changing wich config is used while in radial menu
		if(DiagArch.GetDialogueConfigLite(BranchID) && DiagArch.CheckIfDialogueBranches(DiagArch.GetDialogueConfigLite(BranchID)) == true)
		{
			if (RadMenuDiags.IsOpen() == false)
			{
				RadMenuDiags.Start(Character, Player);
			}
			return;
		}
		else
			if (RadMenuDiags.IsOpen() == true)
			{
				RadMenuDiags.Close(Character)
			}
			SendText(m_DialogTexttoshow, m_ChatChannel, senderId, senderName);
			IncrementDiagStage(Character, BranchID, IncrementAmount);
			
	}
	//----------------------------------------------------------------------------------------------------------------//
	//DoDialogue duplicate used for radial meny entries
	void DoRadialDialogue(IEntity Character, IEntity Player, int BranchID, int IncrementAmount, int EntryID)
	{
		//Get name of character that will send message to chat
		senderName = GetCharacterName(Character);
		
		//Find our Diialogue Archetype by matching it to characters info
		DiagArch = LocateCharacterArchetype(Character);
		
		//Get Dialogue config with same branch id. Gives you config that has same stage id as current stage
		DialogueConfig = DiagArch.GetDialogueConfigLite(BranchID);
		
		if (DialogueConfig)
		{
			//find the configuration that contains the text we want
			RadialConfig = DialogueConfig.GetRadialChoiceConfig();
			if(RadialConfig)
			{	
				m_DialogTexttoshow = RadialConfig.GetDialogueText(EntryID);
				EChoiseBehavior ChoiseBehavior = RadialConfig.GetChoiseBehavior();
				switch (ChoiseBehavior) 
					{
				    case 0:
							//Accept & Close
							RadMenuDiags.Close(Player);
						break;
					case 1:
							//Alternate Dialogue WIP
						break;
					case 2:
							//Stay
						break;
					}
			}
		}
		
		SendText(m_DialogTexttoshow, m_ChatChannel, senderId, senderName);
	}
	//----------------------------------------------------------------------------------------------------------------//
	// Used in action to send the text to chat
	void SendText(string Text, BaseChatChannel Chanell, int SenderID, string SenderName)
	{
		SCR_ChatPanelManager.GetInstance().ShowDiagMessage(m_DialogTexttoshow, m_ChatChannel, senderId, senderName);
	}
	//----------------------------------------------------------------------------------------------------------------//
	// returns string from dialogue configuration under field Action Text, ment to be used for text shown on the action
	string GetActionName(int BranchID, IEntity Owner)
	{
		string m_sActionName;
		DiagArch = LocateCharacterArchetype(Owner);
		m_sActionName = DiagArch.GetActionTitle(BranchID);
		return m_sActionName;
	}
	//Get title for radial meny action
	string GetRadialActionName(int BranchID, IEntity Owner, int RadialID)
	{
		string m_sMActionName;
		SP_DialogueConfig DialogueConfiguration;
		SP_RadialChoiceConfig RadialConfiguration;
		//Find correct archetype
		DiagArch = LocateCharacterArchetype(Owner);
		//Find the correct configuration in the archetype for the current stage
		DialogueConfiguration = DiagArch.GetDialogueConfigLite(BranchID);
		if (DialogueConfiguration)
		{
			//look for SP_RadialChoiceConfig
			RadialConfiguration = DialogueConfiguration.GetRadialChoiceConfig();
			if(!RadialConfiguration)
			{
				return STRING_EMPTY;
			}
			//if foind get text from it
			m_sMActionName = RadialConfiguration.GetActionText(RadialID);
			if(!m_sMActionName)
			{
				return STRING_EMPTY;
			}
		}
		
		return m_sMActionName;
	}
	//----------------------------------------------------------------------------------------------------------------//
	//progress dialogue stage int, need to figure out where/how to store the stage
	bool IncrementDiagStage(IEntity owner, int BranchID, int incrementamount)
	{
		//find correct archetype
		DiagArch = LocateCharacterArchetype(owner);
		//Increment the stage of the correct branch
		DiagArch.IncrementStage(BranchID, incrementamount);
		return false;
	}
	//----------------------------------------------------------------------------------------------------------------//
	//initialise configuration to crate map of configuration's contents
	override void EOnInit(IEntity owner)
	{
		GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		RadComp = SCR_RadialMenuComponent.Cast(GameMode.FindComponent(SCR_RadialMenuComponent));
		RadMenuDiags = SP_RadialMenuDiags.Cast(RadComp.GetRadialMenuHandler());
		foreach (SP_DialogueArchetype config: m_CharacterArchetypeList)
		{
			config.Init();
		}
		DialogueArchetypeMap = new map<string, ref SP_DialogueArchetype>;
	}
	//----------------------------------------------------------------------------------------------------------------//
	// set masks;
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
		SetEventMask(owner, EntityEvent.POSTFIXEDFRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	//----------------------------------------------------------------------------------------------------------------//
	//Getters for character info for identifiers, character name also used on sent text
	//Character full name
	string GetCharacterName(IEntity Character)
	{
		SCR_CharacterIdentityComponent IdentityComponent = SCR_CharacterIdentityComponent.Cast(Character.FindComponent(SCR_CharacterIdentityComponent));
		string CharacterFullName;
		if(IdentityComponent)
		{
			return IdentityComponent.GetCharacterFullName();
		}
		else
			return STRING_EMPTY;
	}
	//Character rank
	ECharacterRank GetCharacterRank(IEntity Character)
	{
		SCR_CharacterRankComponent RankComponent = SCR_CharacterRankComponent.Cast(Character.FindComponent(SCR_CharacterRankComponent));
		if(RankComponent)
		{
			return RankComponent.GetCharacterRank(Character);
		}
		return null;
	}
	//Character Faction
	FactionKey GetCharacterFaction(IEntity Character)
	{
		FactionAffiliationComponent FactionComponent = FactionAffiliationComponent.Cast(Character.FindComponent(FactionAffiliationComponent));
		if(FactionComponent)
		{
			return FactionComponent.GetAffiliatedFaction().GetFactionKey();
		}
		else
			return STRING_EMPTY;	
	}
	//----------------------------------------------------------------------------------------------------------------//
	//Using Dialogue ID (EDiagIdentifier) figure out wich archetype matches provided character.
	SP_DialogueArchetype GetArchetypeTemplate(IEntity pOwnerEntity)
	{
		for (int i, count = m_CharacterArchetypeList.Count(); i < count; i++)
		{
			Archid = m_CharacterArchetypeList[i].GetIdentifier();
			switch (Archid) 
			{
			//-----------------------------------------------------------------------------------------------------------//
			// diagid 0 means we look for Character Name
		    case 0:
				senderName = GetCharacterName(pOwnerEntity);
				if (m_CharacterArchetypeList[i].GetArchetypeTemplateName() == senderName)
				{
					DiagArch = m_CharacterArchetypeList[i];
					return DiagArch;
				}
				break;
			//-----------------------------------------------------------------------------------------------------------//
			// diagid 1 means we look for Character Rank
		    case 1:
				senderRank = GetCharacterRank(pOwnerEntity);
				if (m_CharacterArchetypeList[i].GetArchetypeTemplateRank() == senderRank)
				{
					DiagArch = m_CharacterArchetypeList[i];
					return DiagArch;
				}
				break;
			//-----------------------------------------------------------------------------------------------------------//
			// diagid 2 means we look for Character Faction
			case 2:
				senderFaction = GetCharacterFaction(pOwnerEntity);
				if (m_CharacterArchetypeList[i].GetArchetypeTemplateFaction() == senderFaction)
				{
					DiagArch = m_CharacterArchetypeList[i];
					return DiagArch;
				}
				break;
			//-----------------------------------------------------------------------------------------------------------//
			// diagid 3 means we look for Character Faction and Rank
			case 3:
				senderFaction = GetCharacterFaction(pOwnerEntity);
				senderRank = GetCharacterRank(pOwnerEntity);
				if (m_CharacterArchetypeList[i].GetArchetypeTemplateFaction() == senderFaction && m_CharacterArchetypeList[i].GetArchetypeTemplateRank() == senderRank)
				{
					DiagArch = m_CharacterArchetypeList[i];
					return DiagArch;
				}
				break;
			//-----------------------------------------------------------------------------------------------------------//
			}
		}
	return DiagArch;
	}
	//-----------------------------------------------------------------------------------------------------------//
	// locate if there is already an Archetype instace for this specific charater and if not initiates the creation of one
	SP_DialogueArchetype LocateCharacterArchetype(IEntity Character)
	{
		SP_DialogueArchetype CharDialogueArch;
		//using character full name atm to match Character with Archetype
		string LocCharacterName = GetCharacterName(Character);
		//Check if an Archetype with out character's name exists
		if (DialogueArchetypeMap.Contains(LocCharacterName))
			{
				//if yes assign it to CharDialogueArch so we can return it
			    CharDialogueArch = DialogueArchetypeMap[LocCharacterName];
			}
			else
			{
				//if not find an ArchetypeTemplate, make a copy of it and instet it in DialogueArchetypeMap
				//find character template using our character entity
				CharDialogueArch = GetArchetypeTemplate(Character);
				//create a new archetype and copy the stuff in it
				SP_DialogueArchetype DiagArchNew = CopyArchetype(CharDialogueArch);
				//initialise the newly made Archetype after its filled with all data
				DiagArchNew.Init();
				//instert it int the ArchetypeMap
				DialogueArchetypeMap.Insert(LocCharacterName, DiagArchNew);
			}
	return CharDialogueArch;
	}
	//-----------------------------------------------------------------------------------------------------------//
	//takes all info requred from Archetype and returns a newly made Archetype with the copied info
	SP_DialogueArchetype CopyArchetype(SP_DialogueArchetype OriginalArchetype)
	{
		SP_DialogueArchetype DiagArchCopy = new SP_DialogueArchetype(OriginalArchetype, true);
		return DiagArchCopy;
	}
}
//----------------------------------------------------------------------------------------------------------------//
enum EArchetypeIdentifier
	{
		Name,
		Rank,
		"FactionKey",
		"Faction and Rank"
	};