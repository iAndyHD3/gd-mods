#include "includes.h"


#define MEMBERBYOFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
#define MBO MEMBERBYOFFSET


std::string thousandSeparator(int n)
{
	using namespace std;
    string ans = "";
  
    // Convert the given integer
    // to equivalent string
    string num = std::to_string(n);
  
    // Initialise count
    int count = 0;
  
    // Traverse the string in reverse
    for (int i = num.size() - 1;
         i >= 0; i--) {
        count++;
        ans.push_back(num[i]);
  
        // If three characters
        // are traversed
        if (count == 3) {
            ans.push_back('.');
            count = 0;
        }
    }
  
    // Reverse the string to get
    // the desired output
    reverse(ans.begin(), ans.end());
  
    // If the given string is
    // less than 1000
    if (ans.size() % 4 == 0) {
  
        // Remove ','
        ans.erase(ans.begin());
    }
  
    return ans;
}

std::string intToFormatString(int n, float& scaleMult) {
	
	
    if(n < 1000 || n > 100000000)
    return std::to_string(n);

    std::string str = thousandSeparator(n);
    str = str.substr(0, str.find(".") + 2);

    char sufix;

    if(n < 1000000) { sufix = 'K'; scaleMult = 1.1f; }
    else if(n < 100000000) { sufix = 'M'; scaleMult = 1.5f; }
	
	
	return str += sufix ;
}

/*
//TODO: click on the label to change between abbreviated and long string

struct CallBacks {
	
	void onLabel(CCLabelBMFont* label)
};
*/

void (__thiscall* LevelCell_loadCustomLevelCell)(CCLayer* self);
void __fastcall LevelCell_loadCustomLevelCellH(CCLayer* self, void*) {
	
	LevelCell_loadCustomLevelCell(self);

	auto layer = (CCLayer*)self->getChildren()->objectAtIndex(1);

	auto level = MBO(void*, self, 380);

	int likes = MBO(int, level, 624) - MBO(int, level, 628);
	int downloads = MBO(int, level, 488);
	

	bool checkDownload = downloads > 1000;
	bool checkLikes = likes > 1000;
	
	if(!checkDownload && !checkLikes)
		return;
	
	int count = layer->getChildrenCount();
	
	for(int i = 0; i < count; i++)
	{
		auto node = (CCNode*)layer->getChildren()->objectAtIndex(i);

		if(auto label = dynamic_cast<CCLabelBMFont*>(node))
		{
			if(checkDownload && atoi(label->CCLabelBMFont::getString()) == downloads)
			{
				float scaleMult = 1.0f;
				label->CCLabelBMFont::setString(intToFormatString(downloads, scaleMult).c_str());
				label->setScale(label->getScale() * scaleMult);
				label->setTag(downloads);
				continue;
			}
			else if(checkLikes && atoi(label->CCLabelBMFont::getString()) == likes)
			{
				float scaleMult = 1.0f;
				label->CCLabelBMFont::setString(intToFormatString(likes, scaleMult).c_str());
				label->setScale(label->getScale() * scaleMult);
				label->setTag(likes);
			}
		}
	}
}
	


DWORD WINAPI thread_func(void* hModule) {
	
	MH_Initialize();

	auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));
	
	MH_CreateHook(
		reinterpret_cast<void*>(base + 0x5A020),
		reinterpret_cast<void*>(&LevelCell_loadCustomLevelCellH),
		reinterpret_cast<void**>(&LevelCell_loadCustomLevelCell) 
	);
	
	
	MH_EnableHook(MH_ALL_HOOKS);

	return 0;
}


BOOL APIENTRY DllMain(HMODULE handle, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		auto h = CreateThread(0, 0, thread_func, handle, 0, 0);
		if (h)
			CloseHandle(h);
		else
			return FALSE;
	}
	return TRUE;
}