class PluginZenMapLinkChatServer extends PluginBase
{
    static const string SEPARATOR1 = "|";
    static const string SEPARATOR2 = "~";

    // Key = Server_to_Server, Element = Chat messages array
    protected ref map<string, ref array<string>> m_ChatMessagesToSend;

    // General vars
    protected bool m_HasInitPlugin;
    protected bool m_Enabled;
    protected bool m_HasInitServers;
    protected int m_DbCheckTimer;
    protected float m_MasterCfgDeltaTime;
    protected float m_DeltaTime;

    protected ref array<ref MapLinkChatServerCfg> m_ServerLinks;

    override void OnInit()
    {
        m_HasInitPlugin = false;
        m_Enabled = false;
    }

	void OnDeferredInit()
    {
        Print("[PluginZenMapLinkChatServer] :: OnDeferredInit :: Host Server: " + GetOurServerName());
        m_ServerLinks = new array<ref MapLinkChatServerCfg>;
        m_ChatMessagesToSend = new map<string, ref array<string>>;
        m_DbCheckTimer = GetZenMapLinkChatConfig().CheckChatDbDelayTimerSecs;
        m_HasInitServers = false;
        m_DeltaTime = 0;
        m_MasterCfgDeltaTime = 0;
        CheckInitServers();
    }

    override void OnDestroy()
    {
        Print("[PluginZenMapLinkChatServer] OnDestroy");

        if (!m_ServerLinks)
            return;

        // Wipe any unsync'ed messages from our server
        for (int i = 0; i < m_ServerLinks.Count(); i++)
        {
            if (m_ServerLinks.Get(i).GetServerLink().IndexOf(GetOurServerName()) == 0)
            {
                if (m_ServerLinks.Get(i).ChatMessages)
                {
                    m_ServerLinks.Get(i).ChatMessages.Clear();
                    m_ServerLinks.Get(i).Save();
                }
            }
        }
    }

    void PlayerLogin(PlayerIdentity identity)
    {
        if (!identity)
            return;

        GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenMapLinkChatConfig", new Param3<ref ZenMapLinkChatSyncConfig, bool, bool>(GetZenMapLinkChatSyncConfig(), IsPlayerBanned(identity.GetId()), IsAdmin(identity.GetId())), true, identity);
    }

    void SyncBanList(PlayerIdentity identity)
    {
        if (!identity)
            return;

        GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenMapLinkChatBanlist", new Param1<array<string>>(GetZenMapLinkChatConfig().BannedPlayerIDs), true, identity);
    }

    string GetOurServerName()
    {
        return UApiConfig().ServerID;
    }

    bool IsEnabled()
    {
        return m_Enabled;
    }

    void SetEnabled(bool b)
    {
        m_Enabled = b;

        if (m_HasInitPlugin)
        {
            return;
        }

        // CheckInitServers() checks if servers exist and sets up the queues if not.
        // When sever first inits plugins, UAPI is not connected, so delay this a bit.
        if (IsEnabled())
        {
            OnDeferredInit();
        }

        Print("[ZenMapLinkChat] Mod is enabled = " + m_Enabled);
        m_HasInitPlugin = true;
    }

    void BanPlayer(PlayerIdentity admin, string playerID, bool ban)
    {
        if (!admin || !IsAdmin(admin.GetId()))
        {
            return;
        }

        if (IsAdmin(playerID))
        {
            SendNotification(admin, "You can't ban other admins!");
            SyncBanList(admin); // Clears their local banlist
            return;
        }

        int i;

        if (ban)
        {
            if (!IsPlayerBanned(playerID))
            {
                GetZenMapLinkChatConfig().BannedPlayerIDs.Insert(playerID);
                Print("[ZenMapLinkChat] Admin " + admin.GetId() + " just banned player " + playerID + " from MapLink chat.");
            }
            else 
            {
                SendNotification(admin, "Error: That player is already banned!");
                return;
            }
        }
        else 
        {
            bool foundPlayer = false;

            for (i = 0; i < GetZenMapLinkChatConfig().BannedPlayerIDs.Count(); i++)
            {
                if (GetZenMapLinkChatConfig().BannedPlayerIDs.Get(i) == playerID)
                {
                    foundPlayer = true;
                    GetZenMapLinkChatConfig().BannedPlayerIDs.Remove(i);
                    Print("[ZenMapLinkChat] Admin " + admin.GetId() + " just unbanned player " + playerID + " from MapLink chat.");
                    break;
                }
            }

            if (!foundPlayer)
            {
                SendNotification(admin, "Error: That player isn't banned!");
                return;
            }
        }

        string playerName = playerID;
        array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		
		for (i = 0; i < players.Count(); i++)
		{
            PlayerBase pb = PlayerBase.Cast(players.Get(i));
            if (pb && pb.GetIdentity())
            {
                if (pb.GetIdentity().GetId() == playerID)
                {
                    playerName = playerID + " (" + pb.GetIdentity().GetName() + ")";
                    GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenMapLinkChatBanOnClient", new Param1<bool>(ban), true, pb.GetIdentity());
                }
                
                if (IsAdmin(pb.GetIdentity().GetId()))
                {
                    SyncBanList(pb.GetIdentity());
                }
            }
		}

        GetZenMapLinkChatConfig().Save();

        string banned = "banned";
        if (!ban)
            banned = "unbanned";
        
        SendNotification(admin, "You just " + banned + " " + playerName);
    }

    void SendNotification(PlayerIdentity identity, string message, string title = "ADMIN")
    {
        if (!identity)
        {
            return;
        }

        NotificationSystem.SendNotificationToPlayerIdentityExtended(identity, 5.0, title, message, "set:dayz_gui image:icon_player");
    }

    bool IsPlayerBanned(string playerID)
    {
        if (GetZenMapLinkChatConfig().BannedPlayerIDs.Find(playerID) != -1)
        {
            return true;
        }

        return false;
    }

    bool IsPlayerBanned(PlayerIdentity identity)
    {
        if (!identity)
            return false;

        if (IsPlayerBanned(identity.GetId()))
            return true;

        if (IsPlayerBanned(identity.GetPlainId()))
            return true;

        return false;
    }

    bool IsAdmin(string playerID)
    {
        if (GetZenMapLinkChatConfig().AdminIDs.Find(playerID) != -1)
        {
            return true;
        }

        return false;
    }

    bool IsAdmin(PlayerIdentity identity)
    {
        if (!identity)
            return false;

        if (IsAdmin(identity.GetId()))
            return true;

        if (IsAdmin(identity.GetPlainId()))
            return true;

        return false;
    }

    bool HandleAdminCommand(PlayerIdentity admin, string message)
    {
        if (!admin || !IsAdmin(admin.GetId()))
        {
            return false;
        }

        if (message.IndexOf("/") != 0)
        {
            return false;
        }

        message.Replace("/", "");

        array<string> parameters = new array<string>;
        message.Split(" ", parameters);

        if (parameters.Count() >= 2)
        {
            if (parameters.Get(0) == "ban")
            {
                BanPlayer(admin, parameters.Get(1), true);
            }
            else 
            if (parameters.Get(0) == "unban")
            {
                BanPlayer(admin, parameters.Get(1), false);
            }
        }

        return true;
    }

    void AddChatMessage(PlayerIdentity identity, string message)
    {
        if (HandleAdminCommand(identity, message))
        {
            return;
        }

        if (!CheckInitServers() || !identity || IsPlayerBanned(identity.GetId()))
        {
            return;
        }

        if (identity.GetName() == "Survivor" || identity.GetName() == "survivor")
        {
            if (GetZenMapLinkChatConfig().DoNotAllowSurvivorName)
            {
                SendNotification(identity, "#STR_ZenMapLinkChat_NoSurvivorName", "#STR_invalid_ID");
                return;
            }
        }

        message.Replace(SEPARATOR1, "");
        message.Replace(SEPARATOR2, "");

        string chatMessage = GetOurServerName() + SEPARATOR2 + UUtil.GetTimeStamp() + SEPARATOR2 + TruncateName(identity.GetName()) + SEPARATOR2 + message + SEPARATOR2 + identity.GetId();
        Print("[ZenMapLinkChat::" + GetOurServerName() + "] " + identity.GetId() + " (" + identity.GetName() + "): " + message);

        for (int i = 0; i < GetMapLinkConfig().Servers.Count(); i++)
        {
            string serverName = GetMapLinkConfig().Servers.Get(i).Name;

            // Skip self
            if (serverName == GetOurServerName() || serverName == "")
                continue;

            string serverKey = GetOurServerName() + "_to_" + serverName;

            
            if (!m_ChatMessagesToSend.Contains(serverKey))
            {
                m_ChatMessagesToSend.Insert(serverKey, new array<string>);
            }

            m_ChatMessagesToSend.Get(serverKey).Insert(chatMessage);
        }

        // Send chat to our online players
        array<string> tempChatArray = new array<string>;
        tempChatArray.Insert(chatMessage);
        SyncChatArrayToOnlinePlayers(tempChatArray);

        #ifdef GAMELABS
        // Get player object
        int highBits, lowBits;
		GetGame().GetPlayerNetworkIDByIdentityID(identity.GetPlayerId(), lowBits, highBits);
		PlayerBase player = PlayerBase.Cast(GetGame().GetObjectByNetworkId(lowBits, highBits));

        if (!player)
            return;

        _LogPlayerEx logObjectPlayer = new _LogPlayerEx(player);
        _Payload_PlayerChat payloadPlayerChat = new _Payload_PlayerChat(logObjectPlayer, "MapLink", message);
        GetGameLabs().GetApi().PlayerChat(new _Callback(), payloadPlayerChat);
        #endif
    }

    string TruncateName(string name)
    {
	    const int maxLength = 25;
	    if (name.Length() <= maxLength)
		    return name;

	    return name.Substring(0, maxLength - 3) + "...";
    }

    bool CheckInitServers()
    {
        if (!IsEnabled())
        {
            return false;
        }

        if (!m_HasInitServers)
        {
            for (int i = 0; i < GetMapLinkConfig().Servers.Count(); i++)
            {
                // If name is blank then we haven't loaded cfg yet
                if (GetMapLinkConfig().Servers.Get(i).Name == "")
                {
                    continue;
                }

                // Skip Us_to_Us
                if (GetMapLinkConfig().Servers.Get(i).Name == GetOurServerName())
                {
                    continue;
                }

                // eg. If WE are Chernarus, then this is checking: Chernarus_to_Livonia, Chernarus_to_Sakhal
                AddMapLink(GetOurServerName(), GetMapLinkConfig().Servers.Get(i).Name);

                // eg. If WE are Chernarus, then this is checking: Livonia_to_Chernarus, Sakhal_to_Chernarus
                AddMapLink(GetMapLinkConfig().Servers.Get(i).Name, GetOurServerName());
            }

            m_HasInitServers = (m_ServerLinks.Count() > 0);
        }

        return m_HasInitServers;
    }

    void AddMapLink(string fromServer, string toServer)
    {
        string link = fromServer + "_to_" + toServer;
        MapLinkChatServerCfg mapLinkChatCfg = new MapLinkChatServerCfg();
        mapLinkChatCfg.SetServerLink(link);
        m_ServerLinks.Insert(mapLinkChatCfg);
        Print("[ZenMapLinkChat] Registered map chat link: " + link);
    }

    override void OnUpdate(float delta_time)
    {
        if (!IsEnabled())
        {
            return;
        }

        // Reload master config every X seconds (mainly to update ban lists on all servers)
        m_MasterCfgDeltaTime += delta_time;
        if (m_MasterCfgDeltaTime >= GetZenMapLinkChatConfig().ReloadMasterConfigDbDelayTimerSecs)
        {
            m_MasterCfgDeltaTime = 0;
            GetZenMapLinkChatConfig().Load();
        }

        // Reload chat message DB every X seconds
        m_DeltaTime += delta_time;
        if (m_DeltaTime < m_DbCheckTimer)
        {
            return;
        }

        m_DeltaTime = 0;

        if (!CheckInitServers())
        {
            return;
        }
        
        CheckMessages();
    }

    void CheckMessages()
    {
        // First we need to load the JSON data from database. Then when their data is loaded OnDataReceive() will trigger.
        for (int i = 0; i < m_ServerLinks.Count(); i++)
        {
            m_ServerLinks.Get(i).Load();
        }
    }

    void OnDataReceive(MapLinkChatServerCfg cfg)
    {
        // serverKey = eg. Chernarus_to_Livonia, Livonia_to_Chernarus etc.
        string serverKey = cfg.GetServerLink();
        bool hasChatBeenRead = (cfg.ChatMessages.Count() == 0);

        // If we have an existing chat queue for this server, prepare to sync our sent chat
        if (m_ChatMessagesToSend.Contains(serverKey))
        {   
            for (int i = 0; i < m_ChatMessagesToSend.Get(serverKey).Count(); i++)
            {
                cfg.ChatMessages.Insert(m_ChatMessagesToSend.Get(serverKey).Get(i));
            }
            
            // Only save our chat to DB if the old queue has been cleared.
            if (hasChatBeenRead)
            {
                cfg.Save();
                m_ChatMessagesToSend.Get(serverKey).Clear();
            }
        }

        // Check if chat queue is meant for us - if so, relay chat messages, clear queue and save to DB.
        // IndexOf check makes sure we're not checking OurServer_to_OtherServer queues.
        if (serverKey.IndexOf(GetOurServerName()) != 0)
        {
            if (cfg.ChatMessages && cfg.ChatMessages.Count() > 0)
            {
                Print("[ZenMapLinkChat] Relaying messages from: " + serverKey);
                SyncChatArrayToOnlinePlayers(cfg.ChatMessages);
                cfg.ChatMessages.Clear();
                cfg.Save();
            }
        }
    }

    void SyncChatArrayToOnlinePlayers(array<string> chatArray)
    {
        // Create a new array to hold unique values
        array<string> uniqueArray = new array<string>;

        // Use a map as a set to track duplicates
        map<string, bool> seen = new map<string, bool>;

        for (int i = 0; i < chatArray.Count(); i++)
        {
            string value = chatArray.Get(i);
            if (!seen.Contains(value))
            {
                seen.Insert(value, true);
                uniqueArray.Insert(value);
            }
            else 
            {
                Print("[ZenMapLinkChat] Skipping duplicate message: " + value);
            }
        }

        // Now send only the unique values
        GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenMapLinkChatOnClient", new Param1<array<string>>(uniqueArray), true, NULL);
    }
}