[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStagePromotionAction : DialogueStage
{
	[Attribute()]
	FactionKey m_FactionKey;
	
	[Attribute()]
	string m_AnouncerText;
	
	override void Perform(IEntity Character, IEntity Player)
	{
		FactionAffiliationComponent FactComp = FactionAffiliationComponent.Cast(Player.FindComponent(FactionAffiliationComponent));
		SCR_CampaignFaction f = SCR_CampaignFaction.Cast(FactComp.GetAffiliatedFaction());
		SCR_CharacterRankComponent Rankcomp = SCR_CharacterRankComponent.Cast(Player.FindComponent(SCR_CharacterRankComponent));
		SCR_ECharacterRank m_CurrentRank = Rankcomp.GetCharacterRank(Player);
		SCR_ERadioMsg radio = SCR_ERadioMsg.NONE;
		string m_text;
		switch (m_CurrentRank)
		{
		case SCR_ECharacterRank.PRIVATE:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.CORPORAL);
				radio = SCR_ERadioMsg.PROMOTION_CORPORAL;
				m_text = m_AnouncerText + " " + "CORPORAL"
			}
			break;
		case SCR_ECharacterRank.CORPORAL:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.SERGEANT);
				radio = SCR_ERadioMsg.PROMOTION_SERGEANT;
				m_text = m_AnouncerText + " " + "SERGEANT"
			}
			break;
		case SCR_ECharacterRank.SERGEANT:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.LIEUTENANT);
				radio = SCR_ERadioMsg.PROMOTION_LIEUTENANT;
				m_text = m_AnouncerText + " " + "LIEUTENANT"
			}
			break;
		case SCR_ECharacterRank.LIEUTENANT:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.CAPTAIN);
				radio = SCR_ERadioMsg.PROMOTION_CAPTAIN;
				m_text = m_AnouncerText + " " + "CAPTAIN"
			}
			break;
		case SCR_ECharacterRank.CAPTAIN:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.MAJOR);
				radio = SCR_ERadioMsg.PROMOTION_MAJOR;
				m_text = m_AnouncerText + " " + "MAJOR"
			}
			break;
		case SCR_ECharacterRank.MAJOR:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.COLONEL);
				m_text = m_AnouncerText + " " + "COLONEL"
			}
			break;
		case SCR_ECharacterRank.COLONEL:
			{
				Rankcomp.SetCharacterRank(SCR_ECharacterRank.COMMANDER);
				m_text = m_AnouncerText + " " + "COMMANDER"
			}
			break;
		}
		SP_DialogueComponent DiagComp = SP_DialogueComponent.Cast(GetGame().GetGameMode().FindComponent(SP_DialogueComponent));
		DiagComp.DoAnouncerDialogue(m_text);
		SCR_PlayerController Cont = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		f.SendHQMessage(radio, calledID: Cont.GetPlayerId(), public: false, param: m_CurrentRank);
	}
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		return true;
	}

};