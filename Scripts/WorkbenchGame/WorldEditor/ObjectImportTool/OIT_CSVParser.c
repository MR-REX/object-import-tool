class OIT_CSVParser
{
	protected string m_sDelimiter;
	protected string m_sQuoteCharacter;
	
	protected bool m_bIsMultiCharDelimiter;
	
	void OIT_CSVParser(string delimiter, string quoteCharacter)
	{
		m_sDelimiter = delimiter;
		m_sQuoteCharacter = quoteCharacter;
		
		m_bIsMultiCharDelimiter = delimiter.Length() > 1;
	}
	
	protected void ParseWithSingleCharDelimiter(string text, out array<string> row)
	{
		row.Clear();
		
		bool isInQuotes = false;
		
		string buffer = "";
		string currentChar;
		
		for (int i = 0; i < text.Length(); i++)
		{
			currentChar = text.Get(i);
			
			if (currentChar == m_sQuoteCharacter)
			{
				isInQuotes = !isInQuotes;
				continue;
			}
			
			if (currentChar == m_sDelimiter && !isInQuotes)
			{
				row.Insert(buffer.Trim());
				buffer = "";
				
				continue;
			}
			
			buffer += currentChar;
		}
		
		if (!buffer.IsEmpty())
			row.Insert(buffer.Trim());
	}
	
	protected void ParseWithMultiCharDelimiter(string text, out array<string> row)
	{
		row.Clear();
		
		bool isInQuotes = false;
		
		string buffer = "";
		string currentChar;
		
		int position = 0;
		int delimiterLength = m_sDelimiter.Length();
		
		while (position < text.Length())
		{
			if (!isInQuotes && text.Substring(position, delimiterLength) == m_sDelimiter)
			{
				row.Insert(buffer.Trim());
				
				buffer = "";
				position += delimiterLength;
				
				continue;
			}
			
			currentChar = text.Get(position);
			
			if (currentChar == m_sQuoteCharacter)
				isInQuotes = !isInQuotes;
			else
				buffer += currentChar;
			
			position++;
		}
		
		if (!buffer.IsEmpty())
			row.Insert(buffer.Trim());
	}
	
	void Parse(string text, out array<string> row)
	{
		if (m_bIsMultiCharDelimiter)
		{
			ParseWithMultiCharDelimiter(text, row);
			return;
		}
		
		ParseWithSingleCharDelimiter(text, row);
	}
}