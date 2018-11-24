#pragma once

namespace Logging
{
    enum class Token
    {
        DTI, //timestamp
        STI, //short timestamp
        CHN, //channel as int
        MID, //channel as moduleId
        SEV, //severity
        MNM, //module name
        CLK, //clock in ms elapsed from starting
        MSG, //message itself
        STR
    };
}
