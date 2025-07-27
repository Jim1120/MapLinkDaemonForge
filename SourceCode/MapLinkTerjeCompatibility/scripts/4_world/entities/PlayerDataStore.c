// Saves Terje data to database. 
// We can't seem to be able to save map<key, value> objects so I had to get ChatGPT fancy on its ass.
// This code basically just deconstructs the maps into separate arrays and re-assembles them when player loads.
// It *should* also track any future changes to Terje's mods automatically, assuming he sticks with this database style.
modded class PlayerDataStore
{
	// Adds section tag to track "Profile" vs "Stats" etc
	ref array<string> m_Terje_Sections;

	// Simple types
	ref array<string> m_Terje_SingleTypes;
	ref array<int>    m_Terje_SingleIndices;
	ref array<int>    m_Terje_SingleInts;
	ref array<float>  m_Terje_SingleFloats;
	ref array<string> m_Terje_SingleStrings;
	ref array<bool>   m_Terje_SingleBools;

	// Arrays
	ref array<string> m_Terje_ArrayTypes;
	ref array<int>    m_Terje_ArrayRecordIndices;
	ref array<int>    m_Terje_ArrayValueIndices;
	ref array<int>    m_Terje_ArrayIntValues;
	ref array<float>  m_Terje_ArrayFloatValues;
	ref array<string> m_Terje_ArrayStringValues;

	// Maps
	ref array<string> m_Terje_MapTypes;
	ref array<int>    m_Terje_MapRecordIndices;
	ref array<string> m_Terje_MapKeys;
	ref array<int>    m_Terje_MapIntValues;
	ref array<float>  m_Terje_MapFloatValues;
	ref array<string> m_Terje_MapStringValues;

	override void SavePlayer(PlayerBase player)
	{
		if (!player)
		{
			return;
		}

		// Initialize arrays
		m_Terje_Sections = new array<string>();

		m_Terje_SingleTypes = new array<string>();
		m_Terje_SingleIndices = new array<int>();
		m_Terje_SingleInts = new array<int>();
		m_Terje_SingleFloats = new array<float>();
		m_Terje_SingleStrings = new array<string>();
		m_Terje_SingleBools = new array<bool>();

		m_Terje_ArrayTypes = new array<string>();
		m_Terje_ArrayRecordIndices = new array<int>();
		m_Terje_ArrayValueIndices = new array<int>();
		m_Terje_ArrayIntValues = new array<int>();
		m_Terje_ArrayFloatValues = new array<float>();
		m_Terje_ArrayStringValues = new array<string>();

		m_Terje_MapTypes = new array<string>();
		m_Terje_MapRecordIndices = new array<int>();
		m_Terje_MapKeys = new array<string>();
		m_Terje_MapIntValues = new array<int>();
		m_Terje_MapFloatValues = new array<float>();
		m_Terje_MapStringValues = new array<string>();

		TerjePlayerRecordsBase profile = player.GetTerjeProfile();
		if (profile)
		{
			SaveAllTerjeRecordsAsFlatArrays(profile, "Profile");
			Print("[MapLink] Saved Terje profile records for " + player.GetCachedID() + ".");
		}

		TerjePlayerRecordsBase stats = player.GetTerjeStats();
		if (stats)
		{
			SaveAllTerjeRecordsAsFlatArrays(stats, "Stats");
			Print("[MapLink] Saved Terje stats records for " + player.GetCachedID() + ".");
		}

		super.SavePlayer(player);
	}

	override void SetupPlayer(PlayerBase player, vector Pos = vector.Zero, vector Ori = vector.Zero)
	{
		super.SetupPlayer(player, Pos, Ori);

		// Load to profile and stats separately
		LoadAllTerjeRecordsFromFlatArrays(player.GetTerjeProfile(), "Profile");
		LoadAllTerjeRecordsFromFlatArrays(player.GetTerjeStats(), "Stats");
	}

	void SaveAllTerjeRecordsAsFlatArrays(TerjePlayerRecordsBase records, string section)
	{
		int i, j;
		for (i = 0; i < records.MapLink_GetOrderedRecords().Count(); i++)
		{
			TerjeRecordBase record = records.MapLink_GetOrderedRecords().Get(i);
			string recordType = record.GetRecordTypeId();

			if (recordType == "int")
			{
				TerjeRecordInt r1 = TerjeRecordInt.Cast(record);
				m_Terje_Sections.Insert(section);
				m_Terje_SingleTypes.Insert(recordType);
				m_Terje_SingleIndices.Insert(i);
				m_Terje_SingleInts.Insert(r1.GetValue());
				m_Terje_SingleFloats.Insert(0.0);
				m_Terje_SingleStrings.Insert("");
				m_Terje_SingleBools.Insert(false);
			}
			else 
			if (recordType == "num")
			{
				TerjeRecordFloat r2 = TerjeRecordFloat.Cast(record);
				m_Terje_Sections.Insert(section);
				m_Terje_SingleTypes.Insert(recordType);
				m_Terje_SingleIndices.Insert(i);
				m_Terje_SingleInts.Insert(0);
				m_Terje_SingleFloats.Insert(r2.GetValue());
				m_Terje_SingleStrings.Insert("");
				m_Terje_SingleBools.Insert(false);
			}
			else 
			if (recordType == "str")
			{
				TerjeRecordString r3 = TerjeRecordString.Cast(record);
				m_Terje_Sections.Insert(section);
				m_Terje_SingleTypes.Insert(recordType);
				m_Terje_SingleIndices.Insert(i);
				m_Terje_SingleInts.Insert(0);
				m_Terje_SingleFloats.Insert(0.0);
				m_Terje_SingleStrings.Insert(r3.GetValue());
				m_Terje_SingleBools.Insert(false);
			}
			else 
			if (recordType == "bool")
			{
				TerjeRecordBool r4 = TerjeRecordBool.Cast(record);
				m_Terje_Sections.Insert(section);
				m_Terje_SingleTypes.Insert(recordType);
				m_Terje_SingleIndices.Insert(i);
				m_Terje_SingleInts.Insert(0);
				m_Terje_SingleFloats.Insert(0.0);
				m_Terje_SingleStrings.Insert("");
				m_Terje_SingleBools.Insert(r4.GetValue());
			}
			else 
			if (recordType == "arrint")
			{
				TerjeRecordIntArray r5 = TerjeRecordIntArray.Cast(record);
				for (j = 0; j < r5.GetValuesCount(); j++)
				{
					m_Terje_Sections.Insert(section);
					m_Terje_ArrayTypes.Insert(recordType);
					m_Terje_ArrayRecordIndices.Insert(i);
					m_Terje_ArrayValueIndices.Insert(j);
					m_Terje_ArrayIntValues.Insert(r5.GetValue(j));
					m_Terje_ArrayFloatValues.Insert(0.0);
					m_Terje_ArrayStringValues.Insert("");
				}
			}
			else 
			if (recordType == "arrnum")
			{
				TerjeRecordFloatArray r6 = TerjeRecordFloatArray.Cast(record);
				for (j = 0; j < r6.GetValuesCount(); j++)
				{
					m_Terje_Sections.Insert(section);
					m_Terje_ArrayTypes.Insert(recordType);
					m_Terje_ArrayRecordIndices.Insert(i);
					m_Terje_ArrayValueIndices.Insert(j);
					m_Terje_ArrayIntValues.Insert(0);
					m_Terje_ArrayFloatValues.Insert(r6.GetValue(j));
					m_Terje_ArrayStringValues.Insert("");
				}
			}
			else 
			if (recordType == "arrstr")
			{
				TerjeRecordStringArray r7 = TerjeRecordStringArray.Cast(record);
				for (j = 0; j < r7.GetValuesCount(); j++)
				{
					m_Terje_Sections.Insert(section);
					m_Terje_ArrayTypes.Insert(recordType);
					m_Terje_ArrayRecordIndices.Insert(i);
					m_Terje_ArrayValueIndices.Insert(j);
					m_Terje_ArrayIntValues.Insert(0);
					m_Terje_ArrayFloatValues.Insert(0.0);
					m_Terje_ArrayStringValues.Insert(r7.GetValue(j));
				}
			}
			else 
			if (recordType == "mapint")
			{
				TerjeRecordIntMap r8 = TerjeRecordIntMap.Cast(record);
				map<string, int> data1 = r8.GetValues();
				foreach (string key1, int val1 : data1)
				{
					m_Terje_Sections.Insert(section);
					m_Terje_MapTypes.Insert(recordType);
					m_Terje_MapRecordIndices.Insert(i);
					m_Terje_MapKeys.Insert(key1);
					m_Terje_MapIntValues.Insert(val1);
					m_Terje_MapFloatValues.Insert(0.0);
					m_Terje_MapStringValues.Insert("");
				}
			}
			else 
			if (recordType == "mapnum")
			{
				TerjeRecordFloatMap r9 = TerjeRecordFloatMap.Cast(record);
				map<string, float> data2 = r9.GetValues();
				foreach (string key2, float val2 : data2)
				{
					m_Terje_Sections.Insert(section);
					m_Terje_MapTypes.Insert(recordType);
					m_Terje_MapRecordIndices.Insert(i);
					m_Terje_MapKeys.Insert(key2);
					m_Terje_MapIntValues.Insert(0);
					m_Terje_MapFloatValues.Insert(val2);
					m_Terje_MapStringValues.Insert("");
				}
			}
			else 
			if (recordType == "mapstr")
			{
				TerjeRecordStringMap r10 = TerjeRecordStringMap.Cast(record);
				map<string, string> data3 = r10.GetValues();
				foreach (string key3, string val3 : data3)
				{
					m_Terje_Sections.Insert(section);
					m_Terje_MapTypes.Insert(recordType);
					m_Terje_MapRecordIndices.Insert(i);
					m_Terje_MapKeys.Insert(key3);
					m_Terje_MapIntValues.Insert(0);
					m_Terje_MapFloatValues.Insert(0.0);
					m_Terje_MapStringValues.Insert(val3);
				}
			}
		}
	}

	void LoadAllTerjeRecordsFromFlatArrays(TerjePlayerRecordsBase records, string section)
	{
		int i, recordIndex;
		string typeId;

		// Load simple values
		if (m_Terje_SingleTypes && m_Terje_SingleTypes.Count() > 0)
		{
			for (i = 0; i < m_Terje_SingleTypes.Count(); i++)
			{
				if (m_Terje_Sections.Get(i) != section) 
					continue;

				typeId = m_Terje_SingleTypes.Get(i);
				recordIndex = m_Terje_SingleIndices.Get(i);

				if (typeId == "int")
					TerjeRecordInt.Cast(records.MapLink_GetOrderedRecords().Get(recordIndex)).SetValue(m_Terje_SingleInts.Get(i));
				else 
				if (typeId == "num")
					TerjeRecordFloat.Cast(records.MapLink_GetOrderedRecords().Get(recordIndex)).SetValue(m_Terje_SingleFloats.Get(i));
				else 
				if (typeId == "str")
					TerjeRecordString.Cast(records.MapLink_GetOrderedRecords().Get(recordIndex)).SetValue(m_Terje_SingleStrings.Get(i));
				else 
				if (typeId == "bool")
					TerjeRecordBool.Cast(records.MapLink_GetOrderedRecords().Get(recordIndex)).SetValue(m_Terje_SingleBools.Get(i));
			}
		}

		// Load arrays
		for (i = 0; i < m_Terje_ArrayTypes.Count(); i++)
		{
			if (m_Terje_Sections.Get(i) != section) 
				continue;

			typeId = m_Terje_ArrayTypes.Get(i);
			recordIndex = m_Terje_ArrayRecordIndices.Get(i);
			int valueIndex = m_Terje_ArrayValueIndices.Get(i);

			if (typeId == "arrint")
			{
				TerjeRecordIntArray r5 = TerjeRecordIntArray.Cast(records.MapLink_GetOrderedRecords().Get(recordIndex));
				while (r5.GetValuesCount() <= valueIndex) { r5.AddValue(0); }
				r5.SetValue(valueIndex, m_Terje_ArrayIntValues.Get(i));
			}
			else 
			if (typeId == "arrnum")
			{
				TerjeRecordFloatArray r6 = TerjeRecordFloatArray.Cast(records.MapLink_GetOrderedRecords().Get(recordIndex));
				while (r6.GetValuesCount() <= valueIndex) { r6.AddValue(0.0); }
				r6.SetValue(valueIndex, m_Terje_ArrayFloatValues.Get(i));
			}
			else 
			if (typeId == "arrstr")
			{
				TerjeRecordStringArray r7 = TerjeRecordStringArray.Cast(records.MapLink_GetOrderedRecords().Get(recordIndex));
				while (r7.GetValuesCount() <= valueIndex) { r7.AddValue(""); }
				r7.SetValue(valueIndex, m_Terje_ArrayStringValues.Get(i));
			}
		}

		// Load maps
		for (i = 0; i < m_Terje_MapTypes.Count(); i++)
		{
			if (m_Terje_Sections.Get(i) != section) 
				continue;

			typeId = m_Terje_MapTypes.Get(i);
			recordIndex = m_Terje_MapRecordIndices.Get(i);
			string key = m_Terje_MapKeys.Get(i);

			if (typeId == "mapint")
				TerjeRecordIntMap.Cast(records.MapLink_GetOrderedRecords().Get(recordIndex)).SetValue(key, m_Terje_MapIntValues.Get(i));
			else 
			if (typeId == "mapnum")
				TerjeRecordFloatMap.Cast(records.MapLink_GetOrderedRecords().Get(recordIndex)).SetValue(key, m_Terje_MapFloatValues.Get(i));
			else 
			if (typeId == "mapstr")
				TerjeRecordStringMap.Cast(records.MapLink_GetOrderedRecords().Get(recordIndex)).SetValue(key, m_Terje_MapStringValues.Get(i));
		}
	}
}