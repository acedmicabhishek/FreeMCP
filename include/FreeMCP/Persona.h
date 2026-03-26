#ifndef FREEMCP_PERSONA_H
#define FREEMCP_PERSONA_H

#include <string>

namespace Persona {
    enum class Mode {
        CHILL,
        PRODUCTIVE,
        HACKERMODE
    };

    enum class Character {
        LIGHT,
        ACE,
        JARVIS,
        FRIDAY,
        PARTNER,
        COOL_FRIEND
    };

    struct Identity {
        std::string name;
        std::string description;
        std::string tone;
        std::string constraints;
    };

    Identity get_persona();
    void set_mode(Mode mode);
    Mode get_mode();
    void set_character(Character ch);
    Character get_character();
    std::string get_prompt_fragment();
}

#endif
