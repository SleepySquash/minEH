//
//  Parser.cpp
//  minEH
//
//  Created by Никита Исаенко on 31.01.2021.
//

#include "Parser.hpp"

#include <vector>
using std::cout;
using std::endl;

#include "../../Support/Utility.hpp"
#include "../../Support/ResourcePath.hpp"
#include "Stack.hpp"

namespace mh
{
    namespace nss
    {
#pragma mark -
#pragma mark UTF-8
        
        void Parser::put(const std::string& command) { line = command; lastPos = 0; }
        bool Parser::explicitNoMessage()
        {
            if (line.length() && line[line.length() - 1] == '!') {
                line.erase(line.begin() + (line.size() - 1)); return true; }
            return false;
        }
        
        bool Parser::starts(const std::string& command)
        {
            skipSpaces();
            bool reallyFound{ false };
            std::string commandLine = lowercase ? util::lowercase(line) : line;
            if (command.length() <= commandLine.length())
            {
                bool found{ true };
                int i = keepTheLastPos ? lastPos : 0;
                for (int j = 0; found && j < command.length(); i++, j++)
                    found = (commandLine[i] == command[j]);
                if (found) lastPos = i;
                reallyFound = found;
            }
            
            return reallyFound;
        }
        
        std::string Parser::string()
        {
            skipSpaces();
            std::string text; auto pos{ lastPos };
            bool quotes{ false }, found{ false };
            
            while (!found && pos < line.length())
            {
                if (line[pos] == ' ' && !quotes) found = true;
                else if (line[pos] == '"' && !quotes) quotes = true;
                else if (line[pos] == '"' && quotes) { quotes = false; found = true; }
                else text += line[pos];
                ++pos;
            }
            lastPos = pos;
            return text;
        }
        std::string Parser::word() { skipSpaces(); return until(' '); }
        std::string Parser::until(const char& u)
        {
            auto pos{ lastPos }; std::string text; bool found{ false };
            while (!found && pos < line.length())
                if (!(found = (line[pos] == u))) {
                    if (line[pos] != 13) text += line[pos]; ++pos; }
            lastPos = pos + 1; return text;
        }
        
        void Parser::skipSpaces() { while (lastPos < line.length() && (line[lastPos] == L' ' || line[lastPos] == L'\t')) lastPos++; }
        void Parser::removeSpaces() { while (line.length() > 0 && (line[0] == ' ' || line[0] == '\t')) line.erase(line.begin()); }
        
        
        std::string until(const std::string& line, const char& u)
        {
            auto pos{ 0 }; std::string text; bool found{ false };
            while (!found && pos < line.length())
                if (!(found = (line[pos] == u))) {
                    if (line[pos] != 13) text += line[pos]; ++pos; }
            return text;
        }
        
        
#pragma mark -
#pragma mark Wide
        
        void CommandSettingsW::Command(const std::wstring& command, bool skipSpaces)
        {
            //this->line = ns::base::LowercaseTheString(line);
            line = command; lastPos = 0;
            if (skipSpaces) RemoveSpaces(*this);
        }
        bool CommandSettingsW::ExplicitNoMessage()
        {
            if (line.length() && line[line.length() - 1] == L'!') {
                line.erase(line.begin() + (line.size() - 1)); return true; }
            return false;
        }
        
        bool Command(CommandSettingsW& results, const std::wstring& command)
        {
            bool ReallyFound{ false }; SkipSpaces(results);
            std::wstring commandLine = results.lowercaseCommand ? util::lowercase(results.line) : results.line;
            if (command.length() <= commandLine.length())
            {
                bool Found{ true };
                int i = results.keepTheLastPos ? results.lastPos : 0;
                for (int j = 0; Found && j < command.length(); i++, j++)
                    Found = (commandLine[i] == command[j]);
                if (Found) results.lastPos = i;
                ReallyFound = Found;
            }
            
            return ReallyFound;
        }
        bool Command(const std::wstring& line, const std::wstring& command, bool lowercase, bool skipSpaces)
        {
            bool ReallyFound{ false };
            
            int lastPos{ 0 };
            if (skipSpaces) while (lastPos < line.length() && (line[lastPos] == L' ' || line[lastPos] == L'\t')) ++lastPos;
            std::wstring commandLine = lowercase ? util::lowercase(line) : line;
            if (command.length() <= commandLine.length())
            {
                bool Found{ true };
                int i = lastPos;
                for (int j = 0; Found && j < command.length(); i++, j++)
                    Found = (commandLine[i] == command[j]);
                ReallyFound = Found;
            }
            
            return ReallyFound;
        }
        bool Command(const std::string& line, const std::string& command)
        {
            bool ReallyFound{ false };
            
            if (command.length() <= line.length())
            {
                bool Found{ true };
                for (int i = 0; Found && i < command.length(); i++)
                    Found = (line[i] == command[i]);
                
                ReallyFound = Found;
            }
            
            return ReallyFound;
        }
        
        //TODO: Documentation
        void SkipSpaces(CommandSettingsW& results)
        {
            while (results.lastPos < results.line.length() && (results.line[results.lastPos] == L' ' || results.line[results.lastPos] == L'\t'))
                results.lastPos++;
        }
        void RemoveSpaces(CommandSettingsW& results) {
            while (results.line.length() > 0 && (results.line[0] == L' ' || results.line[0] == L'\t')) results.line.erase(results.line.begin()); }
        void RemoveSpaces(std::wstring& line) { while (line.length() > 0 && (line[0] == L' ' || line[0] == L'\t')) line.erase(line.begin()); }
        bool ContainsUsefulInformation(CommandSettingsW& results)
        {
            for (int i = results.lastPos; i < results.line.length(); i++)
                if (results.line[i] != L' ' && results.line[i] != L'\t' && results.line[i] != L'\0' && results.line[i] != L'\n' && results.line[i] != 13)
                    return true;
            return false;
        }
        void Trim(std::wstring& line) { RemoveSpaces(line);
            while (line.length() > 0 && (line[line.length() - 1] == L' ' || line[line.length() - 1] == L'\t')) line.erase(line.begin() + line.length() - 1); }
        bool ContainsUsefulInformation(const std::wstring& wstr)
        {
            for (int i = 0; i < wstr.length(); i++)
                if (wstr[i] != L' ' && wstr[i] != L'\t' && wstr[i] != L'\0' && wstr[i] != L'\n' && wstr[i] != 13)
                    return true;
            return false;
        }
        
        
        
        //TODO: Documentation
        std::wstring ParseUntil(CommandSettingsW& results, const wchar_t until)
        {
            unsigned int pos{ results.lastPos };
            std::wstring text = L""; bool Found{ false };
            while (!Found && pos < results.line.length())
                if (!(Found = (results.line[pos] == until)))
                {
                    if (results.line[pos] != 13) text += results.line[pos];
                    ++pos;
                }
            results.lastPos = pos + 1;
            return text;
        }
        std::string ParseUntil(std::string line, const char until, unsigned int from)
        {
            std::string text = "";
            bool Found{ false };
            while (!Found && from < line.length())
                if (!(Found = (line[from] == until)))
                {
                    if (line[from] != 13) text += line[from];
                    ++from;
                }
            return text;
        }
        std::wstring ParseUntilReverse(std::wstring line, const wchar_t until, unsigned int from)
        {
            std::wstring text = L""; bool Found{ false };
            while (!Found && from >= 0)
            {
                if (!(Found = (line[from] == until))) { if (line[from] != 13) text += line[from]; --from; }
                if (from == 0) break;
            }
            return text;
        }
        
        
        
        //TODO: Documentation
        std::wstring ParseWhile(CommandSettingsW& results, const wchar_t until)
        {
            unsigned int pos{ results.lastPos };
            std::wstring text = L""; bool Found{ true };
            while (Found && pos < results.line.length())
                if ((Found = (results.line[pos] == until))) {
                    if (results.line[pos] != 13) text += results.line[pos]; pos++; }
            return text;
        }
        
        //TODO: Documentation
        std::wstring ParseAsQuoteString(CommandSettingsW& results)
        {
            SkipSpaces(results);
            
            unsigned int pos{ results.lastPos };
            std::wstring text = L"";
            bool QuotesInline{ false }, Found{ false };
            
            while (!Found && pos < results.line.length())
            {
                if (!(Found = (results.line[pos] == '"'))) text += results.line[pos];
                if (!QuotesInline && Found) { QuotesInline = true; Found = false; }
                ++pos;
            }
            if (Found) { results.lastPos = pos; return text; }
            else return L"";
        }
        std::wstring ParseAsMaybeQuoteString(CommandSettingsW& results)
        {
            nss::SkipSpaces(results);
            if (results.line[results.lastPos] == L'"') return nss::ParseAsQuoteString(results);
            else return nss::ParseUntil(results, L' ');
        }
        std::wstring ParseAsMaybeQuoteStringFull(CommandSettingsW& results)
        {
            nss::SkipSpaces(results);
            if (results.line[results.lastPos] == L'"') return nss::ParseAsQuoteString(results);
            else return nss::ParseUntil(results, L'\0');
        }
        
        int ParseAsInt(CommandSettingsW& results) { return atoi(nss::ParseUntil(results, ' ')); }
        long ParseAsLong(CommandSettingsW& results) { return atol(nss::ParseUntil(results, ' ')); }
        float ParseAsFloat(CommandSettingsW& results) { return atof(nss::ParseUntil(results, ' ')); }
        std::wstring ParseAsString(CommandSettingsW& results)
        {
            std::wstring parsedUntil = nss::ParseUntil(results, '\0');
            std::wstring stringValue = util::lowercase(parsedUntil);
            return stringValue;
        }
        std::wstring ParseAsStringWOLowercase(CommandSettingsW& results) { std::wstring parsedUntil = nss::ParseUntil(results, '\0'); return parsedUntil; }
        bool ParseAsBool(CommandSettingsW& results)
        {
            std::wstring stringValue = nss::ParseAsString(results);
            if (stringValue == L"true" || stringValue == L"1") return true;
            else return false;
        }
        
        
        glm::vec4 ParseColor(CommandSettingsW& results)
        {
            nss::SkipSpaces(results);
            std::wstring color1 = nss::ParseUntil(results, ' ');
            if (color1.length() != 0)
            {
                nss::SkipSpaces(results);
                std::wstring color2 = nss::ParseUntil(results, ' ');
                if (color2.length() != 0)
                {
                    nss::SkipSpaces(results);
                    std::wstring color3 = nss::ParseUntil(results, ' ');
                    if (color3.length() != 0)
                    {
                        uint8_t rColor = atoi(color1);
                        uint8_t gColor = atoi(color2);
                        uint8_t bColor = atoi(color3);
                        return uColor(rColor, gColor, bColor, 0);
                    }
                }
                else
                {
                    std::wstring guessColor = util::lowercase(color1);
                    if (guessColor == L"while") return uColor(255, 255, 255, 0);
                    else if (guessColor == L"black") return uColor(0, 0, 0, 0);
                    else if (guessColor == L"red") return glm::vec4(Color::Red.r, Color::Red.g, Color::Red.b, 0);
                    else if (guessColor == L"magenta") return glm::vec4(Color::Magenta.r, Color::Magenta.g, Color::Magenta.b, 0);
                    else if (guessColor == L"cyan") return glm::vec4(Color::Cyan.r, Color::Cyan.g, Color::Cyan.b, 0);
                    else if (guessColor == L"yellow") return glm::vec4(Color::Yellow.r, Color::Yellow.g, Color::Yellow.b, 0);
                }
            }
            
            return glm::vec4(0, 0, 0, 1.f);
        }
        int ParseAlpha(CommandSettingsW& results)
        {
            nss::SkipSpaces(results);
            
            float possibleValueAsFloat{ -1 };
            for (int i = results.lastPos; i < results.line.length() && results.line[i] != L' '; i++)
                if (results.line[i] == L'.')
                    possibleValueAsFloat = nss::ParseAsFloat(results);
            
            int possibleValue;
            if (possibleValueAsFloat < 0 || possibleValueAsFloat > 1)
            {
                possibleValue = nss::ParseAsInt(results);
                if (possibleValue >= 0 && possibleValue <= 255)
                    return possibleValue;
            }
            else
            {
                possibleValue = 255 * possibleValueAsFloat;
                return possibleValue;
            }
            
            return -1;
        }
        
        
        std::wstring ParseArgument(CommandSettingsW& results)
        {
            unsigned int pos{ results.lastPos };
            std::wstring text = L"";
            bool Found{ false };
            bool insideQuotes{ false };
            
            while (!Found && pos < results.line.length())
            {
                if (results.line[pos] == '"')
                    insideQuotes = !insideQuotes;
                if (!(Found = (results.line[pos] == ' ' && !insideQuotes)))
                {
                    if (results.line[pos] != 13 && results.line[pos] != ',')
                        text += results.line[pos];
                    pos++;
                }
            }
            results.lastPos = pos + 1;
            
            return text;
        }
        /*wchar_t** ParseArguments(CommandSettingsW& results)
         {
         wchar_t** arguments{ nullptr };
         int parsedArguments{ 0 };
         
         while (results.lastPos+1 < results.line.length())
         {
         SkipSpaces(results);
         std::wstring argument = nss::ParseArgument(results);
         
         if (argument != L"with" && argument != L"as")
         {
         arguments = (wchar_t**)realloc(arguments, sizeof(wchar_t*)*(parsedArguments + 1));
         arguments[parsedArguments] = (wchar_t*)malloc(sizeof(wchar_t)*(argument.length() + 1));
         
         for (int i = 0; i <= argument.length(); i++)
         arguments[parsedArguments][i] = argument[i];
         
         parsedArguments++;
         }
         }
         if (parsedArguments == 0)
         return nullptr;
         
         arguments = (wchar_t**)realloc(arguments, sizeof(wchar_t*)*(parsedArguments + 1));
         arguments[parsedArguments] = nullptr;
         
         return arguments;
         }*/
        void ParseArguments(CommandSettingsW& results, std::vector<std::wstring>& vec)
        {
            while (results.lastPos+1 < results.line.length())
            {
                SkipSpaces(results);
                std::wstring argument = nss::ParseArgument(results);
                if (argument != L"with" && argument != L"as")
                    vec.push_back(argument);
            }
        }
        std::wstring ArgumentAsString(CommandSettingsW& results)
        {
            std::wstring parsedUntil = nss::ParseUntil(results, '\0');
            std::wstring stringValue = util::lowercase(parsedUntil);
            return stringValue;
        }
        std::wstring ArgumentAsStringWOLowerCase(CommandSettingsW& results)
        {
            std::wstring stringValue = nss::ParseUntil(results, '\0');
            return stringValue;
        }
        bool ArgumentAsBool(CommandSettingsW& results)
        {
            std::wstring stringValue = nss::ArgumentAsString(results);
            if (stringValue == L"true" || stringValue == L"1")
                return true;
            else
                return false;
        }
        float ArgumentAsFloat(CommandSettingsW& results)
        {
            std::wstring stringValue = nss::ParseUntil(results, '\0');
            std::string parsingString = "";
            for (int i = 0; stringValue[i] != '\0'; i++)
                if (stringValue[i] == 46 || (stringValue[i] >= 48 && stringValue[i] <= 57))
                    parsingString += (char)stringValue[i];
            
            return std::atof(parsingString.c_str());
        }
        int ArgumentAsInt(CommandSettingsW& results)
        {
            std::wstring stringValue = nss::ParseUntil(results, '\0');
            std::string parsingString = "";
            for (int i = 0; stringValue[i] != '\0'; i++)
                if (stringValue[i] >= 48 && stringValue[i] <= 57)
                    parsingString += (char)stringValue[i];
            
            return std::atoi(parsingString.c_str());
        }
        
        //TODO: Documentation
        std::wstring GetFromUntil(const std::wstring& line, unsigned int pos, const wchar_t until)
        {
            std::wstring text = L"";
            bool Found{ false };
            
            while (!Found && pos < line.length())
            {
                if (!(Found = (line[pos] == until)))
                {
                    text += line[pos];
                    pos++;
                }
            }
            
            return text;
        }
        
        std::wstring GetPathWOResourcePath(const std::wstring& path)
        {
            /// TODO: if Android, then %external%, %internal%, %builtin% shortcuts.
            
            std::wstring respath = utf16(resourcePath());
            if (respath.length() == 0 || respath == L"" || !Command(path, respath, false)) return path;
            
            std::wstring result{ L"" };
            for (unsigned long i = respath.length(); i < path.length(); ++i) result += path[i];
            return result;
        }
        
        
        
        
        
        
        
        //TODO: ^ operation and support for sin, cos, max etc functions.
        float MathParser(const std::wstring& line)
        {
            bool operatorWasFound{ false };
            Stack<std::wstring> output;
            Stack<std::wstring> operators;
            
            size_t wordLength = 0;
            size_t wordBeg = 0;
            for (int i = 0; i <= line.length(); ++i)
            {
                if (line[i] == L'/' || line[i] == L'+' || (line[i] == L'-') || line[i] == L'*' ||
                    line[i] == L'(' || line[i] == L')' || line[i] == '\0')
                {
                    if (wordLength != 0)
                    {
                        std::wstring wstr = L"";
                        bool notANumber{ false };
                        for (size_t j = wordBeg; j < i; ++j)
                        {
                            if (line[j] != '\t' && line[j] != ' ')
                            {
                                if (!((line[j] >= 48 && line[j] <= 57) || line[j] == 46))
                                    notANumber = true;
                                wstr += line[j];
                            }
                        }
                        
                        if (wstr.length() != 0)
                        {
                            if (notANumber)
                            {
                                /*if (wstr == L"sin" || wstr == L"cos")
                                 operators.Push(wstr);
                                 else*/
                                cout << "Warning :: MathParser :: not an operator." << endl;
                            }
                            else
                            {
                                bool makeNegative = false;
                                if (operators.size == 1 && output.size == 0)
                                    if (operators[0][0] == '-') {
                                        makeNegative = true; operators.Pop(); }
                                
                                if (makeNegative) {
                                    wstr.insert(wstr.begin(), L'-'); makeNegative = false; }
                                output.Push(wstr);
                            }
                        }
                        wordLength = 0;
                    }
                    wordBeg = i + 1;
                    
                    if (line[i] != '\0')
                    {
                        std::wstring wstr = L"";
                        wstr += line[i];
                        
                        if (line[i] == L')')
                        {
                            while (!operators.IsEmpty() && operators[0] != L"(")
                                output.Push(operators.Pop());
                            
                            if (!operators.IsEmpty())
                                operators.Pop();
                        }
                        else if (line[i] != L'(')
                        {
                            operatorWasFound = true;
                            
                            bool lowerFound{ false };
                            while (!operators.IsEmpty() && !lowerFound)
                            {
                                std::wstring op = operators[0];
                                if ((line[i] == '+' && op == L"-") || (line[i] == '-' && op == L"-") ||
                                    (line[i] == '+' && op == L"+") || (line[i] == '-' && op == L"+"))
                                    output.Push(operators.Pop());
                                else if ((line[i] == '*' && op == L"/") || (line[i] == '/' && op == L"/") ||
                                         (line[i] == '*' && op == L"*") || (line[i] == '/' && op == L"*"))
                                    output.Push(operators.Pop());
                                else if ((line[i] == '+' && op == L"*") || (line[i] == '+' && op == L"/") ||
                                         (line[i] == '-' && op == L"*") || (line[i] == '-' && op == L"/"))
                                    output.Push(operators.Pop());
                                else
                                    lowerFound = true;
                            }
                        }
                        
                        if (line[i] != L')')
                            operators.Push(wstr);
                    }
                }
                else
                    wordLength++;
            }
            
            if (!operatorWasFound)
            {
                if (output.IsEmpty())
                    return 0;
                else
                    return atof(output[0]);
            }
            
            while (!operators.IsEmpty())
                output.Push(operators.Pop());
            
            /*cout << "Shunting-yard Algorithm's output: ";
             for (int j = output.size - 1; j >= 0; --j)
             cout << base::ConvertToUTF8(output[j]) << " ";
             cout << endl;*/
            
            Stack<std::wstring> values;
            int pos = output.size;
            while (pos >= 0)
            {
                std::wstring token = output[pos];
                bool notANumber{ false };
                bool possibleNegative{ false };
                for (int i = 0; i < token.length(); ++i)
                {
                    if (token[i] != ' ' && token[i] != '\t')
                    {
                        if (token[i] == '-')
                            possibleNegative = true;
                        else if (!((token[i] >= 48 && token[i] <= 57) || token[i] == 46))
                            notANumber = true;
                        else
                            possibleNegative = false;
                    }
                }
                if (possibleNegative)
                    notANumber = true;
                
                if (notANumber)
                {
                    if (token == L"+" || token == L"-" || token == L"*" || token == L"/")
                    {
                        if (!values.IsEmpty())
                        {
                            std::wstring roperand = values.Pop();
                            if (!values.IsEmpty())
                            {
                                std::wstring loperand = values.Pop();
                                float rop = atof(roperand);
                                float lop = atof(loperand);
                                
                                float result{ 0 };
                                if (token == L"+")
                                    result = lop + rop;
                                else if (token == L"-")
                                    result = lop - rop;
                                else if (token == L"*")
                                    result = lop * rop;
                                else if (token == L"/")
                                    result = lop / rop;
                                
                                values.Push(std::to_wstring(result));
                            }
                            else
                            {
                                cout << "Error :: MathParser :: Operation can't be done: '" << utf8(token) << "'." << endl;
                                values.Push(roperand);
                            }
                        }
                        else
                            cout << "Error :: MathParser :: Operation can't be done: '" << utf8(token) << "'." << endl;
                    }
                    else
                        cout << "Error :: MathParser :: Operation is not supported: '" << utf8(token) << "'." << endl;
                }
                else
                    values.Push(token);
                
                --pos;
            }
            
            if (!values.IsEmpty())
                return atof(values.Pop());
            
            return 0;
        }
        
    }

}
