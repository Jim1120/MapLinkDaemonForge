class CfgPatches
{
	class ZenMapLinkChat
	{
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"UniversalApi",
			"MapLink"
		};
	};
};

class CfgMods
{
	class ZenMapLinkChat
	{
		dir="ZenMapLinkChat";
        name="ZenMapLinkChat";
        credits="";
        author="";
        authorID="0";
        version="0.1";
        extra=0;
        type="mod";
		inputs = "ZenMapLinkChat/data/inputs.xml";
	    dependencies[]={ "Game", "World", "Mission"};
	    class defs
	    {
			class gameScriptModule
            {
				value = "";
                files[]=
				{
					"ZenMapLinkChat/scripts/3_Game"
				};
            };
			class worldScriptModule
            {
                value="";
                files[]=
				{ 
					"ZenMapLinkChat/scripts/4_World"
				};
            };
			
	        class missionScriptModule
            {
                value="";
                files[]=
				{
					"ZenMapLinkChat/scripts/5_Mission"
				};
            };
        };
    };
};