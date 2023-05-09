[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageShareLocation : DialogueStage
{
	
	[Attribute()]
    protected ref SP_DialogueDirectionHint m_WorldDirections;
	
	protected int m_iGridSizeX;
	protected int m_iGridSizeY;
	
	protected const float angleA = 0.775;
	protected const float angleB = 0.325;
	
	override void Perform(IEntity Character, IEntity Player)
	{
	};
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		return true;
	};
	protected int GetGridIndex(int x, int y)
	{
		return 3*y + x;
	}
	override string GetDialogueText(IEntity Character, IEntity Player)
	{
			vector mins,maxs;
			GetGame().GetWorldEntity().GetWorldBounds(mins, maxs);
			
			m_iGridSizeX = maxs[0]/3;
			m_iGridSizeY = maxs[2]/3;
	 
			SCR_EditableEntityCore core = SCR_EditableEntityCore.Cast(SCR_EditableEntityCore.GetInstance(SCR_EditableEntityCore));
			ChimeraCharacter player = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
			vector posPlayer = player.GetOrigin();
			
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
	 	return DialogueText + " " + m_WorldDirections.GetQuadHint(playerGridID) + " " + m_WorldDirections.GetPlayerPositionHint() + ", " + closestLocationName;
	};

};
[BaseContainerProps(configRoot: true)]
class SP_DialogueDirectionHint
{
	[Attribute(desc: "Description of given quad. Order: SW, S, SE, W, C, E, NW, N, NE", params: "MaxSize=9")]
	protected ref array<string> m_aQuadHints;
	
	[Attribute("#AR-MapLocationHint_PlayerPositionIslandName")]
	protected string m_sPlayerPositionHint;
	
	LocalizedString GetPlayerPositionHint()
	{
		return m_sPlayerPositionHint;
	}
	
	string GetQuadHint(int index)
	{
		if (!m_aQuadHints.IsIndexValid(index))
			return LocalizedString.Empty;

		return m_aQuadHints[index];
	}
};