

//! TODO: Stringtable everything.


class ZenMapLinkChatGUI extends UIScriptedMenu
{
	static const int MAXIMUM_CHARACTERS = 100; // Beyond this could cut off on 1080p
	static const string	LAYOUT_FILE		= "ZenMapLinkChat/data/gui/layouts/WatchGUI";

	protected ref array<string> m_PlayerIdentities;
	protected float m_DeltaTime = 0;

	protected ref ImageWidget m_BackgroundImage;
	protected ref ImageWidget m_BatteryImage;
	protected ref ImageWidget m_MuteImage;
	protected ref ImageWidget m_PowerImage;
	protected ref ProgressBarWidget m_BatteryLevel;
	protected ref EditBoxWidget m_ChatEditBox;
	protected ref TextListboxWidget m_ChatTextList;
	protected ref MultilineTextWidget m_TitleText;
	protected ref ButtonWidget m_BanButton;

	protected ref PluginZenMapLinkChatClient m_ChatPlugin;

    override Widget Init()
    {
		m_ChatPlugin = PluginZenMapLinkChatClient.Cast(GetPlugin(PluginZenMapLinkChatClient));

		int width, height;
		GetScreenSize(width, height);

		// Text List Boxes have fucked text scaling and look stupid small on 4k monitors and this is the only way I can fix it. Dumb, I know.
		if (width > 3800) // 4k
		{
			layoutRoot = GetGame().GetWorkspace().CreateWidgets(LAYOUT_FILE + "_4k.layout");
		}
		else 
		{
			layoutRoot = GetGame().GetWorkspace().CreateWidgets(LAYOUT_FILE + ".layout");
		}

		m_BackgroundImage = ImageWidget.Cast(layoutRoot.FindAnyWidget("BackgroundImage"));
		m_BatteryImage = ImageWidget.Cast(layoutRoot.FindAnyWidget("BatteryImage"));
		m_MuteImage = ImageWidget.Cast(layoutRoot.FindAnyWidget("MuteImage"));
		m_PowerImage = ImageWidget.Cast(layoutRoot.FindAnyWidget("PowerImage"));
		m_ChatEditBox = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("ChatEditBox"));
		m_ChatTextList = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("ChatTextList"));
		m_BatteryLevel = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("BatteryLevel"));
		m_TitleText = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("TitleText"));
		m_BanButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BanButton"));

		m_BackgroundImage.LoadImageFile(0, "ZenMapLinkChat/data/gui/images/background.edds");
		m_BatteryImage.LoadImageFile(0, "ZenMapLinkChat/data/gui/images/battery.edds");
		m_PowerImage.LoadImageFile(0, "ZenMapLinkChat/data/gui/images/power.edds");

		m_PlayerIdentities = new array<string>;

        return layoutRoot;
    }

	void SetTitle(string text, int resetAfterSecs = 0)
	{
		m_TitleText.SetText(text);

		if (resetAfterSecs > 0)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(SetTitle);
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(SetTitle, resetAfterSecs * 1000, false, "");
		}
	}

	void UpdateChatMessages()
	{
		if (m_PlayerIdentities)
		{
			m_PlayerIdentities.Clear();
		}
		
		m_ChatTextList.ClearItems();

		for (int i = 0; i < m_ChatPlugin.GetChatMessages().Count(); i++)
		{
			AddChatMessage(m_ChatPlugin.GetChatMessages().Get(i));
		}
	}

	void AddChatMessage(string message)
	{
		array<string> columns = new array<string>;
		message.Split(PluginZenMapLinkChatServer.SEPARATOR2, columns);

		if (columns.Count() != 5)
		{
			Error("[ZenMapLinkChatGUI] Received weird chat message format - expected 5 columns, got " + columns.Count() + " out of: " + message);
			return;
		}

		// Server name
		int row = m_ChatTextList.AddItem("[" + columns.Get(0) + "]", NULL, 0);

		// Timestamp
		m_ChatTextList.SetItem(row, "[" + columns.Get(1) + "]", NULL, 1);

		// Name
		m_ChatTextList.SetItem(row, "[" + columns.Get(2) + "]", NULL, 2);

		// Message
		m_ChatTextList.SetItem(row, columns.Get(3), NULL, 3);

		// Player identity 
		if (m_PlayerIdentities)
		{
			m_PlayerIdentities.Insert(columns.Get(4));
		}

		// Show last row 
		m_ChatTextList.EnsureVisible(m_ChatTextList.GetNumItems() - 1);
		//m_ChatTextList.SelectRow(m_ChatTextList.GetNumItems() - 1);
	}

	override void OnShow()
	{
		super.OnShow();

		GetGame().GetMission().AddActiveInputExcludes({"menu"});
		GetGame().GetUIManager().ShowCursor(true);

		UpdateBatteryEnergy();
		UpdateChatMessages();
		UpdateMuteButton();

		SetFocus(m_ChatEditBox);

		m_TitleText.SetText("");
		m_BanButton.Show(false);
	}

	void UpdateMuteButton()
	{
		if (GetZenMapLinkChatLocalConfig().MuteSound)
		{
			m_MuteImage.LoadImageFile(0, "ZenMapLinkChat/data/gui/images/mute_on.edds");
		}
		else 
		{
			m_MuteImage.LoadImageFile(0, "ZenMapLinkChat/data/gui/images/mute_off.edds");
		}
	}

	//! TODO: Find a more robust method to get the energy level (CompEM() stuff doesn't sync to client and I don't wanna RPC server for it)
	void UpdateBatteryEnergy()
	{
		if (GetZenMapLinkChatConfig().RequireItemToOpenChat)
        {
			PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
			if (!pb)
				return;

            ItemBase item = ItemBase.Cast(pb.ZenMapLinkChat_GetValidItem());
            if (!item)
            {
				Close();
				return;
			}

			if (item.GetCompEM())
			{
				ItemBase energyItem = ItemBase.Cast(item.FindAttachmentBySlotName("BatteryD"));

				if (!energyItem)
					energyItem = ItemBase.Cast(item.FindAttachmentBySlotName("CarBattery"));

				if (energyItem)
				{
					m_BatteryImage.Show(true);
					m_BatteryLevel.SetCurrent((energyItem.GetQuantity() / energyItem.GetQuantityMax()) * 100);

					if (energyItem.GetQuantity() == 0)
					{
						Close();
					}

					return;
				}
			}
        }

		// If we made it this far, the item has a weird energy method unsupported with above code or require item is disabled.
		m_BatteryImage.Show(false);
		m_BatteryLevel.Show(false);
	}

	override void OnHide()
	{
		super.OnHide();

		GetGame().GetMission().RemoveActiveInputExcludes({"menu"});
		GetGame().GetUIManager().ShowCursor(false);
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);

		if (KeyState(KeyCode.KC_ESCAPE) == 1)
		{
			Close();
			return;
		}

		if (KeyState(KeyCode.KC_RETURN) == 1)
		{
			SendTextChat();
			return;
		}

		m_DeltaTime += timeslice;

		if (m_DeltaTime < 1)
			return;

		UpdateBatteryEnergy();
		m_DeltaTime = 0;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int selectedRow;

		if (w.GetName() == "PowerButton")
		{
			Close();
			return true;
		}

		if (w.GetName() == "SendChatButton")
		{
			SendTextChat();
			return true;
		}

		if (w.GetName() == "BanButton")
		{
			if (m_PlayerIdentities)
			{
				selectedRow = m_ChatTextList.GetSelectedRow();
				if (selectedRow == -1)
				{
					return true;
				}

				m_ChatPlugin.BanPlayer(m_PlayerIdentities.Get(selectedRow));
				UpdateBanButtonText(selectedRow, true);
			}

			return true;
		}

		// Mute button
		if (w.GetName() == "MuteButton")
		{
			GetZenMapLinkChatLocalConfig().MuteSound = !GetZenMapLinkChatLocalConfig().MuteSound;
			GetZenMapLinkChatLocalConfig().Save();
			UpdateMuteButton();
			return true;
		}

		if (w == m_ChatTextList)
		{
			UpdateBanButtonText(m_ChatTextList.GetSelectedRow());
			return true;
		}

		return super.OnClick(w, x, y, button);
	}

	void UpdateBanButtonText(int row, bool displayOnTitle = false)
	{
		if (!m_ChatPlugin.IsAdmin())
		{
			return;
		}

		if (row == -1)
		{
			return;
		}

		if (row >= m_PlayerIdentities.Count())
		{
			return;
		}

		m_BanButton.Show(true);
		string playerID = m_PlayerIdentities.Get(row);

		foreach (string pid : m_ChatPlugin.GetBannedPlayers())
		{
			if (pid == playerID)
			{
				m_BanButton.SetText("UNBAN SELECTED PLAYER"); // todo stringtable

				if (displayOnTitle)
				{
					UpdateTextStatus("Banned", playerID);
				}

				return;
			}
		}

		m_BanButton.SetText("BAN SELECTED PLAYER");

		if (displayOnTitle)
		{
			UpdateTextStatus("Unbanned", playerID);
		}
	}

	void UpdateTextStatus(string message, string playerID)
	{
		string bannedPlayerName = "";

		for (int i = 0; i < m_ChatPlugin.GetChatMessages().Count(); i++)
		{
			array<string> columns = new array<string>;
			m_ChatPlugin.GetChatMessages().Get(i).Split(PluginZenMapLinkChatServer.SEPARATOR2, columns);

			if (columns.Get(4) == playerID)
			{
				bannedPlayerName = columns.Get(2);
				break;
			}
		}

		SetTitle(message + " " + bannedPlayerName);
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		// Limit text length so it displays ok across all main resolutions (1920x1080->4k)
		if (w == m_ChatEditBox)
		{
			string text = m_ChatEditBox.GetText();
			if (text.Length() > MAXIMUM_CHARACTERS)
				text = text.Substring(0, MAXIMUM_CHARACTERS);

			m_ChatEditBox.SetText(text);
			return true;
		}

		return super.OnChange(w, x, y, finished);
	}

	void SendTextChat()
	{
		string text = m_ChatEditBox.GetText().Trim();

		if (text != "")
		{
			m_ChatPlugin.SendChatMessage(text);
		}			

		m_ChatEditBox.SetText("");
	}
}