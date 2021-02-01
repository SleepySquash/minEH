//
//  Utility.hpp
//  minEH
//
//  Created by Никита Исаенко on 31.01.2021.
//

#pragma once

#include <iostream>

namespace mh
{
    namespace util
    {
        std::wstring getCurrentWorkingDir();
        bool fileExists(const std::wstring& path);
        bool createDirectory(const std::wstring& path);
        
        bool doesFileExistWithResolutionClass(const std::wstring& currentPath);
        
        std::wstring getFolderPath(const std::wstring& path);
        std::wstring getExtentionFromString(const std::wstring& filename);
        std::wstring getStringWithNoExtention(const std::wstring& filename);
        std::wstring getFilenameWOProhibited(const std::wstring& filename);
        
        std::string getFolderPath(const std::string& path);
        std::string getPathWOResourcePath(const std::string& path);
        
        size_t getLengthWONewLinesAndSpaces(const std::wstring& string);
        
        std::wstring lowercase(const std::wstring& str);
        std::string lowercase(const std::string& str);
        
        double atan(double x);
        
        //////////////////////////////////////////////////////////////////////////////
        /// This code below belongs to https://stackoverflow.com/users/592323/leemes
        //////////////////////////////////////////////////////////////////////////////
        template<typename It> class Range
        {
            It b, e;
        public:
            Range(It b, It e) : b(b), e(e) {}
            It begin() const { return b; }
            It end() const { return e; }
        };
        template<typename ORange, typename OIt = decltype(std::begin(std::declval<ORange>())), typename It = std::reverse_iterator<OIt>>
        Range<It> reverse(ORange && originalRange) { return Range<It>(It(std::end(originalRange)), It(std::begin(originalRange))); }
    }
    
    int atoi(const std::wstring& stringValue);
    long atol(const std::wstring& stringValue);
    float atof(const std::wstring& stringValue);
    bool atob(const std::wstring& stringValue);
    
    // Taken from http://tinodidriksen.com/uploads/code/cpp/speed-string-to-int.cpp
    int fatoi(const char *p);
    int fatoui(const char *p);
    int fatoin(const char *p);
    int fatouin(const char *p);
}
