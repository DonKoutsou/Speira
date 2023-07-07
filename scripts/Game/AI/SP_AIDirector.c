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
	
	[Attribute("0", category: "Spawning settings")]
	float m_RespawnTimer;
	
	[Attribute("", category: "Spawning settings")]
	private ResourceName m_pDefaultWaypoint;
	
	[Attribute("1", category: "Tasks")]
	bool AllowRescue;
	
	[Attribute()]
    protected ref SCR_MapLocationQuadHint m_WorldDirections;
	
	[Attribute("1")]
    protected bool m_bVisualize;
	
	vector positiontospawn;
	ResourceName CharToSpawn;
	
	private ref array<IEntity> m_aQueriedSentinels;
	
	protected int m_iGridSizeX;
	protected int m_iGridSizeY;
	protected const float angleA = 0.775;
	protected const float angleB = 0.325;
	
	// private
	private float m_RespawnPeriod;
	private ref array<SCR_AIGroup> m_aGroups = new array<SCR_AIGroup>();
	private AIWaypoint DefWaypoint;

	bool GetDirectorOccupiedByFriendly(Faction faction, out SP_AIDirector Director)
	{
		if (m_aGroups.Count() == 0)
		{
			return false;
		}
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
				if(Directors[i] != this)
				{
					FrDirectors.Insert(Directors[i]);
					UnitCount = Count;
				}
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
		if (m_aGroups.Count() == 0)
		{
			return false;
		}
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
		if (m_aGroups.Count() == 0)
		{
			return false;
		}
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
		if (m_aGroups.Count() == 0)
		{
			return false;
		}
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
	bool GetRandomUnit(out IEntity Char)
	{
		if (m_aGroups.Count() == 0)
		{
			return false;
		}
		array<AIAgent> outAgents = new array<AIAgent>();
		SCR_AIGroup luckygroup = m_aGroups.GetRandomElement();
		if(!luckygroup)
		{
			return false;
		}
		luckygroup.GetAgents(outAgents);
		if(outAgents.Count() <= 0)
		{
			return false;
		}
		Char = outAgents.GetRandomElement().GetControlledEntity();
		if(Char)
		{
			return true;
		}
		return false;
	}
	Faction GetMajorityHolderNCount(out string factionReadable, out int UnitCount)
	{
		if (m_aGroups.Count() == 0)
		{
			return null;
		}
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
		if (m_aGroups.Count() == 0)
		{
			return null;
		}
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
	 	string m_sLocationName = m_WorldDirections.GetQuadHint(playerGridID) + ", " + closestLocationName;
		return m_sLocationName;
	}
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		if(!GetGame().GetWorldEntity())
		{
			return;
		}
		if(!m_aQueriedSentinels)
			m_aQueriedSentinels = new ref array<IEntity>();
		if(!AllDirectors)
			AllDirectors = new ref array<SP_AIDirector>();
		
		AllDirectors.Insert(this);
		
		m_RespawnPeriod = Math.RandomInt(1, 20);
		// get first children, it should be WP
		vector position = GetOrigin();
		vector spawnMatrix[4] = { "1 0 0 0", "0 1 0 0", "0 0 1 0", "0 0 0 0" };
		spawnMatrix[3] = position;
		EntitySpawnParams WPspawnParams = EntitySpawnParams();
		WPspawnParams.TransformMode = ETransformMode.WORLD;
		WPspawnParams.Transform = spawnMatrix;
		Resource WP = Resource.Load(m_pDefaultWaypoint);
		DefWaypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(WP, null, WPspawnParams));
		
		DefWaypoint.SetCompletionRadius(m_Radius);
	}
	bool CheckForCombat()
	{
		array<AIAgent> agents = new array<AIAgent>();
		m_aGroups.GetRandomElement().GetAgents(agents);
		for (int i = agents.Count() - 1; i >= 0; i--)
		{
			int count = agents[i].GetDangerEventsCount();
			if(count > 0)
			{
				return true;
			}
		}
		return false;
	}
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		if (!m_SpawnAI)
			return;
		if(!CharToSpawn)
		{
			SetCharToSpawn();
		}
		if(positiontospawn == vector.Zero)
		{
			SetSpawnPos();
		}
		if (m_Respawn)
		{
			if (GetAgentsCount() < m_MaxAgentsToSpawn)
			{
				if (m_RespawnPeriod <= 0.0)
				{
					if (Spawn())
					{
						m_RespawnPeriod = m_RespawnTimer + Math.RandomInt(m_RespawnTimer/2, m_RespawnTimer + m_RespawnTimer/2);
					}
					
				}
				else
				{
					m_RespawnPeriod -= timeSlice;
				}	
			}
		}
	}
	bool Spawn()
	{	
		if(!CharToSpawn)
		{
			SetCharToSpawn();
		}
		if(positiontospawn == vector.Zero)
		{
			SetSpawnPos();
		}
		EntitySpawnParams spawnParams = EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform[3] = positiontospawn;	
		Resource res;
		IEntity newEnt;
		res = Resource.Load(CharToSpawn);
		newEnt = GetGame().SpawnEntityPrefab(res, GetWorld(), spawnParams);
		if (!newEnt)
			return false;
		if (newEnt.GetPhysics())
			newEnt.GetPhysics().SetActive(ActiveState.ACTIVE);
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
		positiontospawn = vector.Zero;
		CharToSpawn = STRING_EMPTY;
		return true;
	}
	void SetCharToSpawn()
	{
		// randomize entity template from array
		FactionManager factionManager = GetGame().GetFactionManager();
		SCR_Faction randfaction = SCR_Faction.Cast(factionManager.GetFactionByKey(m_FactionsToApear.GetRandomElement()));
		SCR_EntityCatalog entityCatalog = randfaction.GetFactionEntityCatalogOfType(m_eEntityCatalogType);
		array<SCR_EntityCatalogEntry> aFactionEntityEntry = new array<SCR_EntityCatalogEntry>();
		entityCatalog.GetFullFilteredEntityListWithLabels(aFactionEntityEntry, m_aIncludedEditableEntityLabels, m_aExcludedEditableEntityLabels, m_bIncludeOnlySelectedLabels);
		CharToSpawn = aFactionEntityEntry.GetRandomElement().GetPrefab();
	}
	void SetSpawnPos()
	{
		// randomize position in radius
		vector position = GetOrigin();
		float yOcean = GetWorld().GetOceanBaseHeight();
		RandomGenerator rand = new RandomGenerator();
		position[1] = yOcean - 1; // force at least one iteration
		position[0] = position[0] + rand.RandFloatXY(-m_Radius, m_Radius);
		position[2] = position[2] + rand.RandFloatXY(-m_Radius, m_Radius);
		position[1] = GetWorld().GetSurfaceY(position[0], position[2]);
		vector spawnMatrix[4] = { "1 0 0 0", "0 1 0 0", "0 0 1 0", "0 0 0 0" };
		spawnMatrix[3] = position;
		EntitySpawnParams spawnParams = EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform = spawnMatrix;
		positiontospawn = spawnParams.Transform[3];
		float surfaceY = GetGame().GetWorld().GetSurfaceY(positiontospawn[0], positiontospawn[2]);
		if (positiontospawn[1] < surfaceY)
		{
			positiontospawn[1] = surfaceY;
		}
		//Snap to the nearest navmesh point
		AIPathfindingComponent pathFindindingComponent = AIPathfindingComponent.Cast(this.FindComponent(AIPathfindingComponent));
		if (pathFindindingComponent && pathFindindingComponent.GetClosestPositionOnNavmesh(positiontospawn, "50 50 50", positiontospawn))
		{
			float groundHeight = GetGame().GetWorld().GetSurfaceY(positiontospawn[0], positiontospawn[2]);
			if (positiontospawn[1] < groundHeight)
				positiontospawn[1] = groundHeight;
		}
	}
	bool CreateVictim(out IEntity Victim)
	{
		if (!AllowRescue)
		{
			return false;
		}
		if (m_aGroups.Count() == 0)
		{
			return false;
		}
		array<AIAgent> outAgents = new array<AIAgent>();
		SCR_AIGroup luckygroup = m_aGroups.GetRandomElement();
		
		if(!luckygroup)
		{
			return false;
		}
		luckygroup.GetAgents(outAgents);
		if(outAgents.Count() <= 0)
		{
			return false;
		}
		
		Victim = luckygroup.GetLeaderEntity();
		if(!Victim)
		{
			return false;
		}
		foreach(AIAgent agent : outAgents)
		{
			IEntity Char = agent.GetControlledEntity();
			if(Char)
			{
				SCR_CharacterDamageManagerComponent dmg = SCR_CharacterDamageManagerComponent.Cast(Char.FindComponent(SCR_CharacterDamageManagerComponent));
				if(dmg.GetIsUnconscious())
				{
					return false;
				}
				dmg.ForceUnconsciousness();
				dmg.SetPermitUnconsciousness(false, true);
				dmg.AddParticularBleeding();
			}
		}
		return true;
	};
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
	/*override void _WB_SetExtraVisualiser(EntityVisualizerType type, IEntitySource src)
	{		
		m_bVisualize = false;
		switch (type)
		{
			case EntityVisualizerType.EVT_NONE:
				return;
			
			case EntityVisualizerType.EVT_NORMAL:
				return;
		}
		
		m_bVisualize = true;		
		super._WB_SetExtraVisualiser(type, src);
	}
	*/
	private bool QueryEntities(IEntity e)
	{
		SCR_AISmartActionSentinelComponent sentinel = SCR_AISmartActionSentinelComponent.Cast(e.FindComponent(SCR_AISmartActionSentinelComponent));
		if (sentinel)
			m_aQueriedSentinels.Insert(e);
		
		return true;
	}
	private void GetSentinels(float radius)
	{
		BaseWorld world = GetWorld();
		world.QueryEntitiesBySphere(GetOrigin(), radius, QueryEntities);
	}
	private void _CaptureSentinels()
	{
		m_aQueriedSentinels = {};
		GetSentinels(m_Radius);
	}
	override bool _WB_OnKeyChanged(BaseContainer src, string key, BaseContainerList ownerContainers, IEntity parent) 
	{
		_CaptureSentinels();
		return super._WB_OnKeyChanged(src, key, ownerContainers, parent);
	}
	
	override void _WB_SetExtraVisualiser(EntityVisualizerType type, IEntitySource src)
	{	
		m_bVisualize = true;			
		_CaptureSentinels();
		super._WB_SetExtraVisualiser(type, src);
	}
	override void _WB_AfterWorldUpdate(float timeSlice)
	{
		if (m_bVisualize)
		{
			string factionstospawn;
			foreach(FactionKey disfact: m_FactionsToApear)
			{
				factionstospawn = factionstospawn +  "|" + disfact + "| ";
			}
			string infoText2 = string.Format("Max Agents to Spawn: %1 ", m_MaxAgentsToSpawn.ToString());
			auto origin = GetOrigin();
			auto radiusShape = Shape.CreateSphere(COLOR_BLUE, ShapeFlags.WIREFRAME | ShapeFlags.ONCE, origin, m_Radius);	
			DebugTextWorldSpace.Create(GetGame().GetWorld(), factionstospawn, DebugTextFlags.CENTER | DebugTextFlags.FACE_CAMERA | DebugTextFlags.ONCE, origin[0], origin[1] + m_Radius +10, origin[2], 10, 0xFFFFFFFF, Color.BLACK);
			DebugTextWorldSpace.Create(GetGame().GetWorld(), infoText2, DebugTextFlags.CENTER | DebugTextFlags.FACE_CAMERA | DebugTextFlags.ONCE, origin[0], origin[1] + m_Radius, origin[2], 10, 0xFFFFFFFF, Color.BLACK);
			if(m_aQueriedSentinels)
			{
				foreach (IEntity entity : m_aQueriedSentinels)
				{
					vector entorigin = entity.GetOrigin();
					Shape.CreateSphere(Color.PINK, ShapeFlags.WIREFRAME | ShapeFlags.ONCE, entorigin, 5);
					array<Managed> outComponents = new array<Managed>();
					entity.FindComponents(SCR_AISmartActionSentinelComponent, outComponents);
					foreach(Managed smart : outComponents)
					{
						string tags;
						SCR_AISmartActionSentinelComponent sent = SCR_AISmartActionSentinelComponent.Cast(smart);
						array<string> outTagstemp = new array<string>();
						sent.GetTags(outTagstemp);
						foreach(string tag : outTagstemp)
						{
							tags = tags +  "|" + tag + "| ";
						}
						vector Smartloc = entorigin + sent.GetActionOffset();
						Shape.CreateSphere(Color.PINK, ShapeFlags.DEFAULT | ShapeFlags.ONCE, Smartloc, 1);
						DebugTextWorldSpace.Create(GetGame().GetWorld(), tags, DebugTextFlags.CENTER | DebugTextFlags.FACE_CAMERA | DebugTextFlags.ONCE, Smartloc[0], Smartloc[1] + 1, Smartloc[2], 10, 0xFFFFFFFF, Color.BLACK);
						tags = STRING_EMPTY;
					}
					string SmartText = string.Format("%1: seats", outComponents.Count().ToString());
					DebugTextWorldSpace.Create(GetGame().GetWorld(), SmartText, DebugTextFlags.CENTER | DebugTextFlags.FACE_CAMERA | DebugTextFlags.ONCE, entorigin[0], entorigin[1] + 5, entorigin[2], 10, 0xFFFFFFFF, Color.BLACK);
					
				}
			}	
		}
		
		super._WB_AfterWorldUpdate(timeSlice);
	}
};