class ZenMapLinkChatConfig extends UApiConfigBase
{
	bool EnableMod;
	int CheckChatDbDelayTimerSecs;
	int ReloadMasterConfigDbDelayTimerSecs;
	bool RequireItemToOpenChat;
	bool DoNotAllowSurvivorName;
	int ChatThrottleIntervalSecs;
	int MaxChatCountDuringInterval;
	ref array<string> ItemsWhichCanOpenChat;
	ref array<string> AdminIDs;
	ref array<string> BannedPlayerIDs;

	override void SetDefaults()
	{
		EnableMod = false;
		CheckChatDbDelayTimerSecs = 30;
		ReloadMasterConfigDbDelayTimerSecs = 300;
		ChatThrottleIntervalSecs = 3;
		MaxChatCountDuringInterval = 1;
		RequireItemToOpenChat = true;
		DoNotAllowSurvivorName = true;
		ItemsWhichCanOpenChat = new array<string>;
		ItemsWhichCanOpenChat.Insert("PersonalRadio");
		ItemsWhichCanOpenChat.Insert("BaseRadio");
		ItemsWhichCanOpenChat.Insert("ZenWatchGPS");
		AdminIDs = new array<string>;
		BannedPlayerIDs = new array<string>;
	}

	override void OnDataReceive()
	{
		SetDataReceived();

		PluginZenMapLinkChatServer plugin = PluginZenMapLinkChatServer.Cast(GetPlugin(PluginZenMapLinkChatServer));
		if (!plugin)
			return;

		plugin.SetEnabled(EnableMod);

		GetZenMapLinkChatSyncConfig().LoadServerCfg();
	}

	override void Load()
	{
		if (!m_DataReceived)
		{
			SetDefaults();
		}

		m_DataReceived = false;
		UApi().Rest().GlobalsLoad("MapLinkChatConfig", this, this.ToJson());
	}

	override void Save()
	{
		if (GetGame().IsDedicatedServer())
		{
			UApi().Rest().GlobalsSave("MapLinkChatConfig", this.ToJson());
		}
	}

	override string ToJson()
	{
		return UApiJSONHandler<ZenMapLinkChatConfig>.ToString(this);
	}

	override void OnSuccess(string data, int dataSize) 
	{
		if (UApiJSONHandler<ZenMapLinkChatConfig>.FromString(data, this))
		{
			OnDataReceive();
		} else 
		{
			MLLog.Err("CallBack Failed errorCode: Invalid Data");
		}
	}
};

ref ZenMapLinkChatConfig m_ZenMapLinkChatConfig;

static ZenMapLinkChatConfig GetZenMapLinkChatConfig()
{
	if (!m_ZenMapLinkChatConfig)
	{
		Print("[ZenMapLinkChatConfig] Init");
		m_ZenMapLinkChatConfig = new ZenMapLinkChatConfig;
		m_ZenMapLinkChatConfig.Load();
	}

	return m_ZenMapLinkChatConfig;
}