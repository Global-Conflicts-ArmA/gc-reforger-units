[ComponentEditorProps(category: "Game/", description: "Create a PPT on a board.")]
class BLU_PPTComponentClass : ScriptComponentClass
{
}

class BLU_PPTComponent : ScriptComponent
{
	[Attribute(params: "emat", desc: "Slides in emat format to be Displayed", category: "Slides")]
	ref array<ResourceName> SlideArray;

	[RplProp()]
	protected int m_iSlideIndex = 0;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		// Apply locally
		ApplySlideIndex(m_iSlideIndex);

		// Server sets default slide (replicated property will sync)
		if (Replication.IsServer())
		{
			m_iSlideIndex = 0;
			ApplySlideIndex(m_iSlideIndex);
		}
	}

	protected void OnRep_m_iSlideIndex()
	{
		ApplySlideIndex(m_iSlideIndex);
	}

	int GetMaxSlideValue()
	{
		if (!SlideArray || SlideArray.IsEmpty())
			return 0;

		return SlideArray.Count();
	}

	void PreviewSlideIndex(int idx)
	{
		ApplySlideIndex(idx);
	}

	void ServerSetSlideIndex(int idx)
	{
		if (!Replication.IsServer())
			return;

		m_iSlideIndex = ClampIndex(idx);
		ApplySlideIndex(m_iSlideIndex);
	}

	protected int ClampIndex(int idx)
	{
		if (!SlideArray || SlideArray.IsEmpty())
			return 0;

		return Math.Clamp(idx, 0, SlideArray.Count() - 1);
	}

	protected void ApplySlideIndex(int idx)
	{
		IEntity owner = GetOwner();
		if (!owner || !SlideArray || SlideArray.IsEmpty())
			return;

		int safeIdx = ClampIndex(idx);
		SCR_Global.SetMaterial(owner, SlideArray[safeIdx], false);
	}
}
