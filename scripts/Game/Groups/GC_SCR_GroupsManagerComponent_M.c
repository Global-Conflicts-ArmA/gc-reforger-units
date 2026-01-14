modded class SCR_GroupsManagerComponent
{
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

			transceiver.SetFrequency(groupComponent.GetActualGroupFrequency());
		}
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
		
		array<SCR_GadgetComponent> radios = gadgetManager.GetGadgetsByType(EGadgetType.RADIO);
		foreach (SCR_GadgetComponent gc : radios)
		{
			SCR_RadioComponent rc = SCR_RadioComponent.Cast(gc);
			if (!rc || !rc.m_bTuneToSquadFrequency)
				continue;
			
			BaseRadioComponent radioComponent = BaseRadioComponent.Cast(gc.GetOwner().FindComponent(BaseRadioComponent));
			
			if (radioComponent && m_bTuneAgentsRadioToGroupFrequency)
			{
				BaseTransceiver tsv = radioComponent.GetTransceiver(0);
				if (tsv)
					tsv.SetFrequency(group.GetRadioFrequency());
			}
		}

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
