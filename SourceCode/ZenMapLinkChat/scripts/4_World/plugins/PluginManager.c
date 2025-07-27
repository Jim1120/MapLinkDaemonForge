modded class PluginManager
{
	override void Init()
	{
		super.Init();

		RegisterPlugin("PluginZenMapLinkChatServer", false, true);
		RegisterPlugin("PluginZenMapLinkChatClient", true, false);
	}
}