
#include "Data.hxx"
#include <map>

struct LookupTableItem
{
	Data wav;
};

class LookupTable;

class LookupTable
{
    public:
	static LookupTable* instance(const char* filename);
	bool lookup(Data name, LookupTableItem& item);

    protected:
	LookupTable(const char* filename);
	void parseFile(const char* filename);

    private:
	static LookupTable* myInstance;
	map <Data, LookupTableItem> myMap; 



};
