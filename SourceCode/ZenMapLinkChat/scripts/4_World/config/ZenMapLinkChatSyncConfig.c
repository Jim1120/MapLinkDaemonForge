class ZenMapLinkChatSyncConfig extends Managed
{
	bool EnableMod;
	bool RequireItemToOpenChat;
	int ChatThrottleIntervalSecs;
	int MaxChatCountDuringInterval;
	ref array<string> ItemsWhichCanOpenChat;

	void SetConfig(ZenMapLinkChatSyncConfig cfg)
	{
		m_ZenMapLinkChatSyncConfig = cfg;
	}

	void LoadServerCfg()
	{
		if (!GetGame().IsDedicatedServer())
			return;

		EnableMod = GetZenMapLinkChatConfig().EnableMod;
		RequireItemToOpenChat = GetZenMapLinkChatConfig().RequireItemToOpenChat;
		ChatThrottleIntervalSecs = GetZenMapLinkChatConfig().ChatThrottleIntervalSecs;
		MaxChatCountDuringInterval = GetZenMapLinkChatConfig().MaxChatCountDuringInterval;
		ItemsWhichCanOpenChat = new array<string>;

		for (int i = 0; i < GetZenMapLinkChatConfig().ItemsWhichCanOpenChat.Count(); i++)
		{
			ItemsWhichCanOpenChat.Insert(GetZenMapLinkChatConfig().ItemsWhichCanOpenChat.Get(i));
		}
	}
}

static ref ZenMapLinkChatSyncConfig m_ZenMapLinkChatSyncConfig;

static ZenMapLinkChatSyncConfig GetZenMapLinkChatSyncConfig()
{
	if (!m_ZenMapLinkChatSyncConfig)
	{
		m_ZenMapLinkChatSyncConfig = new ZenMapLinkChatSyncConfig;
		m_ZenMapLinkChatSyncConfig.LoadServerCfg();
	}

	return m_ZenMapLinkChatSyncConfig;
}