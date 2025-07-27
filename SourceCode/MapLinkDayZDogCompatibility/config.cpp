class CfgPatches
{
	class MapLinkDayZDogCompatibility
	{
		requiredVersion = 0.1;
		requiredAddons[] =
		{
			"DZ_Data",
			"DZ_Scripts",
			"MapLink",
			"Dayz_Dog"
		};
	};
};

class CfgMods
{
	class MapLinkDayZDogCompatibility
	{
		dir = "MapLinkDayZDogCompatibility";
		name = "";
		author = "";
		credits = "y";
		version = "1.0";
		type = "mod";
		dependencies[] = 
		{ 
			"Game", 
			"World", 
			"Mission" 
		};
		class defs
		{
			class gameScriptModule
			{
				files[] = 
				{ 
					"MapLinkDayZDogCompatibility/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				files[] = 
				{ 
					"MapLinkDayZDogCompatibility/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				files[] = 
				{ 
					"MapLinkDayZDogCompatibility/scripts/5_mission"
				};
			};
		};
	};
};