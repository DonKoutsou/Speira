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