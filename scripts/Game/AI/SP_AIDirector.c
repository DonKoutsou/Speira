class SP_AIDirectorClass: AIGroupClass
{
};
class SP_AIDirector : AIGroup
{
	
	static ref array<SP_AIDirector> AllDirectors = null; // new ref array<SP_AIDirector>();
	// parameters
	[Attribute("3")]
	int m_MaxAgentsToSpawn;
	
	[Attribute("100")]
	float m_Radius;
	
	[Attribute("100")]
	float m_CompositionRadius;
	
	[Attribute("true")]
	bool m_Respawn;
	
	[Attribute("true")]
	bool m_RenegadeRespawn;
	
	[Attribute("false")]
	bool m_CommanderRespawn;
	
	[Attribute("true")]
	bool m_SpawnAI;
	
	static bool m_SpawnCompositions = true;
	
	[Attribute("0")]
	float m_RespawnTimer;
	
	[Attribute("0")]
	float m_CommanderRespawnTimer;
	
	[Attribute("0")]
	float m_RenegadeRespawnTimer;
	
	[Attribute("")]
	ref array<ResourceName> m_AgentTemplates;
	
	[Attribute("")]
	ResourceName m_Commander;
	
	[Attribute("")]
	ResourceName m_Renegade;
	
	bool commanderspawned = false;
	
	[Attribute("")]
	ref array<ResourceName> m_Compositions;
	
	[Attribute("")]
	ref array<ResourceName> m_CompositionsMed;
	
	[Attribute("")]
	ref array<ResourceName> m_CompositionsLarge;
	
	[Attribute("")]
	ref array<ResourceName> m_CompositionsRoad;
	
	string m_sLocationName;
	
	[Attribute("")]
	bool m_InstantSpawn;
	
	[Attribute("")]
	private ResourceName m_pDefaultWaypoint;
	
	[Attribute("")]
	private ResourceName m_pCommanderWaypoint;
	
	[Attribute()]
    protected ref SCR_MapLocationQuadHint m_WorldDirections;

	protected int m_iGridSizeX;
	protected int m_iGridSizeY;
	
	protected const float angleA = 0.775;
	protected const float angleB = 0.325;
	
	string GetLocationName ()
	{
		return m_sLocationName;
	}
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
	bool GetDirectorOccupiedBy(FactionKey Key, out SP_AIDirector Director)
	{
		ref array<SP_AIDirector> Directors = SP_AIDirector.AllDirectors;
		int UnitCount;
		for (int i = 0; i < Directors.Count(); i++)
		{
			FactionKey FKey;
			int Count;
			FactionKey Key2 = Directors[i].GetMajorityHolderNCount(FKey, Count);
			if (Key2 == Key && Count > UnitCount && Directors[i] != this)
			{
				Director = Directors[i];
				UnitCount = Count;
			}
		}
		if(Director)
		{
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
	FactionKey GetMajorityHolderNCount(out string factionReadable, out int UnitCount)
	{
		int USSRcount;
		int UScount;
		int FIAcount;
		int Banditcount;
		int Renegcount;
		for (int i = m_aGroups.Count() - 1; i >= 0; i--)
		{
			string faction = m_aGroups[i].GetFaction().GetFactionKey();
			switch(faction)
			{
				case "USSR":
				{
					USSRcount = USSRcount + m_aGroups[i].GetAgentsCount();
				}
				break;
				case "US":
				{
					UScount = UScount + m_aGroups[i].GetAgentsCount();
				}
				break;
				case "FIA":
				{
					FIAcount = FIAcount + m_aGroups[i].GetAgentsCount();
				}
				break;
				case "BANDITS":
				{
					Banditcount = Banditcount + m_aGroups[i].GetAgentsCount();
				}
				break;
				case "RENEGADE":
				{
					Renegcount = Renegcount + m_aGroups[i].GetAgentsCount();
				}
				break;
			}
		}
		int max = USSRcount;
    	string MajorFaction = "USSR";
		factionReadable = "soviet";
	    if (UScount > max)
	    {
	        max = UScount;
			MajorFaction = "US";
			factionReadable = "US";
	    }
	    
	    if (FIAcount > max)
	    {
	        max = FIAcount;
			MajorFaction = "FIA";
			factionReadable = "guerrilla";
	    }
	    
	    if (Banditcount > max)
	    {
	        max = Banditcount;
			MajorFaction = "BANDITS";
			factionReadable = "bandit";
	    }
		if (Renegcount > max)
	    {
	        max = Renegcount;
			MajorFaction = "RENEGADES";
			factionReadable = "renegade";
	    }
		UnitCount = max;
	    return MajorFaction; 	
	}
	Faction GetMajorityHolder(out string factionReadable)
	{
		int USSRcount;
		int UScount;
		int FIAcount;
		int Banditcount;
		int Renegcount;
		for (int i = m_aGroups.Count() - 1; i >= 0; i--)
		{
			string faction = m_aGroups[i].GetFaction().GetFactionKey();
			switch(faction)
			{
				case "USSR":
				{
					USSRcount = USSRcount + m_aGroups[i].GetAgentsCount();
				}
				break;
				case "US":
				{
					UScount = UScount + m_aGroups[i].GetAgentsCount();
				}
				break;
				case "FIA":
				{
					FIAcount = FIAcount + m_aGroups[i].GetAgentsCount();
				}
				break;
				case "BANDITS":
				{
					Banditcount = Banditcount + m_aGroups[i].GetAgentsCount();
				}
				break;
				case "RENEGADE":
				{
					Renegcount = Renegcount + m_aGroups[i].GetAgentsCount();
				}
				break;
			}
		}
		int max = USSRcount;
		FactionManager FMan = FactionManager.Cast(GetGame().GetFactionManager());
    	Faction MajorFaction = FMan.GetFactionByKey("USSR");
		factionReadable = "soviet";
	    if (UScount > max)
	    {
	        max = UScount;
			MajorFaction = FMan.GetFactionByKey("US");
			factionReadable= "US";
	    }
	    
	    if (FIAcount > max)
	    {
	        max = FIAcount;
			MajorFaction = FMan.GetFactionByKey("FIA");
			factionReadable = "guerrilla";
	    }
	    
	    if (Banditcount > max)
	    {
	        max = Banditcount;
			MajorFaction = FMan.GetFactionByKey("BANDITS");
			factionReadable = "bandit";
	    }
		if (Renegcount > max)
	    {
	        max = Renegcount;
			MajorFaction = FMan.GetFactionByKey("RENEGADES");
			factionReadable = "renegade";
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
	
	ResourceName GetRandomComposition()
	{
		int i = Math.RandomInt(0, m_Compositions.Count());
		return m_Compositions[i];
	}
	ResourceName GetRandomCompositionMedium()
	{
		int i = Math.RandomInt(0, m_CompositionsMed.Count());
		return m_CompositionsMed[i];
	}
	ResourceName GetRandomCompositionLarge()
	{
		int i = Math.RandomInt(0, m_CompositionsLarge.Count());
		return m_CompositionsLarge[i];
	}
	ResourceName GetRandomCompositionRoad()
	{
		int i = Math.RandomInt(0, m_CompositionsRoad.Count());
		return m_CompositionsRoad[i];
	}
	
	bool HandleFoundSlot(IEntity e)
	{
		SCR_SiteSlotEntity slot = SCR_SiteSlotEntity.Cast(e);
		if (slot)
		{
			m_Slots.Insert(slot);
		}
		
		return true;
	}	
	protected int GetGridIndex(int x, int y)
	{
		return 3*y + x;
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
		m_RenegadeRespawnPeriod = m_RenegadeRespawnTimer;
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
		
		
		if (m_SpawnCompositions)
		{
			m_Slots.Clear();
			
			// get composition slots
			if (GetGame() && GetGame().GetWorld())
				GetGame().GetWorld().QueryEntitiesBySphere(GetOrigin(), m_CompositionRadius, HandleFoundSlot);	
					
			foreach(SCR_SiteSlotEntity slot : m_Slots)
			{
				SCR_EditableEntityComponent edit = SCR_EditableEntityComponent.GetEditableEntity(slot);
				SCR_EditableEntityUIInfo info = SCR_EditableEntityUIInfo.Cast(edit.GetInfo());
				ResourceName n;
				if (slot.GetOccupant()) continue;
				if (m_CompositionsRoad.Count() > 0)
				{
					if(info.HasEntityLabel(EEditableEntityLabel.SLOT_ROAD_LARGE) == true)
					{
						n = GetRandomCompositionRoad();
					}
					if(info.HasEntityLabel(EEditableEntityLabel.SLOT_ROAD_MEDIUM) == true)
					{
						n = GetRandomCompositionRoad();
					}
					if(info.HasEntityLabel(EEditableEntityLabel.SLOT_ROAD_SMALL) == true)
					{
						n = GetRandomCompositionRoad();
					}
				}
				if(info.HasEntityLabel(EEditableEntityLabel.SLOT_FLAT_LARGE) == true)
				{
					n = GetRandomCompositionLarge();
				}
				else if (info.HasEntityLabel(EEditableEntityLabel.SLOT_FLAT_MEDIUM) == true)
				{
					n = GetRandomCompositionMedium();
				}
				else
				{
					n = GetRandomComposition();
				}
				
				Resource a = Resource.Load(n);
				slot.SpawnEntityInSlot(a);
				slot.GetScale();
			}
		}
		//vector posPlayer = this.GetOrigin();
		//SCR_EditableEntityCore core = SCR_EditableEntityCore.Cast(SCR_EditableEntityCore.GetInstance(SCR_EditableEntityCore));
	    //   if (!core)
	    //       return;
		//SCR_EditableEntityComponent nearest = core.FindNearestEntity(posPlayer, EEditableEntityType.COMMENT, EEditableEntityFlag.LOCAL);
		//if (!nearest)
		//	return;
		//GenericEntity nearestLocation = nearest.GetOwner();
		//SCR_MapDescriptorComponent mapDescr = SCR_MapDescriptorComponent.Cast(nearestLocation.FindComponent(SCR_MapDescriptorComponent));
		//string closestLocationName;
		//if (!mapDescr)
		//	return;
		//MapItem item = mapDescr.Item();
		//m_sLocationName = item.GetDisplayName();
		//////////////////////////////////////
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
		MapItem item = mapDescr.Item();
		closestLocationName = item.GetDisplayName();

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
		
		if (m_AgentTemplates.Count() == 0 || !m_SpawnAI)
			return;


		if (m_Respawn)
		{
			if (GetAgentsCount() < m_MaxAgentsToSpawn)
			{
				if (m_InstantSpawn == true)
				{
					if (Spawn())
					{
						m_InstantSpawn = false;
					}
				}
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
		if (m_RenegadeRespawn && GetAgentsCount() >= m_MaxAgentsToSpawn)
		{
			if (m_RenegadeRespawnPeriod <= 0.0)
			{
				commanderspawned = false;
				if (SpawnRenegade(m_Renegade))
				{
					m_RenegadeRespawnPeriod = m_RenegadeRespawnTimer;
				}
			}
			else
			{
				m_RenegadeRespawnPeriod -= timeSlice;
			}
		}
	}
	bool SpawnRenegade(ResourceName name)
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
		int randomId = rand.RandInt(0, m_AgentTemplates.Count());
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
		if (pathFindindingComponent && pathFindindingComponent.GetClosestPositionOnNavmesh(pos, "10 10 10", pos))
		{
			float groundHeight = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]);
			if (pos[1] < groundHeight)
				pos[1] = groundHeight;
		}
		
		spawnParams.Transform[3] = pos;
		Resource res;
		IEntity newEnt;
		res = Resource.Load(name);
		newEnt = GetGame().SpawnEntityPrefab(res, GetWorld(), spawnParams);
		if (!newEnt)
			return false;
		if (newEnt && newEnt.GetPhysics())
			newEnt.GetPhysics().SetActive(ActiveState.ACTIVE);
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
		return true;
	};
	bool SpawnCommander(ResourceName Name)
	{
		if (m_AgentTemplates.Count() == 0)
			return false;
		
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
		//DiagComp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		//DiagComp.DoAnouncerDialogue(faction + " " + "Commander squad has been deployed on location" + " " + m_sLocationName);
		if (newEnt.GetPhysics())
			newEnt.GetPhysics().SetActive(ActiveState.ACTIVE);
			
		OnSpawn(newEnt);
		SCR_AIGroup CommandGroup = SCR_AIGroup.Cast(newEnt);
		m_CommanderEnt = CommandGroup.GetMaster();
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
		if (m_AgentTemplates.Count() == 0)
			return false;
		
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
		int randomId = rand.RandInt(0, m_AgentTemplates.Count());
		ResourceName name = m_AgentTemplates.Get(randomId);

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
		if (pathFindindingComponent && pathFindindingComponent.GetClosestPositionOnNavmesh(pos, "10 10 10", pos))
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
			//DiagComp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
			//DiagComp.DoAnouncerDialogue(faction + " " + "Commander squad has been deployed on location" + " " + m_sLocationName);
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

};