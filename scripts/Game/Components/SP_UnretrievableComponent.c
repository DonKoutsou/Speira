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
	
	void GetInfo(out string OName, out string DName, out string DLoc)
	{
		OName = OwnerName;
		DName = DeliveryName;
		DLoc = DeliveryLocation;
	};
	void SetInfo(string OName, string DName, string DLoc)
	{
		OwnerName = OName;
		DeliveryName = DName;
		DeliveryLocation = DLoc;
	}
};
class SP_BountyComponentClass : SP_UnretrievableComponentClass
{
};

class SP_BountyComponent : SP_UnretrievableComponent
{
	string OwnerName;
	string TargetName;
	string TargetLocation;
	
	void GetInfo(out string OName, out string DName, out string TLoc)
	{
		OName = OwnerName;
		DName = TargetName;
		TLoc = TargetLocation;
	};
	void SetInfo(string OName, string DName, string TLoc)
	{
		OwnerName = OName;
		TargetName = DName;
		TargetLocation = TLoc;
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