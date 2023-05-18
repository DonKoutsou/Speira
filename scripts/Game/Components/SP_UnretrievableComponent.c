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