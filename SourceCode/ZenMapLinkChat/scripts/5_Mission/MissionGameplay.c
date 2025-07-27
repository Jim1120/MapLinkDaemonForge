modded class MissionGameplay
{
	override UIScriptedMenu CreateScriptedMenu(int id)
    {
        UIScriptedMenu menu = super.CreateScriptedMenu(id);

        if (!menu)
        {
            switch (id)
            {
                case ZenMapLinkChatGUI_ID:
                {
                    menu = new ZenMapLinkChatGUI;
                    menu.SetID(id);
                    break;
                }
            }
        }

        return menu;
    }

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice); 

		if (!GetGame())
        {
			return;
        }

		UpdateZenMapLinkChatInput();
	}

	void UpdateZenMapLinkChatInput()
	{
        if (!GetGame().GetUIManager())
        {
            return;
        }

		if (GetGame().GetInput().LocalPress("UAZenMapLinkChatGUI"))
		{
            if (GetGame().GetUIManager().GetMenu() != NULL) 
            {
                return;
            }
            
            if (!GetZenMapLinkChatSyncConfig() || !GetZenMapLinkChatSyncConfig().EnableMod)
            {
                return;
            }

            PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
            if (!pb)
            {
                return;
            }

            if (GetZenMapLinkChatSyncConfig().RequireItemToOpenChat)
            {
                if (!pb.ZenMapLinkChat_GetValidItem())
                {
                    return;
                }
            }

            ZenMapLinkChatGUI gui = ZenMapLinkChatGUI.Cast(GetGame().GetUIManager().EnterScriptedMenu(ZenMapLinkChatGUI_ID, NULL));
		}
	}
}