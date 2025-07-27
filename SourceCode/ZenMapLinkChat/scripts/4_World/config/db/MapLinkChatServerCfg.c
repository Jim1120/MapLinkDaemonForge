// This is the only way I could figure out how to do this without overwriting each other etc.
// We basically treat each Server_to_Server link as its own mod config, storing its own chat message array.
// This enables us to verify messages have been relayed before clearing the queue and re-saving it to DB.
class MapLinkChatServerCfg extends UApiConfigBase
{
	string SERVER_LINK = "uninit";
	ref array<string> ChatMessages;

	void SetServerLink(string link)
	{
		SERVER_LINK = link;
	}

	string GetServerLink()
	{
		return SERVER_LINK;
	}

	override void SetDefaults()
	{
		ChatMessages = new array<string>;
	}

	override void OnDataReceive()
	{
		SetDataReceived();

		PluginZenMapLinkChatServer plugin = PluginZenMapLinkChatServer.Cast(GetPlugin(PluginZenMapLinkChatServer));
		if (!plugin)
			return;

		plugin.OnDataReceive(this);
	}

	override void Load()
	{
		if (!m_DataReceived)
		{
			SetDefaults();
		}

		m_DataReceived = false;
		UApi().Rest().GlobalsLoad("MapLinkChat_" + GetServerLink(), this, this.ToJson());
	}

	override void Save()
	{
		if (GetGame().IsDedicatedServer())
		{
			UApi().Rest().GlobalsSave("MapLinkChat_" + GetServerLink(), this.ToJson());
		}
	}

	override string ToJson()
	{
		return UApiJSONHandler<MapLinkChatServerCfg>.ToString(this);
	}

	override void OnSuccess(string data, int dataSize) 
	{
		if (UApiJSONHandler<MapLinkChatServerCfg>.FromString(data, this))
		{
			OnDataReceive();
		} else 
		{
			MLLog.Err("CallBack Failed errorCode: Invalid Data");
		}
	}
}