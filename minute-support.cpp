#include <matdash.hpp>
#include <matdash/minhook.hpp>
#include <matdash/boilerplate.hpp>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <gd.h>

#include "stdio.h"
#include <string_view>
#include <iomanip>

using namespace gd;
using namespace cocos2d;

//#define USE_WIN32_CONSOLE
#define MEMBERBYOFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
#define MBO MEMBERBYOFFSET
//#define FMTLOG(format, ...) std::cout << fmt::format(format, __VA__ARGS)


template <typename... T>
	void FMTLOG(fmt::format_string<T...> fmt, T&&... args) {
	std::cout << fmt::format("[{:%H:%M:%S}] {}\n", fmt::gmtime(std::time(NULL)), fmt::format(fmt, std::forward<T>(args)...));
}


std::vector<std::string_view> splitByDelimStringView(std::string_view str, char delim)
{
    std::vector<std::string_view> tokens;
    size_t pos = 0;
    size_t len = str.length();

    while (pos < len) {
        size_t end = str.find(delim, pos);
        if (end == std::string_view::npos) {
            tokens.emplace_back(str.substr(pos));
            break;
        }
        tokens.emplace_back(str.substr(pos, end - pos));
        pos = end + 1;
    }

    return tokens;
}

int _stoi(const std::string_view s) {
	if(s.empty())
		return 0;
	
	int ret = 0;
	std::from_chars(s.data(),s.data() + s.size(), ret);
	return ret;
}


std::string doubleToMinuteString(std::string_view doubleStr) {
    if(doubleStr.find('.') == std::string_view::npos)
    {
        int seconds = _stoi(doubleStr);
		
		if(seconds <= 60)
			return {};
		
        return fmt::format("{:%M:%S}", std::chrono::seconds(seconds));
    }

    auto split = splitByDelimStringView(doubleStr, '.');
    int seconds = _stoi(split[0]);
	
	if(seconds <= 60)
		return {};
	
    int decimals = _stoi(split[1]);
    //std::cout << decimals;
    return decimals == 0 ? fmt::format("{:%M:%S}", std::chrono::seconds(seconds)) : fmt::format("{:%M:%S}.{}", std::chrono::seconds(seconds), decimals);
}

std::string minuteStringToDouble(std::string_view str)
{
	if(str.find(':') == std::string::npos || str.back() == ':')
		return {};
	
    auto split = splitByDelimStringView(str, ':');
	std::string_view mins = split[0];
	std::string_view seconds = split[1];
	int secondsAll = _stoi(mins) * 60 + _stoi(seconds);
	if(auto point = seconds.find('.'); point != std::string_view::npos)
	{
		std::string_view decimals = seconds.substr(point + 1);
		return fmt::format("{}.{}", secondsAll, decimals);
	}
	
	return fmt::format("{}", secondsAll);
}

bool SongOptionsLayer_init(void* self, void* levelSettings) {
	if(!matdash::orig<&SongOptionsLayer_init>(self, levelSettings))
		return false;
	
	auto secondsInput = MBO(CCTextInputNode*, self, 464);
	secondsInput->setAllowedChars("0123456789.:");
	secondsInput->setMaxLabelLength(7);
	
	const char* str = secondsInput->getString();
	if(std::string_view(str).empty())
		return true;
	
	std::string newstr = doubleToMinuteString(str);
	
	if(!newstr.empty()) {
		secondsInput->setString(newstr.c_str());
	}
	//FMTLOG("minutstr: {}", minuteString);	
	return true;
}

void SongOptionsLayer_onClose(void* self, void* btn)
{
	auto secondsInput = MBO(CCTextInputNode*, self, 464);
	const char* str = secondsInput->getString();
	
	if(std::string_view(str).empty())
		return matdash::orig<SongOptionsLayer_onClose>(self, btn);
	
	std::string newstr = minuteStringToDouble(str);
	
	if(!newstr.empty()) {
		secondsInput->setString(newstr.c_str());
	}
	
	matdash::orig<SongOptionsLayer_onClose>(self, btn);
}


//terrible solution lmao
void SongOptionsLayer_onPlayback(void* self, void* btn)
{
	if(FMODAudioEngine::sharedEngine()->isBackgroundMusicPlaying())
		return matdash::orig<SongOptionsLayer_onPlayback>(self, btn);
	
	auto secondsInput = MBO(CCTextInputNode*, self, 464);
	std::string original = secondsInput->getString();
	std::string_view str(original);
	
	if(str.empty() || str.find(':') == std::string_view::npos)
		return matdash::orig<SongOptionsLayer_onPlayback>(self, btn);
	
	std::string newstr = minuteStringToDouble(original);
	
	if(!newstr.empty()) {
		secondsInput->setString(newstr.c_str());
		matdash::orig<SongOptionsLayer_onPlayback>(self, btn);
		return secondsInput->setString(original.c_str());
	}
	matdash::orig<SongOptionsLayer_onPlayback>(self, btn);
}

void mod_main(HMODULE) {
	
	#ifdef USE_WIN32_CONSOLE
		if(AllocConsole()) {
			freopen("CONOUT$", "wt", stdout);
			freopen("CONIN$", "rt", stdin);
			freopen("CONOUT$", "w", stderr);
			std::ios::sync_with_stdio(1);
		}
	#endif

	matdash::add_hook<&SongOptionsLayer_init>(base + 0x67520);
	matdash::add_hook<&SongOptionsLayer_onClose>(base + 0x681e0);
	matdash::add_hook<&SongOptionsLayer_onPlayback>(base + 0x67e20);
}