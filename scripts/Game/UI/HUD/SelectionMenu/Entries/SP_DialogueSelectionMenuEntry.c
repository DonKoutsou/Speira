[BaseContainerProps(configRoot: true)]
class SP_Dialogues : ScriptAndConfig
{
	[Attribute("", UIWidgets.Object, "")]
	ref array<ref SP_DiagMenuCategory> m_aDialogueCategories;
}

class SP_DiagMenuCategory : BaseSelectionMenuCategory
{
	[Attribute("", UIWidgets.EditBox, "")]
	protected string m_sDiagCategoryName;
	[Attribute("", UIWidgets.Object, "")]
	protected ref array<ref BaseSelectionMenuEntry> m_aDiagCategoryContent;
	//------------------------------------------------------------------------------------------------
	override void AddElementToCategory(BaseSelectionMenuEntry element)
	{
		m_aDiagCategoryContent.Insert(element);
	}

	//------------------------------------------------------------------------------------------------
	override array<ref BaseSelectionMenuEntry> GetCategoryElements()
	{
		return m_aDiagCategoryContent;
	}
}

class SP_DialogueSelectionMenuEntry : SCR_BaseGroupEntry
{
	
	[Attribute( defvalue: "", desc: "DialogueBranchKey, Key used to define wich action should use this config", category: "Dialogue",  )]			//TODO: make config, memory
	protected int m_iDialogueBranchKey;
	[Attribute( defvalue: "", desc: "ID used to collect texts for DialogueComponent",  )]			//TODO: make config, memory
	protected int EntryID;
	[Attribute()]
	bool IsExit;
	
	string m_sPageName;
	protected string m_sActionText;
	protected string m_sDialogueText;
	protected SCR_BaseGameMode GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	protected SP_DialogueComponent DiagComp;
	protected SCR_RadialMenuComponent RadComp;
	protected SCR_RadialMenuHandler RadHand;
	protected SP_DialogueConfig DiagConf;
	//------------------------------------------------------------------//
	void SetDialogueConfig(SP_DialogueConfig DConf)
	{
		DiagConf = DConf;
		return;
	}
	override void UpdateVisuals()
	{
		
		RadComp = SCR_RadialMenuComponent.Cast(GameMode.FindComponent(SCR_RadialMenuComponent));
		RadHand = SCR_RadialMenuHandler.Cast(RadComp.GetRadialMenuHandler());
		SP_RadialMenuDiags RadDiag = SP_RadialMenuDiags.Cast(RadHand);
		IEntity owner = RadDiag.GetTalkingChar();
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		m_sDialogueText = DiagComp.GetRadialActionName(m_iDialogueBranchKey, owner, EntryID);
		if (IsExit == true)
		{
			m_sDialogueText = "Leave";
		}
		if (m_EntryComponent)
			m_EntryComponent.SetLabelText(m_sDialogueText);
	}
	IEntity GetTalkingCharacter()
	{
		RadComp = SCR_RadialMenuComponent.Cast(GameMode.FindComponent(SCR_RadialMenuComponent));
		RadHand = SCR_RadialMenuHandler.Cast(RadComp.GetRadialMenuHandler());
		SP_RadialMenuDiags RadDiag = SP_RadialMenuDiags.Cast(RadHand);
		IEntity owner = RadDiag.GetTalkingChar();
		return owner;
	}
	protected override event void OnPerform(IEntity user, BaseSelectionMenu sourceMenu)
	{
		

		RadComp = SCR_RadialMenuComponent.Cast(GameMode.FindComponent(SCR_RadialMenuComponent));
		SP_RadialMenuDiags RadMenuDiags = SP_RadialMenuDiags.Cast(RadComp.GetRadialMenuHandler());
		if (IsExit == true)
		{
			RadMenuDiags.Close(user);
			return;
		}
		RadHand = SCR_RadialMenuHandler.Cast(RadComp.GetRadialMenuHandler());
		SP_RadialMenuDiags RadDiag = SP_RadialMenuDiags.Cast(RadHand);
		IEntity owner = RadDiag.GetTalkingChar();
		super.OnPerform(user, sourceMenu);
		
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		
		if (DiagComp)
		{
			IEntity talkingchar = RadMenuDiags.GetTalkingChar();
			//SendText function on dialogue component, sending all gathered data from action
			DiagComp.DoRadialDialogue(owner, talkingchar, m_iDialogueBranchKey, 1, EntryID);
		}
		
		return;
	}
	override bool GetEntryNameScript(out string outName)
	{
		RadComp = SCR_RadialMenuComponent.Cast(GameMode.FindComponent(SCR_RadialMenuComponent));
		RadHand = SCR_RadialMenuHandler.Cast(RadComp.GetRadialMenuHandler());
		SP_RadialMenuDiags RadDiag = SP_RadialMenuDiags.Cast(RadHand);
		IEntity owner = RadDiag.GetTalkingChar();
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		outName = DiagComp.GetActionName(m_iDialogueBranchKey, owner);
		if(!outName)
		{
			return false;
		}
		return true;
	}
	override string GetName() 
	{
		 return m_sDialogueText;
	}
	override bool GetEntryDescriptionScript(out string outDescription)
	{
		outDescription = m_sDialogueText;
		return true;
	}
	//------------------------------------------------------------------//
	int GetDialogueBranchKey()
	{
		return m_iDialogueBranchKey;
	}
	//------------------------------------------------------------------//
	string GetActionText(int stageid)
	{
		string actiontext;
		actiontext = m_sActionText;
		
		return actiontext;
	}
};
enum EChoiseBehavior
	{
		"Accept&Close",
		"AlrernateDialogue",
		"Stay"
	};