[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageTask : DialogueStage
{
	SP_Task t_Task;
	override void Perform(IEntity Character, IEntity Player)
	{
		SP_RequestManagerComponent TaskMan = SP_RequestManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SP_RequestManagerComponent));
		SP_BountyTask t_Bounty = SP_BountyTask.Cast(t_Task);
		SP_DeliverTask t_Deliver = SP_DeliverTask.Cast(t_Task);
		if(t_Bounty)
		{
			IEntity BountyPaper = t_Bounty.GetBountyEnt();
			InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
			InventoryStorageManagerComponent invChar = InventoryStorageManagerComponent.Cast(Character.FindComponent(InventoryStorageManagerComponent));
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(BountyPaper.FindComponent(InventoryItemComponent));
			InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
			invChar.TryRemoveItemFromStorage(BountyPaper, parentSlot.GetStorage());
			if(inv.TryInsertItem(BountyPaper))
			{
				SP_DialogueComponent diagcomp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
				diagcomp.DoAnouncerDialogue("The bounty has been added to your inventory");
			}
			else
			{
				SP_DialogueComponent diagcomp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
				diagcomp.DoAnouncerDialogue("No space in inventory, bounty left on the floor");
			}
			t_Bounty.AssignCharacter(Player);
		}
		if(t_Deliver)
		{
			IEntity Package = t_Deliver.GetPackage();
			InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
			InventoryStorageManagerComponent invChar = InventoryStorageManagerComponent.Cast(Character.FindComponent(InventoryStorageManagerComponent));
			InventoryItemComponent pInvComp = InventoryItemComponent.Cast(Package.FindComponent(InventoryItemComponent));
			InventoryStorageSlot parentSlot = pInvComp.GetParentSlot();
			invChar.TryRemoveItemFromStorage(Package, parentSlot.GetStorage());
			if(inv.TryInsertItem(Package))
			{
				SP_DialogueComponent diagcomp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
				diagcomp.DoAnouncerDialogue("The package has been added to your inventory");
			}
			else
			{
				SP_DialogueComponent diagcomp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
				diagcomp.DoAnouncerDialogue("No space in inventory, package left on the floor");
			}
			t_Deliver.AssignCharacter(Player);
		}
	};
	void SetupTask(SP_Task task)
	{
		t_Task = task;
	}
	override bool GetActionText(IEntity Character, IEntity Player, out string acttext)
	{
		if(t_Task && t_Task.CharacterAssigned(Player) == false)
		{
			acttext = t_Task.GetTaskDescription();
			return true;
		}
		return false;
	}
	override string GetStageDialogueText(IEntity Character, IEntity Player)
		{
			return t_Task.GetTaskDescription();
		 	
		}
}