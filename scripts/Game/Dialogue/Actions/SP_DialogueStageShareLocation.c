[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageShareLocation : DialogueStage
{
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
		AIControlComponent comp = AIControlComponent.Cast(Character.FindComponent(AIControlComponent));
		AIAgent agent = comp.GetAIAgent();
		SP_AIDirector OrDirector = SP_AIDirector.Cast(agent.GetParentGroup().GetParentGroup());
	 	return DialogueText + " " + OrDirector.GetCharacterLocation(Player);
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