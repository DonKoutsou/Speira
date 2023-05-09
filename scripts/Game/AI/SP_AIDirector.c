class SP_AIDirectorClass: AISpawnerGroupClass
{
};
class SP_AIDirector : AIGroup
{
	// parameters
	[Attribute("3")]
	int m_MaxAgentsToSpawn;
	
	[Attribute("100")]
	float m_Radius;
	
	[Attribute("100")]
	float m_CompositionRadius;
	
	[Attribute("true")]
	bool m_Respawn;
	
	[Attribute("false")]
	bool m_CommanderRespawn;
	
	[Attribute("true")]
	bool m_SpawnAI;
	
	static bool m_SpawnCompositions = true;
	
	[Attribute("0")]
	float m_RespawnTimer;
	
	[Attribute("0")]
	float m_CommanderRespawnTimer;
	
	[Attribute("")]
	ref array<ResourceName> m_AgentTemplates;
	
	[Attribute("")]
	ResourceName m_Commander;
	
	[Attribute("")]
	ResourceName m_ResearchGroup;
	
	bool commanderspawned = false;
	
	[Attribute("")]
	ref array<ResourceName> m_Compositions;
	
	[Attribute("")]
	ref array<ResourceName> m_CompositionsMed;
	
	[Attribute("")]
	ref array<ResourceName> m_CompositionsLarge;
	
	[Attribute("")]
	ref array<ResourceName> m_CompositionsRoad;
	
	[Attribute("")]
	string m_sLocationName;
	
	[Attribute("")]
	bool m_InstantSpawn;
	
	[Attribute("")]
	private ResourceName m_pDefaultWaypoint;
	
	[Attribute("")]
	private ResourceName m_pCommanderWaypoint;
	
	// private
	private int m_SpawnedCounter;	
	private float m_RespawnPeriod;
	private float m_CommanderRespawnPeriod;
	private ref array<SCR_AIGroup> m_aGroups = new array<SCR_AIGroup>();
	private ref array<SCR_SiteSlotEntity> m_Slots = {};
	private AIWaypoint DefWaypoint;
	private AIWaypoint ComWaypoint;
	protected IEntity m_CommanderEnt;
	protected SP_DialogueComponent DiagComp;

	void SP_AIDirector(IEntitySource src, IEntity parent)
	{
		SetFlags(EntityFlags.ACTIVE, false);	
		SetEventMask(EntityEvent.INIT);
		
		// allow AI spawning only on the server
		if (RplSession.Mode() != RplMode.Client)
			SetEventMask(EntityEvent.FRAME);
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
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
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
	}
	bool SpawnResearchGroup()
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

		Resource res = Resource.Load(m_ResearchGroup);
		IEntity newEnt = GetGame().SpawnEntityPrefab(res, GetWorld(), spawnParams);
		if (!newEnt)
			return false;
		
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
	};
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