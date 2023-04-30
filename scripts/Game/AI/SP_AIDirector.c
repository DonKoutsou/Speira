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
	
	[Attribute("true")]
	bool m_Respawn;
	
	[Attribute("true")]
	bool m_SpawnAI;
	
	static bool m_SpawnCompositions = true;
	
	[Attribute("0")]
	float m_RespawnTimer;
	
	[Attribute("")]
	ref array<ResourceName> m_AgentTemplates;
	
	[Attribute("")]
	ResourceName m_Commander;
	
	bool commanderspawned = false;
	
	[Attribute("")]
	ref array<ResourceName> m_Compositions;
	
	[Attribute("")]
	ref array<ResourceName> m_CompositionsMed;
	
	[Attribute("")]
	ref array<ResourceName> m_CompositionsLarge;
	
	
	
	[Attribute("")]
	private ResourceName m_pDefaultWaypoint;	
	// private
	private int m_SpawnedCounter;	
	private float m_RespawnPeriod;
	private ref array<SCR_AIGroup> m_aGroups = new array<SCR_AIGroup>();
	private ref array<SCR_SiteSlotEntity> m_Slots = {};
	private AIWaypoint DefWaypoint;

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
		// get first children, it should be WP
		vector position = GetOrigin();
		vector spawnMatrix[4] = { "1 0 0 0", "0 1 0 0", "0 0 1 0", "0 0 0 0" };
		spawnMatrix[3] = position;
		EntitySpawnParams spawnParams = EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform = spawnMatrix;
		Resource WP = Resource.Load(m_pDefaultWaypoint);
		DefWaypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(WP, null, spawnParams));
		
		if (m_SpawnCompositions)
		{
			m_Slots.Clear();
			
			// get composition slots
			if (GetGame() && GetGame().GetWorld())
				GetGame().GetWorld().QueryEntitiesBySphere(GetOrigin(), m_Radius, HandleFoundSlot);	
					
			foreach(SCR_SiteSlotEntity slot : m_Slots)
			{
				SCR_EditableEntityComponent edit = SCR_EditableEntityComponent.GetEditableEntity(slot);
				SCR_EditableEntityUIInfo info = SCR_EditableEntityUIInfo.Cast(edit.GetInfo());
				ResourceName n;
				if (slot.GetOccupant()) continue;
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

		for (;m_SpawnedCounter < m_MaxAgentsToSpawn;)		
		{
			if (Spawn())
				m_SpawnedCounter++;
		}

		if (m_Respawn)
		{
			if (GetAgentsCount() < m_MaxAgentsToSpawn)
			{
				if (m_RespawnPeriod <= 0.0)
				{
					if (Spawn())
						m_RespawnPeriod = m_RespawnTimer;
				}
				else
					m_RespawnPeriod -= timeSlice;
			}
		}
	}
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
		
		if (newEnt.GetPhysics())
			newEnt.GetPhysics().SetActive(ActiveState.ACTIVE);
			
		OnSpawn(newEnt);

		if (newEnt)
		{
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
		if (commanderspawned == false && m_Commander)
		{
			res = Resource.Load(m_Commander);
			commanderspawned = true;
		}
		else
		{
			res = Resource.Load(name);
		}
		IEntity newEnt = GetGame().SpawnEntityPrefab(res, GetWorld(), spawnParams);
		if (!newEnt)
			return false;
		
		if (newEnt.GetPhysics())
			newEnt.GetPhysics().SetActive(ActiveState.ACTIVE);
			
		OnSpawn(newEnt);

		if (newEnt)
		{
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
	
};