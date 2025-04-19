[ComponentEditorProps(category: "Game/", description: "Create a PPT on a board.")]
class BLU_PPTComponentClass : ScriptComponentClass
{
}

class BLU_PPTComponent : ScriptComponent 
{
	[Attribute(params:"emat",desc: "Slides in emat format to be Displayed", category: "Slides")]
	ref array<ResourceName> SlideArray;
	int m_MaxSlideValue = SlideArray.Count(); 
	
	
	override void OnPostInit(IEntity owner) 
	{	
		SCR_Global.SetMaterial(owner, SlideArray[0],false);
	}
}