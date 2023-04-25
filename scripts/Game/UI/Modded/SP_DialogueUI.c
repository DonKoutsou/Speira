modded enum ChimeraMenuPreset
{
    DialogueLayout
}
modded enum EModularButtonEventHandler
{
	BUTTON_1		= 1<<6,
	BUTTON_2		= 1<<7,
	BUTTON_3		= 1<<8,
	BUTTON_4		= 1<<9
};
class DialogueUIClass: ChimeraMenuBase
{ 
	//----------------------------------------------------------------//
	//UI Widgets
	protected Widget 							m_wRoot; 
	OverlayWidget 								m_ListBoxOverlay;
	TextWidget 									m_CharacterName;
	TextWidget 									m_CharacterRank;
	ImageWidget 								m_CharacterFactionIcon;
	SCR_ListBoxElementComponent 				m_ListBoxElement;
    SCR_ListBoxComponent 						m_ListBoxComponent;
    //----------------------------------------------------------------//
	//PlayerCharacter
	IEntity 									myUserEntity;           
	//----------------------------------------------------------------//
	//Charactaer we are talking to
	IEntity 									myCallerEntity;
	string 										CharName;
	string 										CharRank;
	ECharacterRank 								rank;
	FactionKey 									faction;
	//----------------------------------------------------------------//
	//DialogueStystem
	SP_DialogueComponent 						DiagComp;
	SP_DialogueArchetype 						DiagArch;
	protected SCR_BaseGameMode 					GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	int 										CurrentBranchID;
	ref ScriptInvoker 							m_OnKeyDown = new ScriptInvoker();
    //------------------------------------------------------------------------------------------------//
	void Init(IEntity Owner, IEntity User)
	{
		myCallerEntity = Owner;
		myUserEntity = User;
		m_wRoot = GetRootWidget();
		m_ListBoxOverlay = OverlayWidget.Cast(m_wRoot.FindAnyWidget("ListBox0")); 
		m_ListBoxComponent = SCR_ListBoxComponent.Cast(m_ListBoxOverlay.FindHandler(SCR_ListBoxComponent));
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		if(myCallerEntity)
		{
			CharName = DiagComp.GetCharacterName(myCallerEntity);
			rank = DiagComp.GetCharacterRank(myCallerEntity);
			faction = DiagComp.GetCharacterFaction(myCallerEntity);
		}
		switch(rank)
		{
			case 1:
			{
				CharRank = "Private";
			}
		}
		m_CharacterFactionIcon = ImageWidget.Cast(m_wRoot.FindAnyWidget("FactionIcon"));
		if (m_CharacterFactionIcon)
		{
		switch(faction)
		{
			case "US":
			{
				m_CharacterFactionIcon.LoadImageTexture(0, "{EB7F65DBC9392557}UI/Textures/Editor/EditableEntities/Factions/EditableEntity_Faction_USA.edds");
			}
			break;
			case "USSR":
			{
				m_CharacterFactionIcon.LoadImageTexture(0, "{40B12B0DF911B856}UI/Textures/Editor/EditableEntities/Factions/EditableEntity_Faction_USA.edds");
			}
			break;
			case "FIA":
			{
				m_CharacterFactionIcon.LoadImageTexture(0, "{FB266CDD4502D60B}UI/Textures/Editor/EditableEntities/Factions/EditableEntity_Faction_Fia.edds");
			}
			break;
		}
		}
		m_CharacterName = TextWidget.Cast(m_wRoot.FindAnyWidget("CharacterName"));
		m_CharacterName.SetText(CharName);
		
		m_CharacterName = TextWidget.Cast(m_wRoot.FindAnyWidget("CharacterRank"));
		m_CharacterName.SetText(CharRank);
		
	}
	//------------------------------------------------------------------------------------------------//
	void UpdateEntries(IEntity Character, IEntity Player)
	{
		string DiagText;
		int entryamount;
		//Check if any there arent inputs comming form GetActionName, if not do not create the item
		DiagText = DiagComp.GetActionName(0, myCallerEntity, Player);
		if (DiagText != STRING_EMPTY)
		{
			m_ListBoxComponent.AddItem(DiagText);
			CurrentBranchID = 0;
			SCR_ListBoxElementComponent elComp0 = m_ListBoxComponent.GetElementComponent(entryamount);
			elComp0.m_OnClicked.Insert(ExecuteDialogue0);
			string entrynumber = (entryamount + 1).ToString();
			elComp0.SetTextNumber(entrynumber);
			entryamount = entryamount + 1;
			DiagText = STRING_EMPTY;
		}
		DiagText = DiagComp.GetActionName(1, myCallerEntity, Player);
		if (DiagText != STRING_EMPTY)
		{
			m_ListBoxComponent.AddItem(DiagText);
			CurrentBranchID = 1;
			SCR_ListBoxElementComponent elComp1 = m_ListBoxComponent.GetElementComponent(entryamount);
			elComp1.m_OnClicked.Insert(ExecuteDialogue1);
			string entrynumber = (entryamount + 1).ToString();
			elComp1.SetTextNumber(entrynumber);
			entryamount = entryamount + 1;
			DiagText = STRING_EMPTY;
		}
		DiagText = DiagComp.GetActionName(2, myCallerEntity, Player);
		if (DiagText != STRING_EMPTY)
		{
			m_ListBoxComponent.AddItem(DiagText);
			CurrentBranchID = 2;
			SCR_ListBoxElementComponent elComp2 = m_ListBoxComponent.GetElementComponent(entryamount);
			elComp2.m_OnClicked.Insert(ExecuteDialogue2);
			string entrynumber = (entryamount + 1).ToString();
			elComp2.SetTextNumber(entrynumber);
			entryamount = entryamount + 1;
			DiagText = STRING_EMPTY;
		}
		DiagText = DiagComp.GetActionName(3, myCallerEntity, Player);
		if (DiagText != STRING_EMPTY)
		{
			m_ListBoxComponent.AddItem(DiagText);
			CurrentBranchID = 3;
			SCR_ListBoxElementComponent elComp3 = m_ListBoxComponent.GetElementComponent(entryamount);
			elComp3.m_OnClicked.Insert(ExecuteDialogue3);
			string entrynumber = (entryamount + 1).ToString();
			elComp3.SetTextNumber(entrynumber);
			entryamount = entryamount + 1;
			DiagText = STRING_EMPTY;
		}
		//Check if Archtype is branched an choose to create a Leave button or a Go Back button
		SP_DialogueArchetype DArch = DiagComp.LocateDialogueArchetype(myCallerEntity);
		if (DArch.IsCharacterBranched == true)
		{
			m_ListBoxComponent.AddItem("Go Back");
			SCR_ListBoxElementComponent elComp4 = m_ListBoxComponent.GetElementComponent(entryamount);
			elComp4.m_OnClicked.Insert(DoDialogueBack);
			string entrynumber = (entryamount + 1).ToString();
			elComp4.SetTextNumber(entrynumber);
			return;
		}
		m_ListBoxComponent.AddItem("Leave");
		SCR_ListBoxElementComponent elComp4 = m_ListBoxComponent.GetElementComponent(entryamount);
		elComp4.m_OnClicked.Insert(LeaveFunction);
		string entrynumber = (entryamount + 1).ToString();
		elComp4.SetTextNumber(entrynumber);
	}
	//------------------------------------------------------------------------------------------------//
	//Function called to close menu
    void LeaveFunction()
    {
		GetGame().GetMenuManager().CloseAllMenus();
    }
	//------------------------------------------------------------------------------------------------//
	//DoDialogue function wich branch ID 0
	void ExecuteDialogue0()
	{
		DiagComp.DoDialogue(myCallerEntity, myUserEntity, 0);
	}
	//------------------------------------------------------------------------------------------------//
	void ExecuteDialogue1()
	{
		DiagComp.DoDialogue(myCallerEntity, myUserEntity, 1);
	}
	//------------------------------------------------------------------------------------------------//
	void ExecuteDialogue2()
	{
		DiagComp.DoDialogue(myCallerEntity, myUserEntity, 2);
	}
	//------------------------------------------------------------------------------------------------//
	void ExecuteDialogue3()
	{
		DiagComp.DoDialogue(myCallerEntity, myUserEntity, 3);
	}
	//------------------------------------------------------------------------------------------------//
	void DoDialogueBack()
	{
		DiagComp.DoBackDialogue(myCallerEntity, myUserEntity);
	}
	//------------------------------------------------------------------------------------------------//
	override event bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if (key == 1)
		{
			OnClick(w, x, y, 0);
			m_OnKeyDown.Invoke(this);
			return true;
		}
		return false;
	}
}