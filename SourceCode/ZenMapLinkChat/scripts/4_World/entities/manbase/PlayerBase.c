modded class PlayerBase
{
	ItemBase ZenMapLinkChat_GetValidItem()
	{
		ItemBase armbandSlot = ItemBase.Cast(GetItemOnSlot("Armband"));
		if (IsValidZenMapLinkChatItem(armbandSlot))
			return armbandSlot; // Check for ZenWatchGPS

		ItemBase itemInHands = ItemBase.Cast(GetItemInHands());
		if (IsValidZenMapLinkChatItem(itemInHands))
			return itemInHands; // Check item is in hands

		// Check we have a valid item somewhere in our inventory
		int x;
		ItemBase item;
		array<EntityAI> itemsArray = new array<EntityAI>;

		GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

		for (int i = 0; i < itemsArray.Count(); i++)
		{
			item = ItemBase.Cast(itemsArray.Get(i));
			if (IsValidZenMapLinkChatItem(item))
				return ItemBase.Cast(itemsArray.Get(i));
		}

		return NULL;
	}

	bool IsValidZenMapLinkChatItem(EntityAI entity)
	{
		ItemBase item = ItemBase.Cast(entity);

		if (!item || item.IsRuined())
			return false;

		for (int x = 0; x < GetZenMapLinkChatConfig().ItemsWhichCanOpenChat.Count(); x++)
		{
			if (item.IsKindOf(GetZenMapLinkChatConfig().ItemsWhichCanOpenChat.Get(x)))
			{
				if (item.GetCompEM() && !item.GetCompEM().IsWorking())
					continue;

				return true;
			}
		}

		return false;
	}
}