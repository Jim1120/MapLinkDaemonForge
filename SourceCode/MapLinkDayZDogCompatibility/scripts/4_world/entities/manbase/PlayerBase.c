modded class PlayerBase
{
	override void SendUApiAfterLoadClient()
	{
		super.SendUApiAfterLoadClient();

		if (HaveDog())
		{
			GetGame().ObjectDelete(GetMyDog());
			InitialDogSpawn(ModelToWorld("-0.5 0 1.5"));
		}
	}
}

modded class PlayerDataStore
{
	override void SavePlayer(PlayerBase player)
	{
		if (player)
		{
			player.StoreDogInventory();
			player.StoreDogHealth();
		}
		
		super.SavePlayer(player);
	}
}