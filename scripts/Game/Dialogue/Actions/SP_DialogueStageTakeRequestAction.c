[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageTakeRequestAction : DialogueStage
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ResourceName m_ItemToGive;
	
	[Attribute("", UIWidgets.Coords, params: "", desc: "")]
	vector m_SpawnOffset;
	
	[Attribute("4")]
	int m_MaxAmount;
	
	[Attribute()]
	private ResourceName m_pDefaultWaypoint;
	
	[Attribute("")]
	ref array<EEditableEntityLabel> m_labelstolookfor;
	
	string LocName;
	string m_RequestText;
	FactionKey TFact;
	SP_AIDirector MyDirector;
	
	string DName;
	
	override void Perform(IEntity Character, IEntity Player)
	{
		AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector OrDirector = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
		FactionAffiliationComponent FC = FactionAffiliationComponent.Cast(Character.FindComponent(FactionAffiliationComponent));
		SCR_Faction myfact = SCR_Faction.Cast(FC.GetAffiliatedFaction());
		
		string key = myfact.GetFactionKey();
		
		vector mat[4];
		Character.GetWorldTransform(mat);
		vector spawnPos = m_SpawnOffset.Multiply3(mat) + mat[3];
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos; 
		IEntity RequestPaper;
		array<EEditableEntityLabel> entityLabels = new array<EEditableEntityLabel>();
		SCR_EditableEntityUIInfo info = SCR_EditableEntityUIInfo.Cast(Character.FindComponent(SCR_EditableEntityUIInfo));
		array<SCR_EntityCatalogEntry> itemList = new array<SCR_EntityCatalogEntry>();
		SCR_EArsenalItemType type;
		SCR_EArsenalItemMode mode;
		SCR_EntityCatalog requestcatalog = myfact.GetFactionEntityCatalogOfType(EEntityCatalogType.ITEM);
		requestcatalog.GetEntityListWithData(SCR_ArsenalItem, itemList);
		m_RequestText = FigureDisShit(itemList, type, mode);
		//if(info)
		//{
		//	info.GetEntityLabels(entityLabels);
		//	SCR_EntityCatalog requestcatalog = myfact.GetFactionEntityCatalogOfType(EEntityCatalogType.ITEM);
		//	requestcatalog.GetFullFilteredEntityListWithLabels(itemList, entityLabels);
		//}
		Resource res = Resource.Load(m_ItemToGive);
		if (res)
		{	
			RequestPaper = GetGame().SpawnEntityPrefab(res, Character.GetWorld(), params);
		}
		InventoryStorageManagerComponent inv = InventoryStorageManagerComponent.Cast(Player.FindComponent(InventoryStorageManagerComponent));
		if(inv.TryInsertItem(RequestPaper))
		{
			SP_DialogueComponent diagcomp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
			diagcomp.DoAnouncerDialogue("The request has been added to your inventory");
		}
		else
		{
			SP_DialogueComponent diagcomp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
			diagcomp.DoAnouncerDialogue("No space in inventory, bounty left on the floor");
		}
		SP_ItemBountyComponent ItemBComp = SP_ItemBountyComponent.Cast(RequestPaper.FindComponent(SP_ItemBountyComponent));
		CharacterIdentityComponent CharID = CharacterIdentityComponent.Cast(Character.FindComponent(CharacterIdentityComponent));
		SCR_CharacterRankComponent CharRank = SCR_CharacterRankComponent.Cast(Character.FindComponent(SCR_CharacterRankComponent));
		string OName = CharRank.GetCharacterRankName(Character) + " " + CharID.GetIdentity().GetName() + " " + CharID.GetIdentity().GetSurname();
		int amount = Math.RandomInt(0, m_MaxAmount);
		string RequestLocName = MyDirector.GetCharacterLocation(Character);
		ItemBComp.SetInfo(OName, type, mode, amount, RequestLocName);
		TFact = key;
	};
	override string GetDialogueText(IEntity Character, IEntity Player)
	{
			return GetRandomDeliver();
	}
	string GetRandomDeliver()
	{
		string TextToSend = string.Format(DialogueText, m_RequestText);
		return TextToSend;
	}
	string FigureDisShit(array<SCR_EntityCatalogEntry> itemList, out SCR_EArsenalItemType type, out SCR_EArsenalItemMode mode)
	{
		string RequestText;

		int index = Math.RandomInt(0, itemList.Count());
		SCR_ArsenalItem Arsenal = SCR_ArsenalItem.Cast(itemList[index].GetEntityDataOfType(SCR_ArsenalItem));
		type = Arsenal.GetItemType();
		mode = Arsenal.GetItemMode();
		RequestText = typename.EnumToString(SCR_EArsenalItemType, type) + " " + typename.EnumToString(SCR_EArsenalItemMode, mode);
		
		return RequestText;
		
	}
}