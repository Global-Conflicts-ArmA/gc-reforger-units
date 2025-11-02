/*
[ComponentEditorProps(category: "GameScripted/GameMode/Components", description: "")]
class GC_ExplosivesManagerClass : SCR_ExplosiveTriggerComponentClass
{
}

class GC_ExplosivesManager : SCR_ExplosiveTriggerComponent
{
	protected bool m_isArmed = false;
	protected bool m_isExploded = false;
	
	void Explode()
	{
		SCR_ExplosiveTriggerComponent trigger = SCR_ExplosiveTriggerComponent.Cast(GetOwner().FindComponent(SCR_ExplosiveTriggerComponent));
		if(!trigger)
			return;
		
		UseTrigger();
		m_isExploded = true;
	}
	
	void SetDeadmans(bool state, IEntity user)
	{
		if(state == m_isArmed)
			return;
		
		ChimeraCharacter character = ChimeraCharacter.Cast(user);
		if (!character)
			return;
		
		SCR_CompartmentAccessComponent compAccess = SCR_CompartmentAccessComponent.Cast(character.GetCompartmentAccessComponent());
		if(!compAccess)
			return;
		
		SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
		if (!controller)
			return;
		
		if(state && Replication.IsServer())
		{
			if(Replication.IsServer())
				controller.m_OnNewLifeState.Insert(OnNewLifeState);
			compAccess.GetOnPlayerCompartmentExit().Insert(OnCompartmentExit);
		}else{
			if(Replication.IsServer())
				controller.m_OnNewLifeState.Remove(OnNewLifeState);
			compAccess.GetOnPlayerCompartmentExit().Remove(OnCompartmentExit);
		}
		
		m_isArmed = state;
	}
	
	bool CanActivateDeadmans(IEntity user)
	{
		if(m_isExploded)
			return false;
		
		if(!IsInVehicle(user))
			return false;
		
		return true;
	}
	
	bool IsInVehicle(IEntity user = null)
	{
		if(!user)
			user = GetGame().GetPlayerController().GetControlledEntity();

		ChimeraCharacter character = ChimeraCharacter.Cast(user);
		if(!character)
			return false;
		
		CompartmentAccessComponent compAccess = character.GetCompartmentAccessComponent();
		if(!compAccess)
			return false;

		if(compAccess.IsGettingIn() || compAccess.IsGettingOut())
			return false;
		
		if(character.IsInVehicle())
			return true;
		
		return false;
	}
	
	void OnCompartmentExit(IEntity character, IEntity targetEntity)
	{
		SetDeadmans(false, character);
	}
	
	void OnNewLifeState(ECharacterLifeState state, SCR_CharacterControllerComponent controller)
	{
		if(state == ECharacterLifeState.ALIVE)
			return;
		
		Explode();
		SetDeadmans(false, controller.GetOwner());
	}
	
	bool IsArmed()
	{
		return m_isArmed;
	}
}

class GC_ActivateDeadmans : ScriptedUserAction
{
	GC_ExplosivesManager exp;
	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		exp = GC_ExplosivesManager.Cast(GetOwner().FindComponent(GC_ExplosivesManager))
	}
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if(exp.CanActivateDeadmans(pUserEntity))
			exp.SetDeadmans(!exp.IsArmed(), pUserEntity);
	}
	
	override bool CanBeShownScript(IEntity user)
	{
		return exp.CanActivateDeadmans(user);
	}
	
	override bool GetActionNameScript(out string outName)
	{
		if(exp.IsArmed())
			outName = "Deactivate Deadmans Switch";
		else
			outName = "Activate Deadmans Switch";

		return true;
	}
}

class GC_ActivateExplosion : ScriptedUserAction
{
	GC_ExplosivesManager exp;
	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		exp = GC_ExplosivesManager.Cast(GetOwner().FindComponent(GC_ExplosivesManager))
	}
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if(exp.CanActivateDeadmans(pUserEntity))
			exp.Explode()
	}
	
	override bool CanBeShownScript(IEntity user)
	{
		return exp.CanActivateDeadmans(user);
	}
}

typedef func OnNewLifeState;
typedef ScriptInvokerBase<OnNewLifeState> OnNewLifeStateInvoker;

modded class SCR_CharacterControllerComponent
{
	ref OnNewLifeStateInvoker m_OnNewLifeState = new OnNewLifeStateInvoker();
	
	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		m_OnNewLifeState.Invoke(newLifeState, this);
		
		super.OnLifeStateChanged(previousLifeState, newLifeState);
	}
}
*/