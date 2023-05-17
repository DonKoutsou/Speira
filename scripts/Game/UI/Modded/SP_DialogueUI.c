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
	SCR_ECharacterRank 								rank;
	FactionKey 									faction;
	//----------------------------------------------------------------//
	//DialogueStystem
	SP_DialogueComponent 						DiagComp;
	SP_DialogueArchetype 						DiagArch;
	protected SCR_BaseGameMode 					GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	int 										CurrentBranchID;
	ref ScriptInvoker 							m_OnKeyDown = new ScriptInvoker();
    //------------------------------------------------------------------------------------------------//
	override void OnMenuUpdate(float tDelta)
	{
		GetGame().GetInputManager().ActivateContext("DialogueMenu", 0);
	}
	void RemoveListeners()
	{
		GetGame().GetInputManager().RemoveActionListener("Dialogue0", EActionTrigger.DOWN, ExecuteDialogue0);
		GetGame().GetInputManager().RemoveActionListener("Dialogue1", EActionTrigger.DOWN, ExecuteDialogue1);
		GetGame().GetInputManager().RemoveActionListener("Dialogue2", EActionTrigger.DOWN, ExecuteDialogue2);
		//GetGame().GetInputManager().RemoveActionListener("Dialogue3", EActionTrigger.DOWN, ExecuteDialogue3);
		GetGame().GetInputManager().RemoveActionListener("DialogueBack", EActionTrigger.DOWN, DoDialogueBack);
		GetGame().GetInputManager().RemoveActionListener("DialogueBack", EActionTrigger.DOWN, LeaveFunction);
	}
	void Init(IEntity Owner, IEntity User)
	{
		
		myCallerEntity = Owner;
		myUserEntity = User;
		m_wRoot = GetRootWidget();
		m_ListBoxOverlay = OverlayWidget.Cast(m_wRoot.FindAnyWidget("ListBox0")); 
		m_ListBoxComponent = SCR_ListBoxComponent.Cast(m_ListBoxOverlay.FindHandler(SCR_ListBoxComponent));
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		
	}
	//------------------------------------------------------------------------------------------------//
	void UpdateEntries(IEntity Character, IEntity Player)
	{
		myCallerEntity = Character;
		myUserEntity = Player;
		m_wRoot = GetRootWidget();
		m_ListBoxOverlay = OverlayWidget.Cast(m_wRoot.FindAnyWidget("ListBox0")); 
		m_ListBoxComponent = SCR_ListBoxComponent.Cast(m_ListBoxOverlay.FindHandler(SCR_ListBoxComponent));
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		string DiagText;
		int entryamount;
		//Check if any there arent inputs comming form GetActionName, if not do not create the item
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
			break;
			case 8:
			{
				CharRank = "GENERAL";
			}
			break;
			case 22:
			{
				CharRank = "Commander";
			}
			break;
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
			case "BANDITS":
			{
				m_CharacterFactionIcon.LoadImageTexture(0, "{855625FE8D6A87A8}UI/Textures/DamageState/damageState-dead.edds");
			}
			break;
			case "SPEIRA":
			{
				m_CharacterFactionIcon.LoadImageTexture(0, "{CEAC8771342689C5}Assets/Data/Faction_SPEIRA.edds");
			}
			break;
				
		}
		}
		m_CharacterName = TextWidget.Cast(m_wRoot.FindAnyWidget("CharacterName"));
		m_CharacterName.SetText(CharName);
		
		m_CharacterName = TextWidget.Cast(m_wRoot.FindAnyWidget("CharacterRank"));
		m_CharacterName.SetText(CharRank);
		
		DiagText = DiagComp.GetActionName(0, myCallerEntity, Player);
		if (DiagText != STRING_EMPTY)
		{
			m_ListBoxComponent.AddItem(DiagText);
			CurrentBranchID = 0;
			SCR_ListBoxElementComponent elComp0 = m_ListBoxComponent.GetElementComponent(entryamount);
			elComp0.m_OnClicked.Insert(ExecuteDialogue0);
			string entrynumber = (entryamount + 1).ToString();
			if(GetGame().GetInputManager().GetLastUsedInputDevice() == EInputDeviceType.GAMEPAD)
			{
				elComp0.SetTextNumber("A");
			}
			else
			{
				elComp0.SetTextNumber(entrynumber);
			}
			entryamount = entryamount + 1;
			DiagText = STRING_EMPTY;
			GetGame().GetInputManager().AddActionListener("Dialogue0", EActionTrigger.DOWN, ExecuteDialogue0);
		}
		DiagText = DiagComp.GetActionName(1, myCallerEntity, Player);
		if (DiagText != STRING_EMPTY)
		{
			m_ListBoxComponent.AddItem(DiagText);
			CurrentBranchID = 1;
			SCR_ListBoxElementComponent elComp1 = m_ListBoxComponent.GetElementComponent(entryamount);
			elComp1.m_OnClicked.Insert(ExecuteDialogue1);
			string entrynumber = (entryamount + 1).ToString();
			if(GetGame().GetInputManager().GetLastUsedInputDevice() == EInputDeviceType.GAMEPAD)
			{
				elComp1.SetTextNumber("X");
			}
			else
			{
				elComp1.SetTextNumber(entrynumber);
			}
			entryamount = entryamount + 1;
			DiagText = STRING_EMPTY;
			GetGame().GetInputManager().AddActionListener("Dialogue1", EActionTrigger.DOWN, ExecuteDialogue1);
		}
		DiagText = DiagComp.GetActionName(2, myCallerEntity, Player);
		if (DiagText != STRING_EMPTY)
		{
			m_ListBoxComponent.AddItem(DiagText);
			CurrentBranchID = 2;
			SCR_ListBoxElementComponent elComp2 = m_ListBoxComponent.GetElementComponent(entryamount);
			elComp2.m_OnClicked.Insert(ExecuteDialogue2);
			string entrynumber = (entryamount + 1).ToString();
			if(GetGame().GetInputManager().GetLastUsedInputDevice() == EInputDeviceType.GAMEPAD)
			{
				elComp2.SetTextNumber("Y");
			}
			else
			{
				elComp2.SetTextNumber(entrynumber);
			}
			entryamount = entryamount + 1;
			DiagText = STRING_EMPTY;
			GetGame().GetInputManager().AddActionListener("Dialogue2", EActionTrigger.DOWN, ExecuteDialogue2);
		}
		//DiagText = DiagComp.GetActionName(3, myCallerEntity, Player);
		//if (DiagText != STRING_EMPTY)
		//{
		//	m_ListBoxComponent.AddItem(DiagText);
		//	CurrentBranchID = 3;
		//	SCR_ListBoxElementComponent elComp3 = m_ListBoxComponent.GetElementComponent(entryamount);
		//	elComp3.m_OnClicked.Insert(ExecuteDialogue3);
		//	string entrynumber = (entryamount + 1).ToString();
		//	elComp3.SetTextNumber(entrynumber);
		//	entryamount = entryamount + 1;
		//	DiagText = STRING_EMPTY;
		//	GetGame().GetInputManager().AddActionListener("Dialogue3", EActionTrigger.DOWN, ExecuteDialogue3);
		//}
		//Check if Archtype is branched an choose to create a Leave button or a Go Back button
		SP_DialogueArchetype DArch = DiagComp.LocateDialogueArchetype(myCallerEntity, Player);
		if (DArch.IsCharacterBranched == true)
		{
			m_ListBoxComponent.AddItem("Go Back");
			SCR_ListBoxElementComponent elComp4 = m_ListBoxComponent.GetElementComponent(entryamount);
			elComp4.m_OnClicked.Insert(DoDialogueBack);
			if(GetGame().GetInputManager().GetLastUsedInputDevice() == EInputDeviceType.GAMEPAD)
			{
				elComp4.SetTextNumber("B");
			}
			else
			{
				elComp4.SetTextNumber("[BACKSPACE]");
			}
			GetGame().GetInputManager().AddActionListener("DialogueBack", EActionTrigger.DOWN, DoDialogueBack);
			return;
		}
		m_ListBoxComponent.AddItem("Leave");
		SCR_ListBoxElementComponent elComp4 = m_ListBoxComponent.GetElementComponent(entryamount);
		elComp4.m_OnClicked.Insert(LeaveFunction);
		if(GetGame().GetInputManager().GetLastUsedInputDevice() == EInputDeviceType.GAMEPAD)
			{
				elComp4.SetTextNumber("B");
			}
			else
			{
				elComp4.SetTextNumber("[BACKSPACE]");
			}
		GetGame().GetInputManager().AddActionListener("DialogueBack", EActionTrigger.DOWN, LeaveFunction);
	}
	//------------------------------------------------------------------------------------------------//
	//Function called to close menu
    void LeaveFunction()
    {
		RemoveListeners();
		DiagComp.DoBackDialogue(myCallerEntity, myUserEntity);
		DiagComp.ReleaseAI(myCallerEntity, myUserEntity);
		GetGame().GetMenuManager().CloseAllMenus();
    }
	override void OnMenuClose()
	{
		RemoveListeners();
	}
	//------------------------------------------------------------------------------------------------//
	//DoDialogue function wich branch ID 0
	void ExecuteDialogue0()
	{
		RemoveListeners();
		DiagComp.DoDialogue(myCallerEntity, myUserEntity, 0);
	}
	//------------------------------------------------------------------------------------------------//
	void ExecuteDialogue1()
	{
		RemoveListeners();
		DiagComp.DoDialogue(myCallerEntity, myUserEntity, 1);
	}
	//------------------------------------------------------------------------------------------------//
	void ExecuteDialogue2()
	{
		RemoveListeners();
		DiagComp.DoDialogue(myCallerEntity, myUserEntity, 2);
	}
	//------------------------------------------------------------------------------------------------//
	//void ExecuteDialogue3()
	//{
	//	RemoveListeners();
	//	DiagComp.DoDialogue(myCallerEntity, myUserEntity, 3);
	//}
	//------------------------------------------------------------------------------------------------//
	void DoDialogueBack()
	{
		RemoveListeners();
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