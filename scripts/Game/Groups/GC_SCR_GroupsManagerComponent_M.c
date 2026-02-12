modded class SCR_GroupsManagerComponent
{
	// Set radio encryption keys at runtime when the player or AI is first spawned
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		GetGameMode().GetOnPlayerSpawned().Insert(GCC_SetEncryptionKey);
	}
	
	override void OnGroupAgentAdded(AIAgent child)
	{
		super.OnGroupAgentAdded(child);
		GetGame().GetCallqueue().CallLater(GCC_SetEncryptionKey, 1, false, -1, child.GetControlledEntity());
	}
	
	protected void GCC_SetEncryptionKey(int playerId, IEntity player)
	{
		SCR_ChimeraCharacter cc = SCR_ChimeraCharacter.Cast(player);
		if (!cc)
			return;
		
		Faction f = cc.GetFaction();
		if (!f)
			return;
		
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(player);
		if (!gadgetManager)
			return;

		array<SCR_GadgetComponent> radios = gadgetManager.GetGadgetsByType(EGadgetType.RADIO);
		
		foreach (SCR_GadgetComponent r : radios)
		{
			BaseRadioComponent radioComponent = BaseRadioComponent.Cast(r.GetOwner().FindComponent(BaseRadioComponent));
			if (!radioComponent)
				continue;
			
			radioComponent.SetEncryptionKey(f.GetFactionRadioEncryptionKey());
		}
	}
	
	// Adjustments to vanilla radio tuning
	
	protected void GCC_TuneRadios(SCR_GadgetManagerComponent gadgetManager, int frequency)
	{
		array<SCR_GadgetComponent> radios = gadgetManager.GetGadgetsByType(EGadgetType.RADIO);
		
		foreach (SCR_GadgetComponent gc : radios)
		{
			SCR_RadioComponent rc = SCR_RadioComponent.Cast(gc);
			if (!rc || !rc.m_bTuneToSquadFrequency)
				continue;
			
			BaseRadioComponent radioComponent = BaseRadioComponent.Cast(gc.GetOwner().FindComponent(BaseRadioComponent));
			if (!radioComponent)
				continue;

			BaseTransceiver transceiver = radioComponent.GetTransceiver(0);
			if (!transceiver)
				continue;

			transceiver.SetFrequency(frequency);
		}
	}
	
	override void TunePlayersFrequency(int playerId, IEntity player)
	{
		PlayerController controller = GetGame().GetPlayerManager().GetPlayerController(playerId);
		if (!controller)
			return;

		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(player);
		if (!gadgetManager)
			return;
		
		SCR_PlayerControllerGroupComponent groupComponent = SCR_PlayerControllerGroupComponent.Cast(controller.FindComponent(SCR_PlayerControllerGroupComponent));
		if (!groupComponent || groupComponent.GetActualGroupFrequency() == 0)
			return;
		
		GCC_TuneRadios(gadgetManager, groupComponent.GetActualGroupFrequency());
	}
	
	override private void TuneAgentsRadio(AIAgent agentEntity)
	{
		if (!agentEntity)
			return;

		SCR_AIGroup group = SCR_AIGroup.Cast(agentEntity.GetParentGroup());
		if (!group)
			return;
		
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.Cast(agentEntity.GetControlledEntity().FindComponent(SCR_GadgetManagerComponent));
		if (!gadgetManager)
			return;
		
		GCC_TuneRadios(gadgetManager, group.GetRadioFrequency());

		// Set radio active channel to group default radio channel
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(agentEntity.GetControlledEntity());
		if (playerId <= 0)
			return;

		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!pc)
			return;

		SCR_VONController vonController = SCR_VONController.Cast(pc.FindComponent(SCR_VONController));
		if (!vonController)
			return;

		vonController.SetActiveChannel(group.GetDefaultActiveRadioChannel());
	}
}