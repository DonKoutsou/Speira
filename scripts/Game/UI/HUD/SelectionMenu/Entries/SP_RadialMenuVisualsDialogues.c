class SP_RadialMenuVisualsDialogues : SCR_RadialMenuVisuals
{
	
	//------------------------------------------------------------------------------------------------
	override void SetContent(array<BaseSelectionMenuEntry> allEntries, array<BaseSelectionMenuEntry> disabledEntries, bool clearData = false)
	{
		super.SetContent(allEntries, disabledEntries, clearData);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void SetElementData(SCR_RadialMenuPair<Widget, BaseSelectionMenuEntry> element, bool canBePerformed, SCR_SelectionEntryWidgetComponent widgetComp)
	{
		super.SetElementData(element, canBePerformed, widgetComp);
		
		// Update entry visuals 
		ScriptedSelectionMenuEntry entry = ScriptedSelectionMenuEntry.Cast(element.m_pEntry);
		if (entry)
			entry.UpdateVisuals();
	}
}