class SP_DialogueAction : ScriptedUserAction
{
	[Attribute("")]
	private ResourceName m_pDefaultWaypoint;
	
	//[Attribute("0", UIWidgets.ComboBox, "Message type", "", ParamEnumArray.FromEnum(EOrderType_Character) )]
	//private EOrderType_Character m_OrderType;
	
	private AIWaypoint DefWaypoint;
	//------------------------------------------------------------------//
	protected SP_DialogueComponent DiagComp;
	protected SCR_BaseGameMode GameMode;
	
	protected int PrevWalkSpeed;
	protected vector PrevWalkDir;
	//------------------------------------------------------------------//
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		AIControlComponent comp = AIControlComponent.Cast(pOwnerEntity.FindComponent(AIControlComponent));
		if (!comp)
			return;
		AIAgent agent = comp.GetAIAgent();
		if (!agent)
			return;
		SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
		if (!utility)
			return;
		
		SCR_AIConverseBehavior action = new SCR_AIConverseBehavior(utility, null, pUserEntity.GetOrigin());
		
		
		//vector position[4];
		//pUserEntity.GetTransform(position);
		//EntitySpawnParams spawnParams = EntitySpawnParams();
		//spawnParams.TransformMode = ETransformMode.WORLD;
		//spawnParams.Transform = position;
		//Resource WP = Resource.Load(m_pDefaultWaypoint);
		//DefWaypoint = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(WP, null, spawnParams));
		GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		DiagComp = SP_DialogueComponent.Cast(GameMode.FindComponent(SP_DialogueComponent));
		
		AIGroup group = AIGroup.Cast(agent.GetParentGroup());
		group.AddWaypoint(DefWaypoint);
		string NoTalkText = "Cant talk to you now";
		string GreetText;
		FactionKey SenderFaction = DiagComp.GetCharacterFaction(pOwnerEntity);
		BaseChatChannel Channel;
		string name = DiagComp.GetCharacterName(pOwnerEntity);
		//CharacterControllerComponent CharCont = CharacterControllerComponent.Cast(pOwnerEntity.FindComponent(CharacterControllerComponent));
		//PrevWalkSpeed = CharCont.GetMovementSpeed();
		//CharCont.SetMovement(0, "0 0 0");
        if (group)
		{
			agent = group.GetLeaderAgent();
		}
		switch (SenderFaction)
			{
				case "FIA":
				{
					GreetText = "What do you need?";
					Channel = DiagComp.m_ChatChannelFIA;
				}
				break;
				case "USSR":
				{
					GreetText = "Papers civilian. Quick!";
					Channel = DiagComp.m_ChatChannelUSSR;
				}
				break;
				case "BANDITS":
				{
					GreetText = "Hey bro!";
					Channel = DiagComp.m_ChatChannelBANDITS;
				}
				break;
				case "SPEIRA":
				{
					Channel = DiagComp.m_ChatChannelSPEIRA;
				}
				break;
			}
		//if (Agent.HasOrders() == true)
		//{
		//	DiagComp.SendText(NoTalkText, Channel, 0, name);
		//	return;
		//}
		//Agent.GetMovementComponent().ForceFollowPathOfEntity(pUserEntity) = true;
		if (!DiagComp.LocateDialogueArchetype(pOwnerEntity, pUserEntity))
		{
			DiagComp.SendText(NoTalkText, Channel, 0, name);
			return;
		}
		utility.AddAction(action);
		MenuBase myMenu = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.DialogueMenu);
		DialogueUIClass DiagUI = DialogueUIClass.Cast(myMenu);
		DiagUI.Init(pOwnerEntity, pUserEntity);
		DiagUI.UpdateEntries(pOwnerEntity, pUserEntity);
		if (GreetText)
		{
			DiagComp.SendText(GreetText, Channel, 0, name);
		}
		//AIBaseMovementComponent movcomp = AIBaseMovementComponent.Cast(Agent.GetMovementComponent());
		//movcomp.RequestFollowPathOfEntity(pUserEntity);
	}
	override event void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
	};
	//------------------------------------------------------------------//
	override bool CanBeShownScript(IEntity user)
	{
		ChimeraCharacter char = ChimeraCharacter.Cast(GetOwner());
		if (!char)
			return false;
		SCR_CharacterDamageManagerComponent damageMan = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		if (!damageMan)
			return false;
		
		if (damageMan.GetState() == EDamageState.DESTROYED)
			return false;
		if(damageMan.GetIsUnconscious() == true)
		{
			return false;
		}
		
		return super.CanBePerformedScript(user);
	}
}