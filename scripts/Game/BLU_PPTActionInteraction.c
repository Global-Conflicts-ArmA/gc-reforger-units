class BLU_PPTActionInteraction : ScriptedSignalUserAction
{
	[Attribute(defvalue: "1", desc: "Adjustment step")]
	protected float m_fAdjustmentStep;

	[Attribute(desc: "If action should wait for player to use their scroll wheel in order to change value")]
	protected bool m_bManualAdjustment;

	[Attribute(desc: "Determines if this action will start from the begining when max value is reached - or from the other side if Adjustment Step is below 0")]
	protected bool m_bLoopAction;

	[Attribute(defvalue: "SelectAction", desc: "Input action for increase")]
	protected string m_sActionIncrease;

	[Attribute(desc: "Input action for decrease")]
	protected string m_sActionDecrease;

	[Attribute(desc: "Action start sound event name")]
	protected string m_sActionStartSoundEvent;

	[Attribute(desc: "Action canceled sound event name")]
	protected string m_sActionCanceledSoundEvent;

	[Attribute(desc: "Movement sound event name")]
	protected string m_sMovementSoundEvent;

	[Attribute(desc: "Movement stop sound event name")]
	protected string m_sMovementStopSoundEvent;

	[Attribute(desc: "Show this action only when a player is inside a vehicle")]
	protected bool m_bOnlyInVehicle;

	[Attribute(desc: "Enable if only the Pilot/Driver should see this action. OnlyInVehicle needs to be true for this to work!")]
	protected bool m_bPilotOnly;

	protected float m_fTargetValue;
	protected bool m_bIsAdjustedByPlayer;

	protected SoundComponent m_SoundComponent;
	protected float m_fLerpLast;
	protected AudioHandle m_MovementAudioHandle;

	//------------------------------------------------------------------------------------------------
	bool IsManuallyAdjusted()
	{
		return m_bManualAdjustment;
	}

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_SoundComponent = SoundComponent.Cast(pOwnerEntity.FindComponent(SoundComponent));

		if (GetActionDuration() != 0)
			m_fAdjustmentStep /= Math.AbsFloat(GetActionDuration());

		m_fTargetValue = Math.Clamp(SCR_GetCurrentValue(), SCR_GetMinimumValue(), SCR_GetMaximumValue());
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!m_bLoopAction && !m_bManualAdjustment)
		{
			if (m_fAdjustmentStep > 0 && SCR_GetCurrentValue() >= SCR_GetMaximumValue())
				return false;

			if (m_fAdjustmentStep < 0 && SCR_GetCurrentValue() <= SCR_GetMinimumValue())
				return false;
		}

		if (!m_bOnlyInVehicle)
			return true;

		ChimeraCharacter character = ChimeraCharacter.Cast(user);
		if (!character)
			return false;

		if (!character.IsInVehicle())
			return false;

		CompartmentAccessComponent compartmentAccess = character.GetCompartmentAccessComponent();
		if (!compartmentAccess)
			return false;

		BaseCompartmentSlot slot = compartmentAccess.GetCompartment();
		if (!slot)
			return false;

		if (m_bPilotOnly)
		{
			if (!PilotCompartmentSlot.Cast(slot))
				return false;

			Vehicle vehicle = Vehicle.Cast(GetOwner().GetRootParent());
			if (vehicle && vehicle.GetPilotCompartmentSlot() != slot)
				return false;
		}

		if (m_bOnlyInVehicle && slot.GetOwner().GetRootParent() != GetOwner().GetRootParent())
			return false;

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected void ToggleActionBypass()
	{
		HandleAction(1);
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		UIInfo actionInfo = GetUIInfo();
		if (!actionInfo)
			return false;

		BLU_PPTComponent comp = BLU_PPTComponent.Cast(GetOwner().FindComponent(BLU_PPTComponent));
		int maxSlides = 1;
		if (comp)
			maxSlides = comp.GetMaxSlideValue();

		outName = "Slide [" + m_fTargetValue + "/" + maxSlides + "]";
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void PerformContinuousAction(IEntity pOwnerEntity, IEntity pUserEntity, float timeSlice)
	{
		if (!m_bManualAdjustment)
			HandleAction(timeSlice);
	}

	//------------------------------------------------------------------------------------------------
	override void OnActionStart(IEntity pUserEntity)
	{
		if (m_SoundComponent && m_sActionStartSoundEvent != string.Empty)
			m_SoundComponent.SoundEvent(m_sActionStartSoundEvent);

		m_bIsAdjustedByPlayer = (SCR_PlayerController.GetLocalControlledEntity() == pUserEntity);

		if (!m_bIsAdjustedByPlayer)
			return;

		if (m_fTargetValue < SCR_GetMinimumValue() || m_fTargetValue > SCR_GetMaximumValue())
			m_fTargetValue = Math.Clamp(SCR_GetCurrentValue(), SCR_GetMinimumValue(), SCR_GetMaximumValue());

		if (!GetActionDuration())
			ToggleActionBypass();

		if (!m_bManualAdjustment)
			return;

		if (!m_sActionIncrease.IsEmpty())
			GetGame().GetInputManager().AddActionListener(m_sActionIncrease, EActionTrigger.VALUE, HandleAction);

		if (!m_sActionDecrease.IsEmpty())
			GetGame().GetInputManager().AddActionListener(m_sActionDecrease, EActionTrigger.VALUE, HandleActionDecrease);
	}

	//------------------------------------------------------------------------------------------------
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (m_SoundComponent && m_sActionCanceledSoundEvent != string.Empty)
			m_SoundComponent.SoundEvent(m_sActionCanceledSoundEvent);

		if (!m_bIsAdjustedByPlayer)
			return;

		m_bIsAdjustedByPlayer = false;

		if (!m_bManualAdjustment)
			return;

		if (!m_sActionIncrease.IsEmpty())
			GetGame().GetInputManager().RemoveActionListener(m_sActionIncrease, EActionTrigger.VALUE, HandleAction);

		if (!m_sActionDecrease.IsEmpty())
			GetGame().GetInputManager().RemoveActionListener(m_sActionDecrease, EActionTrigger.VALUE, HandleActionDecrease);
	}

	//------------------------------------------------------------------------------------------------
	protected void HandleAction(float value)
	{
		if (value == 0)
			return;

		if (m_bManualAdjustment)
			value /= Math.AbsFloat(value);

		value *= m_fAdjustmentStep;
		m_fTargetValue += value;

		if (m_bLoopAction)
		{
			if (value > 0 && float.AlmostEqual(SCR_GetCurrentValue(), SCR_GetMaximumValue()))
				m_fTargetValue = SCR_GetMinimumValue();
			else if (value < 0 && float.AlmostEqual(SCR_GetCurrentValue(), SCR_GetMinimumValue()))
				m_fTargetValue = SCR_GetMaximumValue();
		}

		// Round to adjustment step
		m_fTargetValue = Math.Round(m_fTargetValue / m_fAdjustmentStep) * m_fAdjustmentStep;

		// Clamp to [min..max]
		m_fTargetValue = Math.Clamp(m_fTargetValue, SCR_GetMinimumValue(), SCR_GetMaximumValue());

		// Local preview for responsiveness (doesn't replicate)
		BLU_PPTComponent comp = BLU_PPTComponent.Cast(GetOwner().FindComponent(BLU_PPTComponent));
		if (comp)
			comp.PreviewSlideIndex((int)m_fTargetValue - 1); // 1..N -> 0..N-1

		// Trigger replication of action data
		if (!float.AlmostEqual(m_fTargetValue, SCR_GetCurrentValue()))
			SetSendActionDataFlag();
	}

	//------------------------------------------------------------------------------------------------
	protected void HandleActionDecrease(float value)
	{
		HandleAction(-value);
	}

	//------------------------------------------------------------------------------------------------
	protected float SCR_GetCurrentValue()
	{
		return GetCurrentValue();
	}

	protected float SCR_GetMinimumValue()
	{
		return 1;
	}

	protected float SCR_GetMaximumValue()
	{
		BLU_PPTComponent comp = BLU_PPTComponent.Cast(GetOwner().FindComponent(BLU_PPTComponent));
		int maxSlides = 1;
		if (comp)
			maxSlides = comp.GetMaxSlideValue();
		return Math.Max(1, maxSlides);
	}

	//------------------------------------------------------------------------------------------------
	protected void PlayMovementAndStopSound(float lerp)
	{
		if (!m_SoundComponent)
			return;

		if (m_fLerpLast == lerp)
			return;

		vector contextTransform[4];
		GetActiveContext().GetTransformationModel(contextTransform);

		if (float.AlmostEqual(lerp, 1))
		{
			if (!float.AlmostEqual(m_fLerpLast, 1))
			{
				m_SoundComponent.Terminate(m_MovementAudioHandle);
				if (m_sMovementStopSoundEvent != string.Empty)
					m_SoundComponent.SoundEventOffset(m_sMovementStopSoundEvent, contextTransform[3]);
			}
		}
		else if (float.AlmostEqual(lerp, 0))
		{
			if (!float.AlmostEqual(m_fLerpLast, 0))
			{
				m_SoundComponent.Terminate(m_MovementAudioHandle);
				if (m_sMovementStopSoundEvent != string.Empty)
					m_SoundComponent.SoundEventOffset(m_sMovementSoundEvent, contextTransform[3]);
			}
		}
		else
		{
			if (m_SoundComponent.IsFinishedPlaying(m_MovementAudioHandle) && m_sMovementSoundEvent != string.Empty)
				m_SoundComponent.SoundEventOffset(m_sMovementSoundEvent, contextTransform[3]);
		}

		m_fLerpLast = lerp;
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}

	override bool CanBroadcastScript()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override protected bool OnSaveActionData(ScriptBitWriter writer)
	{
		writer.WriteFloat(m_fTargetValue);

		SetSignalValue(m_fTargetValue);
		PlayMovementAndStopSound(Math.InverseLerp(SCR_GetMinimumValue(), SCR_GetMaximumValue(), m_fTargetValue));

		return true;
	}

	//------------------------------------------------------------------------------------------------
override protected bool OnLoadActionData(ScriptBitReader reader)
{
	if (m_bIsAdjustedByPlayer)
		return true;

	reader.ReadFloat(m_fTargetValue);

	SetSignalValue(m_fTargetValue);
	PlayMovementAndStopSound(Math.InverseLerp(SCR_GetMinimumValue(), SCR_GetMaximumValue(), m_fTargetValue));

	BLU_PPTComponent comp = BLU_PPTComponent.Cast(GetOwner().FindComponent(BLU_PPTComponent));
	if (comp)
	{
		int idx = (int)m_fTargetValue - 1;   // 1..N -> 0..N-1

		// IMPORTANT: make receivers actually change material
		comp.PreviewSlideIndex(idx);

		// Optional: keep authoritative state on server (for JIP, if component replication is enabled)
		if (Replication.IsServer())
			comp.ServerSetSlideIndex(idx);
	}

	return true;
}


	//------------------------------------------------------------------------------------------------
	override float GetActionProgressScript(float fProgress, float timeSlice)
	{
		if (IsManuallyAdjusted() && SCR_GetMaximumValue() - SCR_GetMinimumValue() != 0)
			return (m_fTargetValue - SCR_GetMinimumValue()) / (SCR_GetMaximumValue() - SCR_GetMinimumValue());

		return fProgress + timeSlice;
	}
}
