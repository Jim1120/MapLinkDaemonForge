modded class MissionBase
{
	void MissionBase()
	{
		#ifdef SERVER
        GetRPCManager().AddRPC("ZenMod_RPC", "RPC_ReceiveZenMapLinkChatOnServer", this, SingeplayerExecutionType.Server);
        GetRPCManager().AddRPC("ZenMod_RPC", "RPC_ReceiveZenMapLinkChatBanRequested", this, SingeplayerExecutionType.Server);
        #else
        GetRPCManager().AddRPC("ZenMod_RPC", "RPC_ReceiveZenMapLinkChatOnClient", this, SingeplayerExecutionType.Client);
		GetRPCManager().AddRPC("ZenMod_RPC", "RPC_ReceiveZenMapLinkChatConfig", this, SingeplayerExecutionType.Client);
        GetRPCManager().AddRPC("ZenMod_RPC", "RPC_ReceiveZenMapLinkChatBanOnClient", this, SingeplayerExecutionType.Client);
        GetRPCManager().AddRPC("ZenMod_RPC", "RPC_ReceiveZenMapLinkChatBanlist", this, SingeplayerExecutionType.Client);
        #endif
	}

    void RPC_ReceiveZenMapLinkChatBanRequested(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server)
            return;

        Param2<string, bool> data;
        if (!ctx.Read(data))
        {
            Error("Error receiving data - RPC_ReceiveZenMapLinkChatBanRequested");
            return;
        }

        PluginZenMapLinkChatServer plugin = PluginZenMapLinkChatServer.Cast(GetPlugin(PluginZenMapLinkChatServer));
        if (plugin)
        {
            plugin.BanPlayer(sender, data.param1, data.param2);
        }
    }

    void RPC_ReceiveZenMapLinkChatOnServer(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server)
            return;

        Param1<string> data;
        if (!ctx.Read(data))
        {
            Error("Error receiving data - RPC_ReceiveZenMapLinkChatOnServer");
            return;
        }

        PluginZenMapLinkChatServer plugin = PluginZenMapLinkChatServer.Cast(GetPlugin(PluginZenMapLinkChatServer));
        if (plugin)
        {
            plugin.AddChatMessage(sender, data.param1);
        }
    }

    void RPC_ReceiveZenMapLinkChatOnClient(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Client)
            return;

        Param1<array<string>> data;
        if (!ctx.Read(data))
        {
            Error("Error receiving data - RPC_ReceiveZenMapLinkChatOnClient");
            return;
        }

        PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
        if (!pb)
            return;

        if (GetZenMapLinkChatConfig().RequireItemToOpenChat)
        {
            if (pb.ZenMapLinkChat_GetValidItem() == NULL)
            {
                return;
            }
        }

        PluginZenMapLinkChatClient plugin = PluginZenMapLinkChatClient.Cast(GetPlugin(PluginZenMapLinkChatClient));
        ZenMapLinkChatGUI chatMenu;

        if (GetGame().GetUIManager())
		{
            chatMenu = ZenMapLinkChatGUI.Cast(GetGame().GetUIManager().GetMenu());
        }
        
        for (int i = 0; i < data.param1.Count(); i++)
        {
            Print("[ZenMapLinkChat] Add chat message: " + data.param1.Get(i));

            if (plugin)
            {
                plugin.AddChatMessage(data.param1.Get(i));
            }

			if (chatMenu)
			{
				chatMenu.AddChatMessage(data.param1.Get(i));
			}
        }

        if (!GetZenMapLinkChatLocalConfig().MuteSound)
        {
            SEffectManager.PlaySound("tuneRadio_SoundSet", pb.GetPosition());
        }
    }

    void RPC_ReceiveZenMapLinkChatConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Client)
            return;

        Param3<ref ZenMapLinkChatSyncConfig, bool, bool> data;
        if (!ctx.Read(data))
        {
            Error("Error receiving data - RPC_ReceiveZenMapLinkChatConfig");
            return;
        }

        GetZenMapLinkChatSyncConfig().SetConfig(data.param1);

        PluginZenMapLinkChatClient plugin = PluginZenMapLinkChatClient.Cast(GetPlugin(PluginZenMapLinkChatClient));
        if (plugin)
        {
            plugin.SetEnabled(data.param1.EnableMod);
            plugin.SetBanned(data.param2);
            plugin.SetAdmin(data.param3);
        }
    }

    void RPC_ReceiveZenMapLinkChatBanOnClient(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Client)
            return;

        Param1<bool> data;
        if (!ctx.Read(data))
        {
            Error("Error receiving data - RPC_ReceiveZenMapLinkChatBanOnClient");
            return;
        }

        PluginZenMapLinkChatClient plugin = PluginZenMapLinkChatClient.Cast(GetPlugin(PluginZenMapLinkChatClient));
        if (plugin)
        {
            plugin.SetBanned(data.param1);
        }
    }

    void RPC_ReceiveZenMapLinkChatBanlist(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Client)
            return;

        Param1<array<string>> data;
        if (!ctx.Read(data))
        {
            Error("Error receiving data - RPC_ReceiveZenMapLinkChatBanlist");
            return;
        }

        PluginZenMapLinkChatClient plugin = PluginZenMapLinkChatClient.Cast(GetPlugin(PluginZenMapLinkChatClient));
        if (plugin)
        {
            plugin.GetBannedPlayers().Clear();

            for (int i = 0; i < data.param1.Count(); i++)
            {
                plugin.GetBannedPlayers().Insert(data.param1.Get(i));
            }
        }
    }
}