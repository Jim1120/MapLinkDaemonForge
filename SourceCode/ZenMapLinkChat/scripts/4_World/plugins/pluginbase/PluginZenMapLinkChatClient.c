class PluginZenMapLinkChatClient extends PluginBase
{
    // Make this static so it does not reset between logout/login
    protected static ref array<string> m_ChatMessagesReceived;
    protected static ref array<string> m_BannedPlayerStatus;

    // General vars
    protected float m_DeltaTime;
    protected int m_RecentMessageCounter;
    protected bool m_IsBanned;
    protected bool m_IsAdmin;

	override void OnInit()
    {
        Print("[PluginZenMapLinkChatClient] :: OnInit");

        if (!m_ChatMessagesReceived)
        {
            m_ChatMessagesReceived = new array<string>;
        }

        if (!m_BannedPlayerStatus)
        {
            m_BannedPlayerStatus = new array<string>;
        }

        m_DeltaTime = 0;
        m_RecentMessageCounter = 0;
        m_IsBanned = false;
        m_IsAdmin = false;
    }

    void SetAdmin(bool b)
    {
        m_IsAdmin = b;
    }

    bool IsAdmin()
    {
        return m_IsAdmin;
    }

    void SetBanned(bool b)
    {
        m_IsBanned = b;
    }

    bool IsBanned()
    {
        return m_IsBanned;
    }

    void AddChatMessage(string message)
    {
        if (!GetZenMapLinkChatSyncConfig() || !GetZenMapLinkChatSyncConfig().EnableMod)
        {
            return;
        }

        m_ChatMessagesReceived.Insert(message);
    }

    void SendChatMessage(string message)
    {
        if (!GetZenMapLinkChatSyncConfig() || !GetZenMapLinkChatSyncConfig().EnableMod)
        {
            return;
        }

        if (!GetGame().GetUIManager() || !GetGame().GetUIManager().IsMenuOpen(ZenMapLinkChatGUI_ID))
		    return;

        ZenMapLinkChatGUI gui = ZenMapLinkChatGUI.Cast(GetGame().GetUIManager().FindMenu(ZenMapLinkChatGUI_ID));
        if (!gui)
            return;

        if (IsBanned())
        {
            gui.SetTitle("You are banned from sending chat messages.");
            return;
        }

        // Check throttle counter
        if (m_RecentMessageCounter >= GetZenMapLinkChatSyncConfig().MaxChatCountDuringInterval)
        {
            gui.SetTitle("You cannot send more than " + GetZenMapLinkChatSyncConfig().MaxChatCountDuringInterval + " message(s) within " + GetZenMapLinkChatSyncConfig().ChatThrottleIntervalSecs + " seconds!", 10);
            return;
        }

        GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenMapLinkChatOnServer", new Param1<string>(message), true, NULL);
        m_RecentMessageCounter++;
    }

    array<string> GetChatMessages()
    {
        return m_ChatMessagesReceived;
    }

    array<string> GetBannedPlayers()
    {
        return m_BannedPlayerStatus;
    }

    void BanPlayer(string playerID)
    {
        if (IsAdmin())
        {
            int i;
            bool isBanned;
            int arrayIndex = m_BannedPlayerStatus.Find(playerID);

            if (arrayIndex != -1)
            {
                isBanned = false;
                m_BannedPlayerStatus.Remove(arrayIndex);
            }
            else 
            {
                isBanned = true;
                m_BannedPlayerStatus.Insert(playerID);
            }

            GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenMapLinkChatBanRequested", new Param2<string, bool>(playerID, isBanned), true, NULL);
        }
    }

    override void OnUpdate(float delta_time)
    {
        if (!GetZenMapLinkChatSyncConfig() || !GetZenMapLinkChatSyncConfig().EnableMod)
            return;

        m_DeltaTime += delta_time;

        if (m_DeltaTime < GetZenMapLinkChatSyncConfig().ChatThrottleIntervalSecs)
            return;

        m_DeltaTime = 0;
        m_RecentMessageCounter = 0;
    }

    void SetEnabled(bool isEnabled)
    {
        if (!isEnabled)
        {
            // Not needed.
            delete m_ChatMessagesReceived;
            delete m_BannedPlayerStatus;
        }
    }
}