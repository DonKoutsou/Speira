[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageJoinFactionAction : DialogueStage
{
	[Attribute()]
	FactionKey m_FactionKey;
	
	override void Perform(IEntity Character, IEntity Player)
	{
		FactionAffiliationComponent FactComp = FactionAffiliationComponent.Cast(Player.FindComponent(FactionAffiliationComponent));
		FactComp.SetAffiliatedFactionByKey(m_FactionKey);
		SCR_CharacterRankComponent Rankcomp = SCR_CharacterRankComponent.Cast(Player.FindComponent(SCR_CharacterRankComponent));
		Rankcomp.SetCharacterRank(SCR_ECharacterRank.PRIVATE);
	};
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		FactionAffiliationComponent FactComp = FactionAffiliationComponent.Cast(Player.FindComponent(FactionAffiliationComponent));
		if (FactComp.GetAffiliatedFaction().GetFactionKey() == m_FactionKey)
		{
			m_sCantBePerformedReason = "Already part of faction";
			return false;
		}
		return true;
	}

};