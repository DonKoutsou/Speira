[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageTakePackageAction : DialogueStage
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, params: "et", desc: "")]
	ref array <ref ResourceName> m_ItemToGive;
	[Attribute("", UIWidgets.Coords, params: "", desc: "")]
	vector m_SpawnOffset;
	
	SP_AIDirector MyDirector;
	
	string DName;
	
	override void Perform(IEntity Character, IEntity Player)
	{
		FactionAffiliationComponent FC = FactionAffiliationComponent.Cast(Character.FindComponent(FactionAffiliationComponent));
		string key = FC.GetAffiliatedFaction().GetFactionKey();
		int index;
		index = Math.RandomInt(0, SP_AIDirector.AllDirectors.Count());
		MyDirector = SP_AIDirector.AllDirectors[index];
		MyDirector.GetDirectorOccupiedBy(key, MyDirector);
		vector mat[4];
		Character.GetWorldTransform(mat);
		vector spawnPos = m_SpawnOffset.Multiply3(mat) + mat[3];
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos; 
		IEntity Package;
		for (int i = 0; i < m_ItemToGive.Count(); i++)
		{
			Resource res = Resource.Load(m_ItemToGive[i]);
			if (res)
			{	
				Package = GetGame().SpawnEntityPrefab(res, Character.GetWorld(), params);
			}
		}
		SP_PackageComponent PComp = SP_PackageComponent.Cast(Package.FindComponent(SP_PackageComponent));
		string LocName = MyDirector.GetLocationName();
		CharacterIdentityComponent CharID = CharacterIdentityComponent.Cast(Character.FindComponent(CharacterIdentityComponent));
		string OName = CharID.GetIdentity().GetName() + " " + CharID.GetIdentity().GetSurname();
		IEntity CharToDeliverTo;
		MyDirector.GetRandomUnitByFKey(key, CharToDeliverTo);
		CharacterIdentityComponent CharID2 = CharacterIdentityComponent.Cast(CharToDeliverTo.FindComponent(CharacterIdentityComponent));
		DName = CharID2.GetIdentity().GetName() + " " + CharID2.GetIdentity().GetSurname();
		PComp.SetInfo(OName, DName, LocName);
	};
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		return true;
	}
	override string GetDialogueText(IEntity Character, IEntity Player)
	{
			FactionAffiliationComponent FC = FactionAffiliationComponent.Cast(Character.FindComponent(FactionAffiliationComponent));
			string key = FC.GetAffiliatedFaction().GetFactionKey();
			return GetRandomDeliver(key);
	};
	string GetRandomDeliver(string key)
	{
		string TextToSend = string.Format(DialogueText, DName, MyDirector.GetLocationName());
		return TextToSend;
	}

};