#include <Logging/pch.h>
#include <Logging/Writer.h>

namespace
{
    const char* tokenStrings[] =
    {
        "DTI",
        "STI",
        "CHN",
        "MID",
        "SEV",
        "MNM",
        "CLK",
        "MSG",
    };
    
    Logging::Token StringToToken(const std::string& str)
    {
        std::string upperCase;
        std::transform(str.begin(), str.end(), std::back_inserter(upperCase), ::toupper);
        
        size_t len = sizeof(tokenStrings) / sizeof(char*);
        
        for (int i = 0; i < len; i++) {
            if (tokenStrings[i] == str)
            {
                return static_cast<Logging::Token>(i);
            }
        }
        return Logging::Token::STR;
    }
}

namespace Logging
{
    WriterBase::WriterBase(std::string format, std::string timeFormat)
        : mFormat(std::move(format))
        , mTimeFormat(std::move(timeFormat))
    {
        ParseTokens(mFormat);
    }
    
    void WriterBase::SetMinSeverity(const Severity severity)
    {
        mMinSeverity = severity;
    }
    
    const Severity WriterBase::GetMinSeverity() const
    {
        return mMinSeverity;
    }
    
    std::string WriterBase::FormatMessage(const std::string& msg, const uint32_t channel, const Severity severity, const std::string& component)
    {
        auto its = mFormatStrings.begin();
        
        std::stringstream stream;
        for (auto t : mTokens)
        {
            switch (t)
            {
                case Logging::Token::DTI:
                    WriteTimeStamp(stream);
                    break;
                case Logging::Token::STI:
                    //TODO:
                    break;
                case Logging::Token::CHN:
                    stream << channel;
                    break;
                case Logging::Token::MID:
                    WriteModuleId(stream, channel);
                    break;
                case Logging::Token::SEV:
                    WriteSeverity(stream, severity);
                    break;
                case Logging::Token::MNM:
                    stream << component;
                    break;
                case Logging::Token::CLK:
                    //TODO:
                    break;
                case Logging::Token::MSG:
                    stream << msg;
                    break;
                case Logging::Token::STR:
                    if (its != mFormatStrings.end())
                    {
                        stream << *its;
                        its++;
                    }
                    break;
                default:;
            }
        }
        
        stream.flush();
        return stream.str();
    }
    
    void WriterBase::WriteSeverity(std::ostream& stream, const Severity severity)
    {
        stream << ToString(severity);
    }
    
    void WriterBase::WriteModuleId(std::ostream& stream, const uint32_t moduleId)
    {
        if (moduleId == 0)
        {
            stream << "MAIN";
        }
        else
        {
            char mcp[5];
            ModuleId mid;
            mid.uid = moduleId;
            mcp[0] = mid.cid.b0;
            mcp[1] = mid.cid.b1;
            mcp[2] = mid.cid.b2;
            mcp[3] = mid.cid.b3;
            mcp[4] = '\0';
            stream << mcp;
        }
    }
    
    void WriterBase::WriteTimeStamp(std::ostream& stream)
    {
        time_t now = time(NULL);
        char   buff[32];
        
        strftime(buff, sizeof(buff), mTimeFormat.c_str(), localtime(&now));
        stream << buff;
    }
    
    void WriterBase::ParseTokens(const std::string& format)
    {
        int32_t tkn = -1;
        std::string tknStr, stuffStr;
        
        for (const char& ch : format)
        {
            if ('\\' == ch)
            {
                continue;
            }
            else if (tkn > -1)
            {
                tknStr += ch;
                if (++tkn > 2)
                {
                    tkn = -1;
                    Token t = StringToToken(tknStr);
                    if (t == Logging::Token::STR)
                    {
                        mFormatStrings.push_back(tknStr);
                    }
                    mTokens.push_back(t);
                    tknStr.clear();
                }
            }
            else if ('%' == ch)
            {
                tkn = 0;

                if (!stuffStr.empty())
                {
                    mFormatStrings.push_back(stuffStr);
                    stuffStr.clear();
                    mTokens.push_back(Logging::Token::STR);
                }
            }
            else
            {
                stuffStr += ch;
            }
        }
        
        mTokens.push_back(Logging::Token::STR);
        mFormatStrings.push_back("\n");
    }
}
