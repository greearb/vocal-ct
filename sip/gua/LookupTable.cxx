#include "LookupTable.hxx"
#include <stdio.h>
#include "cpLog.h"

LookupTable* LookupTable::myInstance = 0;

LookupTable::LookupTable(const char* filename)
{
    // read from a file
    parseFile(filename);


}

void
LookupTable::parseFile(const char* filename)
{
    char line [256];

    FILE* fd = fopen (filename , "r");

    if (fd)
    {
	while(fgets(line, 256, fd))
	{
	    // do something to the line.  separate on comma
	    Data name;
	    Data wav;
	    int c = 0;
	    int state = 0;
	    line[255] = '\0';
	    
	    while (c < 256 && line[c] != '\0')
	    {
		if (line[c] == '\r' || line[c] == '\n')
		{
		}
		else if (line[c] == '#')
		{
		    // ignore the rest of the line
		    state = 3;
		} 
		else if (line[c] == ',')
		{
		    state++;
		} 
		else if(state == 0)
		{
		    name += line[c];
		} 
		else if (state == 1)
		{
		    wav += line[c]; 
		}
		c++;
	    }
	    // cpLog(LOG_DEBUG, "state: %d, got name: %s", name.logData());

	    if(state >= 1 && name != "")
	    {
		cpLog(LOG_DEBUG, "got name: %s", name.logData());
		LookupTableItem item;
		item.wav = wav;
		myMap[name] = item;
	    }
        }
        fclose (fd);
    }
    else
    {
        cpLog (LOG_ERR, "Cannot open file: %s", filename);
    }
}


LookupTable* 
LookupTable::instance(const char* filename)
{
    if(LookupTable::myInstance == 0)
    {
	myInstance = new LookupTable(filename);
    }
    return myInstance;
}

bool
LookupTable::lookup(Data name, LookupTableItem& item)
{
    map<Data,LookupTableItem>::iterator i;
    i = myMap.find(name);
    if(i != myMap.end())
    {
	item = i->second;
	return true;
    }
    else
    {
	return false;
    }
}
