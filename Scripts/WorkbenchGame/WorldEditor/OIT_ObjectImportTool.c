#ifdef WORKBENCH

[WorkbenchToolAttribute(name: "Object Import Tool", awesomeFontCode: 0xF0D0)]
class OIT_ObjectImportTool : WorldEditorTool
{
	[Attribute("", UIWidgets.FileNamePicker, "Select *.csv file with objects to spawn", params: "csv FileNameFormat=absolute", category: "File")]
	protected string m_sCSVFilePath;
	
	[Attribute(" ", desc: "Defines the character used to separate values in the CSV file", category: "Parsing Parameters")]
	protected string m_sDelimiterCharacter;
	
	[Attribute("\"", desc: "Defines the character used to enclose values that contain special characters like delimiters", category: "Parsing Parameters")]
	protected string m_sQuoteCharacter;
	
	[Attribute("1", UIWidgets.CheckBox, "Allows you to switch between Quaternions and Tate-Bryan angles", category: "Parsing Parameters")]
	protected bool m_bUseQuaternions;
	
	[Attribute("1", UIWidgets.CheckBox, "Should print warnings to console? Disabling this option may speed up the import", category: "Parsing Parameters")]
	protected bool m_bShouldPrintWarnings;
	
	[Attribute("0 0 0", UIWidgets.Coords, "Shifts the position of the objects specified in the CSV file", category: "Objects")]
	protected vector m_vObjectPositionOffset;
	
	[Attribute("1 1 1", UIWidgets.Coords, "Allows you to change the directions of the placed objects", category: "Objects")]
	protected vector m_vObjectPositionMultiplier;
	
	[Attribute("1", desc: "Allows you to multiply the scale value of imported objects", category: "Objects")]
	protected float m_fObjectScaleMultiplier;
	
	protected void DisplayMessage(string message, string postfix = string.Empty)
	{
		string caption = "Object Import Tool";
		
		if (!postfix.IsEmpty())
			caption += ": " + postfix;
		
		Workbench.Dialog(caption, message);
	}
	
	protected string ValidateAttributes()
	{
		if (m_sCSVFilePath.IsEmpty())
			return "CSV file path is not specified.";
		
		if (!FileIO.FileExists(m_sCSVFilePath))
			return "CSV file does not exist at the specified path.";
		
		if (m_sDelimiterCharacter.IsEmpty())
			return "The delimiter must be a character or a string.";
		
		if (m_sQuoteCharacter.Length() != 1)
			return "The quotes character must be specified in one character.";
		
		return string.Empty;
	}
	
	protected vector QuatToAngles(float quaternion[4])
	{
		vector angles = Math3D.QuatToAngles(quaternion);
		return {-angles[1], -angles[0], -angles[2]};
	}
	
	protected bool TryToCreateEntity(array<string> row)
	{
		if (row.Count() < 9)
			return false;
		
		string className = row[0];
		
		float x = row[1].ToFloat();
		float y = row[2].ToFloat();
		float z = row[3].ToFloat();
		
		vector position = {
			(x + m_vObjectPositionOffset[0]) * m_vObjectPositionMultiplier[0],
			(y + m_vObjectPositionOffset[1]) * m_vObjectPositionMultiplier[1],
			(z + m_vObjectPositionOffset[2]) * m_vObjectPositionMultiplier[2]
		};
		
		vector angles;
		int scaleValueIndex;
		
		if (m_bUseQuaternions)
		{
			float xQuaternion = row[4].ToFloat();
			float yQuaternion = row[5].ToFloat();
			float zQuaternion = row[6].ToFloat();
			float wQuaternion = row[7].ToFloat();
			
			float quaternion[4] = {xQuaternion, yQuaternion, zQuaternion, wQuaternion};
			
			angles = QuatToAngles(quaternion);
			scaleValueIndex = 8;
		}
		else
		{
			float yaw = row[4].ToFloat();
			float pitch = row[5].ToFloat();
			float roll = row[6].ToFloat();
			
			angles = {yaw, pitch, roll};
			scaleValueIndex = 7;
		}
		
		float scale = row[scaleValueIndex].ToFloat();
		scale *= m_fObjectScaleMultiplier;
		
		string name = "ImportedObject_" + position.ToString().Hash();
		int layerId = m_API.GetCurrentEntityLayerId();
		
		IEntitySource entitySource = m_API.CreateEntity(className, name, layerId, null, position, angles);
		
		if (!entitySource)
			return false;
		
		IEntity entity = m_API.SourceToEntity(entitySource);
		
		if (!entity)
		{
			m_API.DeleteEntity(entitySource);
			return false;
		}
		
		name = m_API.GenerateDefaultEntityName(entitySource);
		m_API.RenameEntity(entitySource, name);
		
		entity.SetScale(scale);
		
		return true;
	}
	
	protected void DisplayResultMessage(int executionTime, int skippedLines, int totalLines)
	{	
		string postfix;
		string message;
		
		if (skippedLines == totalLines)
		{
			postfix = "Failed";
			message = string.Format("Failed to import objects.\n\nAll %1 lines were not processed.\nCheck the console log for more details.", skippedLines);
		}
		else if (skippedLines != 0)
		{
			postfix = "Completed with warnings";
			message = "There were problems during the import of objects.\nCheck the console log for more details.";
		}
		else
		{
			postfix = "Completed";
			message = string.Format("Imported all %1 objects from the file.", totalLines);
		}
		
		message += string.Format("\n\nRows processed: %1 of %2\n", totalLines - skippedLines, totalLines);
		message += string.Format("Process duration: %1", SCR_FormatHelper.FormatTime(executionTime));
		
		DisplayMessage(message, postfix);
	}
	
	[ButtonAttribute("Import objects")]
	protected void ImportObjects()
	{
		string errorMessage = ValidateAttributes();
		
		if (!errorMessage.IsEmpty())
		{
			DisplayMessage(errorMessage, "Error");
			return;
		}
		
		OIT_CSVParser csvParser = new OIT_CSVParser(
			delimiter: m_sDelimiterCharacter,
			quoteCharacter: m_sQuoteCharacter
		);
		
		FileHandle file = FileIO.OpenFile(m_sCSVFilePath, FileMode.READ);
		
		if (!file)
		{
			DisplayMessage("Failed to access the specified CSV file.", "Error");
			return;
		}
		
		int startedAt = System.GetUnixTime();
		
		string buffer;
		array<string> row = {};
		
		int line = 1;
		int skippedLines = 0;
		
		m_API.BeginEntityAction();
		
		while (file.ReadLine(buffer) > -1)
		{
			csvParser.Parse(buffer, row);
			
			if (!TryToCreateEntity(row))
			{
				if (m_bShouldPrintWarnings)
					Print("Failed to create an entity based on data from line " + line + ", skipping...", LogLevel.WARNING);
				
				skippedLines++;
			}
			
			line++;
		}
		
		m_API.EndEntityAction();
		file.Close();
		
		int executionTime = System.GetUnixTime() - startedAt;
		DisplayResultMessage(executionTime, skippedLines, line - 1);
	}
}

#endif