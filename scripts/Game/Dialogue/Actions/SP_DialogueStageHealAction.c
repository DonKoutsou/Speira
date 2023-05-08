[BaseContainerProps(configRoot:true), DialogueStageTitleAttribute()]
class DialogueStageHealAction : DialogueStage
{
	override void Perform(IEntity Character, IEntity Player)
	{
		SCR_CharacterDamageManagerComponent CharDamageMnagr = SCR_CharacterDamageManagerComponent.Cast(Player.FindComponent(SCR_CharacterDamageManagerComponent));
		CharDamageMnagr.FullHeal();
	};
	override bool CanBePerformed(IEntity Character, IEntity Player)
	{
		SCR_CharacterDamageManagerComponent dmgmngr = SCR_CharacterDamageManagerComponent.Cast(Player.FindComponent(SCR_CharacterDamageManagerComponent));
		if (dmgmngr.CanBeHealed() == false)
		{
			SetCannotPerformReason("(Full Health)");
			return false;
		}
		return true;
	}
};