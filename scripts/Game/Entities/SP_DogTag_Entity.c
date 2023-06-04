class DogTagEntityClass: GameEntityClass
{

}
class DogTagEntity: GameEntity
{
	private string charname;
	
	void GetCname(out string name)
	{
		name = charname;	
	}
	override event protected void EOnInit(IEntity owner)
	{
		SetEventMask(EntityEvent.FRAME);
	}
	override event protected void EOnFrame(IEntity owner, float timeSlice)
	{
		if(!charname)
		{
			IEntity parent = owner.GetParent();
			if(parent && EntityUtils.IsPlayer(parent))
			{
				PlayerManager mngr = GetGame().GetPlayerManager();
				int pid = mngr.GetPlayerIdFromControlledEntity(parent);
				charname = mngr.GetPlayerName(pid);
				ClearEventMask(EntityEvent.FRAME);
				return;
			}
			parent = ChimeraCharacter.Cast(parent);
			if(parent)
			{
				SCR_CharacterIdentityComponent CharID = SCR_CharacterIdentityComponent.Cast(parent.FindComponent(SCR_CharacterIdentityComponent));
				SCR_CharacterRankComponent RankC = SCR_CharacterRankComponent.Cast(parent.FindComponent(SCR_CharacterRankComponent));
				FactionAffiliationComponent fact = FactionAffiliationComponent.Cast(parent.FindComponent(FactionAffiliationComponent));
				SCR_Faction faction = SCR_Faction.Cast(fact.GetAffiliatedFaction());
				SCR_ECharacterRank rank = RankC.GetCharacterRank(parent);
				charname = faction.GetRankName(rank) + " " + CharID.GetIdentity().GetFullName();
				ClearEventMask(EntityEvent.FRAME);
			}
		}
	}
}