/*
	(c) 2025 | MapLink_ZenSkillsCompatibility | Zenarchist
*/
class CfgPatches
{
	class MapLink_ZenSkillsCompatibility
	{
		requiredAddons[] =
		{
			"ZenSkills",
			"MapLink"
		};
	};
};

class CfgMods
{
	class MapLink_ZenSkillsCompatibility
	{
		author = "Zenarchist";
		type = "mod";
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = 
				{ 
					"MapLink_ZenSkillsCompatibility/Scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value = "";
				files[] = 
				{ 
					"MapLink_ZenSkillsCompatibility/Scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value = "";
				files[] = 
				{ 
					"MapLink_ZenSkillsCompatibility/Scripts/5_Mission"
				};
			};
		};
	};
};