//
//  Parser.hpp
//  minEH
//
//  Created by Никита Исаенко on 31.01.2021.
//

#pragma once

#include "../../Support/utf.hpp"
#include "../Color.hpp"

#include <iostream>

// TODO: should create my own mh::String class which will be std::string on UNIX and std::wstring on Windows.
namespace mh
{
    namespace nss
    {
        struct Parser
        {
            std::string line;
            unsigned int lastPos{ 0 };
            bool keepTheLastPos{ true };
            bool lowercase{ true };
            
            void put(const std::string& line);
            bool explicitNoMessage();
             
            bool starts(const std::string& command); // Returns true if 'line' starts with 'command'
            std::string string();  // Returns: 1) string 2) "string with spaces"
            std::string word(); // Returns word (until space)
            std::string until(const char& u); // Returns everything until 'u'
            
            void removeSpaces(); // Removes spaces in front
            void skipSpaces(); // Skips spaces at current position
        };
        
        std::string until(const std::string& line, const char& u); // Returns everything until 'u'
        
        struct CommandSettingsW
        {
            std::wstring line{ L"" };
            unsigned int lastPos{ 0 };
            bool keepTheLastPos{ true };
            bool lowercaseCommand{ true };
            
            void Command(const std::wstring& command, bool skipSpaces = false);
            bool ExplicitNoMessage();
        };
        
        bool Command(CommandSettingsW& results, const std::wstring& command);
        bool Command(const std::wstring& line, const std::wstring& command, bool lowercase = true, bool skipSpaces = true);
        bool Command(const std::string& line, const std::string& command);
        
        void SkipSpaces(CommandSettingsW& results);
        void RemoveSpaces(CommandSettingsW& results);
        void RemoveSpaces(std::wstring& line);
        void Trim(std::wstring& line);
        bool ContainsUsefulInformation(CommandSettingsW& results);
        bool ContainsUsefulInformation(const std::wstring& wstr);
        
        std::wstring GetFromUntil(const std::wstring& line, unsigned int pos, const wchar_t until);
        
        std::wstring ParseUntil(CommandSettingsW& results, const wchar_t until);
        std::string ParseUntil(std::string line, const char until, unsigned int from = 0);
        std::wstring ParseUntilReverse(std::wstring line, const wchar_t until, unsigned int from = 0);
        std::wstring ParseWhile(CommandSettingsW& results, const wchar_t until);
        std::wstring ParseAsQuoteString(CommandSettingsW& results);
        std::wstring ParseAsMaybeQuoteString(CommandSettingsW& results);
        std::wstring ParseAsMaybeQuoteStringFull(CommandSettingsW& results);
        int ParseAsInt(CommandSettingsW& results);
        long ParseAsLong(CommandSettingsW& results);
        float ParseAsFloat(CommandSettingsW& results);
        std::wstring ParseAsString(CommandSettingsW& results);
        std::wstring ParseAsStringWOLowercase(CommandSettingsW& results);
        bool ParseAsBool(CommandSettingsW& results);
        
        glm::vec4 ParseColor(CommandSettingsW& results);
        int ParseAlpha(CommandSettingsW& results);
        
        std::wstring ParseArgument(CommandSettingsW& results);
        void ParseArguments(CommandSettingsW& results, std::vector<std::wstring>& vec);
        std::wstring ArgumentAsString(CommandSettingsW& results);
        std::wstring ArgumentAsStringWOLowerCase(CommandSettingsW& results);
        bool ArgumentAsBool(CommandSettingsW& results);
        float ArgumentAsFloat(CommandSettingsW& results);
        int ArgumentAsInt(CommandSettingsW& results);
        
        // ...
        
        std::wstring GetPathWOResourcePath(const std::wstring& path);
        
        float MathParser(const std::wstring& finalLine);
    }
}
