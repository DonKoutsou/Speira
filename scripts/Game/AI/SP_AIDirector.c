class SP_AIDirectorClass: AIGroupClass
{
};
class SP_AIDirector : AIGroup
{
	[Attribute("0", UIWidgets.ComboBox, "Select Entity Catalog type for random spawn", "", ParamEnumArray.FromEnum(EEntityCatalogType), category: "Randomization")]
	protected EEntityCatalogType	m_eEntityCatalogType;
	
	[Attribute("0", UIWidgets.ComboBox, "Select Entity Labels which you want to optionally include to random spawn. If you want to spawn everything, you can leave it out empty and also leave Include Only Selected Labels attribute to false.", "", ParamEnumArray.FromEnum(EEditableEntityLabel), category: "Randomization")]
	protected ref array<EEditableEntityLabel> 		m_aIncludedEditableEntityLabels;
	
	[Attribute("0", UIWidgets.ComboBox, "Select Entity Labels which you want to exclude from random spawn", "", ParamEnumArray.FromEnum(EEditableEntityLabel), category: "Randomization")]
	protected ref array<EEditableEntityLabel> 		m_aExcludedEditableEntityLabels;
	
	[Attribute(desc: "If true, it will spawn only the entities that are from Included Editable Entity Labels and also do not contain Label to be Excluded.", category: "Randomization")]
	protected bool				m_bIncludeOnlySelectedLabels;
	
	[Attribute("", category: "Randomization")]
	ref array<FactionKey> m_FactionsToApear;
	
	static ref array<SP_AIDirector> AllDirectors = null; // new ref array<SP_AIDirector>();
	// parameters
	[Attribute("3", category: "Spawning settings")]
	int m_MaxAgentsToSpawn;
	
	[Attribute("100", category: "Spawning settings")]
	float m_Radius;
	
	[Attribute("true", category: "Spawning settings")]
	bool m_Respawn;
	
	[Attribute("true", category: "Spawning settings")]
	bool m_SpawnAI;
	
	[Attribute("false", category: "Spawning settings")]
	bool m_CommanderRespawn;
	
	[Attribute("0", category: "Spawning settings")]
	float m_RespawnTimer;
	
	[Attribute("0", category: "Spawning settings")]
	float m_CommanderRespawnTimer;
	
	[Attribute("", category: "Spawning settings")]
	ResourceName m_Commander;
	
	[Attribute("", category: "Spawning settings")]
	private ResourceName m_pDefaultWaypoint;
	
	[Attribute("", category: "Spawning settings")]
	private ResourceName m_pCommanderWaypoint;

	[Attribute()]
    protected ref SCR_MapLocationQuadHint m_WorldDirections;
	bool commanderspawned = false;
	string m_sLocationName;
	protected int m_iGridSizeX;
	protected int m_iGridSizeY;
	protected const float angleA = 0.775;
	protected const float angleB = 0.325;
	
	// private
	private int m_SpawnedCounter;	
	private float m_RespawnPeriod;
	private float m_CommanderRespawnPeriod;
	private float m_RenegadeRespawnPeriod;
	private ref array<SCR_AIGroup> m_aGroups = new array<SCR_AIGroup>();
	private ref array<SCR_SiteSlotEntity> m_Slots = {};
	private AIWaypoint DefWaypoint;
	private AIWaypoint ComWaypoint;
	protected IEntity m_CommanderEnt;
	protected SP_DialogueComponent DiagComp;
	
	string GetLocationName ()
	{
		return m_sLocationName;
	}
	bool GetDirectorOccupiedByFriendly(Faction faction, out SP_AIDirector Director)
	{
		ref array<SP_AIDirector> Directors = SP_AIDirector.AllDirectors;
		ref array<SP_AIDirector> FrDirectors = new ref array<SP_AIDirector>;
		int UnitCount;
		for (int i = 0; i < Directors.Count(); i++)
		{
			FactionKey FKey;
			int Count;
			Faction Key2 = Directors[i].GetMajorityHolderNCount(FKey, Count);
			if(faction.IsFactionFriendly(Key2))
			{
				FrDirectors.Insert(Directors[i]);
				UnitCount = Count;
			}
			
			
		}
		if(FrDirectors.Count() <= 0)
		{
			return false;
		}	
		else
		{
			int index = Math.RandomInt(0, FrDirectors.Count());
			Director = FrDirectors[index];
			return true;
		}
		return false;
	}
	bool GetDirectorOccupiedByEnemy(Faction faction, out SP_AIDirector Director)
	{
		ref array<SP_AIDirector> Directors = SP_AIDirector.AllDirectors;
		ref array<SP_AIDirector> EnDirectors = new ref array<SP_AIDirector>;
		int UnitCount;
		for (int i = 0; i < Directors.Count(); i++)
		{
			FactionKey FKey;
			int Count;
			Faction Key2 = Directors[i].GetMajorityHolderNCount(FKey, Count);
			if(faction.IsFactionEnemy(Key2))
			{
				EnDirectors.Insert(Directors[i]);
				UnitCount = Count;
			}
			
			
		}
		if(EnDirectors.Count() <= 0)
		{
			return false;
		}	
		else
		{
			int index = Math.RandomInt(0, EnDirectors.Count());
			Director = EnDirectors[index];
			return true;
		}
		return false;
	}
	bool GetDirectorOccupiedBy(FactionKey Key, out SP_AIDirector Director)
	{
		ref array<SP_AIDirector> Directors = SP_AIDirector.AllDirectors;
		ref array<SP_AIDirector> FrDirectors = new ref array<SP_AIDirector>;
		int UnitCount;
		for (int i = 0; i < Directors.Count(); i++)
		{
			FactionKey FKey;
			int Count;
			Faction Key2 = Directors[i].GetMajorityHolderNCount(FKey, Count);
			if (Key2.GetFactionKey() == Key && Directors[i] != this)
			{
				FrDirectors.Insert(Directors[i]);
				UnitCount = Count;
			}
		}
		if(FrDirectors.Count() <= 0)
		{
			return false;
		}	
		else
		{
			int index = Math.RandomInt(0, FrDirectors.Count());
			Director = FrDirectors[index];
			return true;
		}
		return false;
	}
	bool GetRandomUnitByFKey(FactionKey Key, out IEntity Char)
	{
		for (int i = m_aGroups.Count() - 1; i >= 0; i--)
		{
			string faction = m_aGroups[i].GetFaction().GetFactionKey();
			if (faction && faction == Key)
			{
				array<AIAgent> outAgents = new array<AIAgent>();
				m_aGroups[i].GetAgents(outAgents);
				int z = Math.RandomInt(0,outAgents.Count());				
				Char = outAgents[z].GetControlledEntity();
				return true;
			}
		}
		return false;
	}
	Faction GetMajorityHolderNCount(out string factionReadable, out int UnitCount)
	{
		int USSRcount = 0;
	    int UScount = 0;
	    int FIAcount = 0;
	    int Banditcount = 0;
	    int Renegcount = 0;
	
	    FactionManager FMan = FactionManager.Cast(GetGame().GetFactionManager());
	    Faction MajorFaction = null;
	    int max = 0;
	    for (int i = m_aGroups.Count() - 1; i >= 0; i--)
	    {
			if(m_aGroups[i] == null)
			{
				return null;
			}
	        string faction = m_aGroups[i].GetFaction().GetFactionKey();
	        int agentsCount = m_aGroups[i].GetAgentsCount();
	
	        switch (faction)
	        {
	            case "USSR":
	                USSRcount += agentsCount;
	                if (USSRcount > max)
	                {
	                    max = USSRcount;
	                    MajorFaction = FMan.GetFactionByKey("USSR");
	                    factionReadable = "soviet";
	                }
	                break;
	            case "US":
	                UScount += agentsCount;
	                if (UScount > max)
	                {
	                    max = UScount;
	                    MajorFaction = FMan.GetFactionByKey("US");
	                    factionReadable = "US";
	                }
	                break;
	            case "FIA":
	                FIAcount += agentsCount;
	                if (FIAcount > max)
	                {
	                    max = FIAcount;
	                    MajorFaction = FMan.GetFactionByKey("FIA");
	                    factionReadable = "guerrilla";
	                }
	                break;
	            case "BANDITS":
	                Banditcount += agentsCount;
	                if (Banditcount > max)
	                {
	                    max = Banditcount;
	                    MajorFaction = FMan.GetFactionByKey("BANDITS");
	                    factionReadable = "bandit";
	                }
	                break;
	            case "RENEGADE":
	                Renegcount += agentsCount;
	                if (Renegcount > max)
	                {
	                    max = Renegcount;
	                    MajorFaction = FMan.GetFactionByKey("RENEGADE");
	                    factionReadable = "renegade";
	                }
	                break;
	        }
	    }
	
		UnitCount = max;
		if(!MajorFaction)
			return null;
	    return MajorFaction; 	
	}
	Faction GetMajorityHolder(out string factionReadable)
	{
	    int USSRcount = 0;
	    int UScount = 0;
	    int FIAcount = 0;
	    int Banditcount = 0;
	    int Renegcount = 0;
	
	    FactionManager FMan = FactionManager.Cast(GetGame().GetFactionManager());
	    Faction MajorFaction = null;
	    int max = 0;
	
	    for (int i = m_aGroups.Count() - 1; i >= 0; i--)
	    {
			if(m_aGroups[i] == null)
			{
				return null;
			}
	        string faction = m_aGroups[i].GetFaction().GetFactionKey();
	        int agentsCount = m_aGroups[i].GetAgentsCount();
	
	        switch (faction)
	        {
	            case "USSR":
	                USSRcount += agentsCount;
	                if (USSRcount > max)
	                {
	                    max = USSRcount;
	                    MajorFaction = FMan.GetFactionByKey("USSR");
	                    factionReadable = "soviet";
	                }
	                break;
	            case "US":
	                UScount += agentsCount;
	                if (UScount > max)
	                {
	                    max = UScount;
	                    MajorFaction = FMan.GetFactionByKey("US");
	                    factionReadable = "US";
	                }
	                break;
	            case "FIA":
	                FIAcount += agentsCount;
	                if (FIAcount > max)
	                {
	                    max = FIAcount;
	                    MajorFaction = FMan.GetFactionByKey("FIA");
	                    factionReadable = "guerrilla";
	                }
	                break;
	            case "BANDITS":
	                Banditcount += agentsCount;
	                if (Banditcount > max)
	                {
	                    max = Banditcount;
	                    MajorFaction = FMan.GetFactionByKey("BANDITS");
	                    factionReadable = "bandit";
	                }
	                break;
	            case "RENEGADE":
	                Renegcount += agentsCount;
	                if (Renegcount > max)
	                {
	                    max = Renegcount;
	                    MajorFaction = FMan.GetFactionByKey("RENEGADE");
	                    factionReadable = "renegade";
	                }
	                break;
	        }
	    }
	
	    return MajorFaction;
	}
	void SP_AIDirector(IEntitySource src, IEntity parent)
	{
		SetFlags(EntityFlags.ACTIVE, false);	
		SetEventMask(EntityEvent.INIT);

		
		// allow AI spawning only on the server
		if (RplSession.Mode() != RplMode.Client)
			SetEventMask(EntityEvent.FRAME);
	}
	
	void ~SP_AIDirector()
	{
		if(AllDirectors)
			AllDirectors.Remove(AllDirectors.Find(this));
	}	
	protected int GetGridIndex(int x, int y)
	{
		return 3*y + x;
	}
	string GetCharacterLocation(IEntity Character)
	{
		vector mins,maxs;
		GetGame().GetWorldEntity().GetWorldBounds(mins, maxs);
			
		m_iGridSizeX = maxs[0]/3;
		m_iGridSizeY = maxs[2]/3;
	 
		SCR_EditableEntityCore core = SCR_EditableEntityCore.Cast(SCR_EditableEntityCore.GetInstance(SCR_EditableEntityCore));
		vector posPlayer = Character.GetOrigin();
			
		SCR_EditableEntityComponent nearest = core.FindNearestEntity(posPlayer, EEditableEntityType.COMMENT);
		GenericEntity nearestLocation = nearest.GetOwner();
		SCR_MapDescriptorComponent mapDescr = SCR_MapDescriptorComponent.Cast(nearestLocation.FindComponent(SCR_MapDescriptorComponent));
		string closestLocationName;
		closestLocationName = nearest.GetDisplayName();

		vector lastLocationPos = nearestLocation.GetOrigin();
		float lastDistance = vector.DistanceSqXZ(lastLocationPos, posPlayer);
	
		string closeLocationAzimuth;
		vector result = posPlayer - lastLocationPos;
		result.Normalize();
		
		float angle1 = vector.DotXZ(result,vector.Forward);
		float angle2 = vector.DotXZ(result,vector.Right);
				
		if (angle2 > 0)
		{
			if (angle1 >= angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorth";
			if (angle1 < angleA && angle1 >= angleB )
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorthEast";
			if (angle1 < angleB && angle1 >=-angleB)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionEast";
			if (angle1 < -angleB && angle1 >=-angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouthEast";
			if (angle1 < -angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouth";
		}
		else
		{
			if (angle1 >= angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorth";
			if (angle1 < angleA && angle1 >= angleB )
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorthWest";
			if (angle1 < angleB && angle1 >=-angleB)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionWest";
			if (angle1 < -angleB && angle1 >=-angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouthWest";
			if (angle1 < -angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouth";
		}
		int playerGridPositionX = posPlayer[0]/m_iGridSizeX;
		int playerGridPositionY = posPlayer[2]/m_iGridSizeY;
			
		int playerGridID = GetGridIndex(playerGridPositionX,playerGridPositionY);
	 	m_sLocationName = m_WorldDirections.GetQuadHint(playerGridID) + ", " + closestLocationName;
		return m_sLocationName;
	}
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		if(!AllDirectors)
			AllDirectors = new ref array<SP_AIDirector>();
		
		AllDirectors.Insert(this);
		m_SpawnedCounter = 0;
		m_RespawnPeriod = 0;
		m_CommanderRespawnPeriod = m_CommanderRespawnTimer;
		// get first children, it should be WP
		vector position = GetOrigin();
		vector spawnMatrix[4] = { "1 0 0 0", "0 1 0 0", "0 0 1 0", "0 0 0 0" };
		spawnMatrix[3] = position;
		EntitySpawnParams spawnParams = EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform = spawnMatrix;
		Resource WP = Resource.Load(m_pDefaultWaypoint);
		Resource CWP = Resource.Load(m_pCommanderWaypoint);
		DefWaypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(WP, null, spawnParams));
		ComWaypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(CWP, null, spawnParams));
		vector mins,maxs;
		if(!GetGame().GetWorldEntity())
		{
			return;
		}
		GetGame().GetWorldEntity().GetWorldBounds(mins, maxs);
			
		m_iGridSizeX = maxs[0]/3;
		m_iGridSizeY = maxs[2]/3;
	 
		SCR_EditableEntityCore core = SCR_EditableEntityCore.Cast(SCR_EditableEntityCore.GetInstance(SCR_EditableEntityCore));
		vector posPlayer = this.GetOrigin();
			
		SCR_EditableEntityComponent nearest = core.FindNearestEntity(posPlayer, EEditableEntityType.COMMENT, EEditableEntityFlag.LOCAL);
		GenericEntity nearestLocation = nearest.GetOwner();
		SCR_MapDescriptorComponent mapDescr = SCR_MapDescriptorComponent.Cast(nearestLocation.FindComponent(SCR_MapDescriptorComponent));
		string closestLocationName;
		closestLocationName = nearest.GetDisplayName();

		vector lastLocationPos = nearestLocation.GetOrigin();
		float lastDistance = vector.DistanceSqXZ(lastLocationPos, posPlayer);
	
		string closeLocationAzimuth;
		vector result = posPlayer - lastLocationPos;
		result.Normalize();
		
		float angle1 = vector.DotXZ(result,vector.Forward);
		float angle2 = vector.DotXZ(result,vector.Right);
				
		if (angle2 > 0)
		{
			if (angle1 >= angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorth";
			if (angle1 < angleA && angle1 >= angleB )
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorthEast";
			if (angle1 < angleB && angle1 >=-angleB)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionEast";
			if (angle1 < -angleB && angle1 >=-angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouthEast";
			if (angle1 < -angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouth";
		}
		else
		{
			if (angle1 >= angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorth";
			if (angle1 < angleA && angle1 >= angleB )
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionNorthWest";
			if (angle1 < angleB && angle1 >=-angleB)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionWest";
			if (angle1 < -angleB && angle1 >=-angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouthWest";
			if (angle1 < -angleA)
				closeLocationAzimuth = "#AR-MapLocationHint_DirectionSouth";
		}
		int playerGridPositionX = posPlayer[0]/m_iGridSizeX;
		int playerGridPositionY = posPlayer[2]/m_iGridSizeY;
			
		int playerGridID = GetGridIndex(playerGridPositionX,playerGridPositionY);
	 	m_sLocationName = m_WorldDirections.GetQuadHint(playerGridID) + ", " + closestLocationName;
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);

		if (!m_SpawnAI)
			return;


		if (m_Respawn)
		{
			if (GetAgentsCount() < m_MaxAgentsToSpawn)
			{
				if (m_RespawnPeriod <= 0.0)
				{
					if (Spawn())
					{
						m_RespawnPeriod = m_RespawnTimer;
					}
				}
				else
				{
					m_RespawnPeriod -= timeSlice;
				}	
			}
		}
		if (m_CommanderRespawn)
		{
			if (GetLeaderEntity() != m_CommanderEnt)
			{
				if (m_CommanderRespawnPeriod <= 0.0)
				{
					commanderspawned = false;
					if (Spawn())
					{
						m_CommanderRespawnPeriod = m_CommanderRespawnTimer;
					}
				}
				else
					m_CommanderRespawnPeriod -= timeSlice;
			}
		}
	}
	bool SpawnDelivery(ResourceName Name)
	{
		RandomGenerator rand = new RandomGenerator();
		
		vector position = GetOrigin();
		float yOcean = GetWorld().GetOceanBaseHeight();
		
		position[1] = yOcean - 1; // force at least one iteration
		while (position[1] < yOcean)
		{
			position[0] = position[0] + rand.RandFloatXY(-m_Radius, m_Radius);
			position[2] = position[2] + rand.RandFloatXY(-m_Radius, m_Radius);
			position[1] = GetWorld().GetSurfaceY(position[0], position[2]);
		}

		vector spawnMatrix[4] = { "1 0 0 0", "0 1 0 0", "0 0 1 0", "0 0 0 0" };
		spawnMatrix[3] = position;
		EntitySpawnParams spawnParams = EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform = spawnMatrix;

		Resource res = Resource.Load(Name);
		IEntity newEnt = GetGame().SpawnEntityPrefab(res, GetWorld(), spawnParams);
		if (!newEnt)
			return false;
		
		FactionAffiliationComponent factcomp = FactionAffiliationComponent.Cast(newEnt.FindComponent(FactionAffiliationComponent));
		string faction = factcomp.GetAffiliatedFaction().GetFactionName();
		if (newEnt.GetPhysics())
			newEnt.GetPhysics().SetActive(ActiveState.ACTIVE);
			
		OnDelSpawn(newEnt);
		if (newEnt)
		{
			AIAgent agent = AIAgent.Cast(newEnt);
			if (agent)
			{
				AddAgent(agent);
				//SetNewLeader(agent);
			}
			else
			{
				AIControlComponent comp = AIControlComponent.Cast(newEnt.FindComponent(AIControlComponent));
				if (comp && comp.GetControlAIAgent())
				{
					AddAgent(comp.GetControlAIAgent());
				}
			}
		}
		return true;
	}
	bool Spawn()
	{
		RandomGenerator rand = new RandomGenerator();
		
		// randomize position in radius
		vector position = GetOrigin();
		float yOcean = GetWorld().GetOceanBaseHeight();
		
		position[1] = yOcean - 1; // force at least one iteration
		while (position[1] < yOcean)
		{
			position[0] = position[0] + rand.RandFloatXY(-m_Radius, m_Radius);
			position[2] = position[2] + rand.RandFloatXY(-m_Radius, m_Radius);
			position[1] = GetWorld().GetSurfaceY(position[0], position[2]);
		}
		// randomize entity template from array
		int randomId = rand.RandInt(0, m_FactionsToApear.Count());
		FactionManager factionm = GetGame().GetFactionManager();
		if(!m_FactionsToApear.Get(randomId))
		{
			Print("Cant find faction set in director");
			return false;
		}
		SCR_Faction randfaction = SCR_Faction.Cast(factionm.GetFactionByKey(m_FactionsToApear.Get(randomId)));
		SCR_EntityCatalog entityCatalog = randfaction.GetFactionEntityCatalogOfType(m_eEntityCatalogType);
		if(!entityCatalog)
		{
			Print("Faction has no entity catalogs set up");
			return false;
		}
		array<SCR_EntityCatalogEntry> aFactionEntityEntry = new array<SCR_EntityCatalogEntry>();
		entityCatalog.GetFullFilteredEntityListWithLabels(aFactionEntityEntry, m_aIncludedEditableEntityLabels, m_aExcludedEditableEntityLabels, m_bIncludeOnlySelectedLabels);
		int index = Math.RandomInt(0, aFactionEntityEntry.Count() - 1);
		ResourceName name = aFactionEntityEntry[index].GetPrefab();
		vector spawnMatrix[4] = { "1 0 0 0", "0 1 0 0", "0 0 1 0", "0 0 0 0" };
		spawnMatrix[3] = position;
		EntitySpawnParams spawnParams = EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform = spawnMatrix;
		vector pos = spawnParams.Transform[3];
		float surfaceY = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]);
		if (pos[1] < surfaceY)
		{
			pos[1] = surfaceY;
		}
		//Snap to the nearest navmesh point
		AIPathfindingComponent pathFindindingComponent = AIPathfindingComponent.Cast(this.FindComponent(AIPathfindingComponent));
		if (pathFindindingComponent && pathFindindingComponent.GetClosestPositionOnNavmesh(pos, "50 50 50", pos))
		{
			float groundHeight = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]);
			if (pos[1] < groundHeight)
				pos[1] = groundHeight;
		}
		
		spawnParams.Transform[3] = pos;
		Resource res;
		IEntity newEnt;
		IEntity newComEnt;
		if (commanderspawned == false && m_Commander)
		{
			res = Resource.Load(m_Commander);
			newComEnt = GetGame().SpawnEntityPrefab(res, GetWorld(), spawnParams);
			SCR_AIGroup CommandGroup = SCR_AIGroup.Cast(newComEnt);
			m_CommanderEnt = CommandGroup.GetLeaderEntity();
			FactionAffiliationComponent factcomp = FactionAffiliationComponent.Cast(m_CommanderEnt.FindComponent(FactionAffiliationComponent));
			string faction = factcomp.GetAffiliatedFaction().GetFactionName();
			commanderspawned = true;
		}
		else
		{
			res = Resource.Load(name);
			newEnt = GetGame().SpawnEntityPrefab(res, GetWorld(), spawnParams);
		}
		if (!newEnt && !newComEnt)
			return false;
		if (newEnt && newEnt.GetPhysics())
			newEnt.GetPhysics().SetActive(ActiveState.ACTIVE);
		if (newComEnt && newComEnt.GetPhysics())
			newComEnt.GetPhysics().SetActive(ActiveState.ACTIVE);

		if (newEnt)
		{
			OnSpawn(newEnt);
			AIAgent agent = AIAgent.Cast(newEnt);
			if (agent)
			{
				AddAgent(agent);
			}
			else
			{
				AIControlComponent comp = AIControlComponent.Cast(newEnt.FindComponent(AIControlComponent));
				if (comp && comp.GetControlAIAgent())
				{
					AddAgent(comp.GetControlAIAgent());
				}
			}
		}
		if (newComEnt)
		{
			OnComSpawn(newComEnt);
			AIAgent agent = AIAgent.Cast(newComEnt);
			if (agent)
			{
				AddAgent(agent);
				//SetNewLeader(agent);
			}
			else
			{
				AIControlComponent comp = AIControlComponent.Cast(newComEnt.FindComponent(AIControlComponent));
				if (comp && comp.GetControlAIAgent())
				{
					AddAgent(comp.GetControlAIAgent());
					//SetNewLeader(comp.GetControlAIAgent());
				}
			}
		}
		return true;
	}
	event void OnSpawn(IEntity spawned)
	{
		SCR_AIGroup group = SCR_AIGroup.Cast(spawned);
		if (group)
		{
			m_aGroups.Insert(group);
			if (DefWaypoint)
				group.AddWaypoint(DefWaypoint);
		}
	}
	event void OnComSpawn(IEntity spawned)
	{
		SCR_AIGroup group = SCR_AIGroup.Cast(spawned);
		if (group)
		{
			m_aGroups.Insert(group);
			if (ComWaypoint)
				group.AddWaypoint(ComWaypoint);
		}
	}
	event void OnDelSpawn(IEntity spawned)
	{
		FactionAffiliationComponent FactAf = FactionAffiliationComponent.Cast(spawned.FindComponent(FactionAffiliationComponent));
		SP_AIDirector Dir = SP_AIDirector.AllDirectors.GetRandomElement();
		Dir.GetDirectorOccupiedBy(FactAf.GetAffiliatedFaction().GetFactionKey(), Dir);
		SCR_AIGroup group = SCR_AIGroup.Cast(spawned);
		if (group)
		{
			Dir.m_aGroups.Insert(group);
			if (Dir.DefWaypoint)
				group.AddWaypoint(DefWaypoint);
		}
	}

};