modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();

		// Load cfg from DB
		GetZenMapLinkChatConfig().Load();
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity) 
	{
		super.InvokeOnConnect(player, identity);

		// Sync cfg to client
		PluginZenMapLinkChatServer plugin = PluginZenMapLinkChatServer.Cast(GetPlugin(PluginZenMapLinkChatServer));
        if (identity && plugin)
        {
			plugin.PlayerLogin(identity); 
        }
	}
}