class SP_RadialMenuDiags : SCR_RadialMenuHandler
{
	//----------------------------------------------------------------------------------------------------------------//
	[Attribute("", UIWidgets.Object, "Diag config")]
	protected ref SP_Dialogues m_DialogueConfig;
	//----------------------------------------------------------------------------------------------------------------//
	IEntity TalkingChar;
	
	protected static ResourceName m_EntryLayout = "{D263AE2495703ECD}UI/layouts/Common/RadialMenu/RadialEntryElementTextOnly.layout";
	
	void Start(IEntity owner, IEntity user)
	{
		TalkingChar = owner;
		Open(user);
	}
	IEntity GetTalkingChar()
	{
		return TalkingChar;
	}
	override protected void OpenMenu(IEntity owner, bool isOpen)
	{
		super.OpenMenu(owner, isOpen);
		
		if (isOpen)
		{
			m_pSource = SCR_PlayerController.GetLocalMainEntity();
			m_ActualCategory = m_RootCategory;
			PageSetup();
		}
	}
	override protected void Init(IEntity owner)
	{	
		super.Init(owner);
		
		m_pOwner = owner;
	}
	override protected void PageSetup()
	{
		m_aMenuPages.Clear();
		ClearEntries();
		
		foreach (SP_DiagMenuCategory DialogueCategory : m_DialogueConfig.m_aDialogueCategories)
		{
			SCR_MenuPage newPage = new SCR_MenuPage(DialogueCategory.GetCategoryName());
			m_aMenuPages.Insert(newPage);
			foreach (BaseSelectionMenuEntry entry : DialogueCategory.GetCategoryElements())
			{
				SP_DialogueSelectionMenuEntry DiagEntry = SP_DialogueSelectionMenuEntry.Cast(entry);
				DiagEntry.SetEntryLayout(m_EntryLayout);
				DiagEntry.m_sPageName = DialogueCategory.GetCategoryName();
				newPage.AddEntry(DiagEntry);
			}
		}
		SetPage(0);
		UpdateEntries();
	}
	//SCR_RadialMenuComponent GetDiagMenu()
	//{
	//	return SP_RadialMenuDiags;
	//}
}
enum ERadialMenudDIAGSPages
{
	DIAGS = 0
};