class CfgPatches
{
	class MapLinkTerjeCompatibility
	{
		requiredVersion = 0.1;
		requiredAddons[] =
		{
			"DZ_Data",
			"DZ_Scripts",
			"MapLink",
			"TerjeCore"
		};
	};
};

class CfgMods
{
	class MapLinkTerjeCompatibility
	{
		dir = "MapLinkTerjeCompatibility";
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
					"MapLinkTerjeCompatibility/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				files[] = 
				{ 
					"MapLinkTerjeCompatibility/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				files[] = 
				{ 
					"MapLinkTerjeCompatibility/scripts/5_mission"
				};
			};
		};
	};
};