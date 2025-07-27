class ZenMapLinkChatLocalConfig extends Managed
{
	private const static string zenModFolder = "$profile:\\Zenarchist\\";
	private const static string zenConfigName = "ZenMapLinkChatLocalConfig.json";

	bool MuteSound = false;

	void Load()
	{
		if (GetGame().IsDedicatedServer())
			return;

		string serverPath;
		GetCLIParam("connect", serverPath);
		serverPath.Replace(":", "");
		serverPath.Replace(".", "");

		if (FileExist(zenModFolder + serverPath + "\\" + zenConfigName))
		{
			JsonFileLoader<ZenMapLinkChatLocalConfig>.JsonLoadFile(zenModFolder + serverPath + "\\" + zenConfigName, this);
			return;
		}
		
		Save();
	}

	void Save()
	{
		// If config folder doesn't exist, create it.
		if (!FileExist(zenModFolder))
		{
			MakeDirectory(zenModFolder);
		}

		string serverPath;
		GetCLIParam("connect", serverPath);
		serverPath.Replace(":", "");
		serverPath.Replace(".", "");

		if (!FileExist(zenModFolder + serverPath))
		{
			MakeDirectory(zenModFolder + serverPath);
		}

		string savePath = zenModFolder + serverPath + "\\" + zenConfigName;
		JsonFileLoader<ZenMapLinkChatLocalConfig>.JsonSaveFile(savePath, this);
	}
}

static ref ZenMapLinkChatLocalConfig m_ZenMapLinkChatLocalConfig;

static ZenMapLinkChatLocalConfig GetZenMapLinkChatLocalConfig()
{
	if (!m_ZenMapLinkChatLocalConfig)
	{
		Print("[ZenMapLinkChatLocalConfig] Init");
		m_ZenMapLinkChatLocalConfig = new ZenMapLinkChatLocalConfig;
		m_ZenMapLinkChatLocalConfig.Load();
	}

	return m_ZenMapLinkChatLocalConfig;
}