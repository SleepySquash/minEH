//
//  Modes.hpp
//  minEH
//
//  Created by Никита Исаенко on 01.02.2021.
//

#pragma once

namespace mh
{
    enum class Mode { Appear, Disappear, Exist, Deprecate,  WaitingForTime, WaitingForInput, WaitingForChoose };
    enum class MessageBack { No, AtAppearance, AtDisappearance, AtDeprecated };
    enum class Position { Custom, Left, CLeft, Center, CRight, Right };
}
