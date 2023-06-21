class SP_UnretrievableComponentClass : GameComponentClass
{
};

class SP_UnretrievableComponent : GameComponent
{
};

class SP_PackageComponentClass : SP_UnretrievableComponentClass
{
};

class SP_PackageComponent : SP_UnretrievableComponent
{
	string OwnerName;
	string DeliveryName;
	string DeliveryLocation;
	FactionKey m_Faction;
	void GetInfo(out string OName, out string DName, out string DLoc, out FactionKey faction)
	{
		OName = OwnerName;
		DName = DeliveryName;
		DLoc = DeliveryLocation;
		faction = m_Faction;
	};
	void SetInfo(string OName, string DName, string DLoc, FactionKey faction)
	{
		OwnerName = OName;
		DeliveryName = DName;
		DeliveryLocation = DLoc;
		m_Faction = faction;
	}
};
class SP_BountyComponentClass : SP_UnretrievableComponentClass
{
};

class SP_BountyComponent : SP_UnretrievableComponent
{
	string OwnerName;
	IEntity Target;
	string TargetName;
	string TargetLocation;
	FactionKey m_Faction;
	void GetInfo(out string OName, out IEntity target, out string DName, out string TLoc, out FactionKey faction)
	{
		OName = OwnerName;
		DName = TargetName;
		target = Target;
		TLoc = TargetLocation;
		faction = m_Faction;
	};
	void SetInfo(string OName, IEntity target, string DName, string TLoc, FactionKey faction)
	{
		OwnerName = OName;
		TargetName = DName;
		Target = target;
		TargetLocation = TLoc;
		m_Faction = faction;
	}
	string GetTargetName()
	{
		if(TargetName)
		{
			return TargetName;
		}
		return STRING_EMPTY;
	}
};
class SP_ItemBountyComponentClass : SP_UnretrievableComponentClass
{
};

class SP_ItemBountyComponent : SP_UnretrievableComponent
{
	string OwnerName;
	int ItemAmount;
	string OwnerLocation;
	SCR_EArsenalItemType	RequestType;
	SCR_EArsenalItemMode	RequestMode;

	void GetInfo(out string OName, out SCR_EArsenalItemType type, out SCR_EArsenalItemMode mode, out int Amount, out string TLoc)
	{
		OName = OwnerName;
		type = RequestType;
		mode = RequestMode;
		Amount = ItemAmount;
		TLoc = OwnerLocation;
	};
	void SetInfo(string OName, SCR_EArsenalItemType type, SCR_EArsenalItemMode mode ,int Amount, string TLoc)
	{
		OwnerName = OName;
		RequestType = type;
		RequestMode = mode;
		ItemAmount = Amount;
		OwnerLocation = TLoc;
	}
};
modded enum EEntityCatalogType
{
	REQUEST 						= 1000
}