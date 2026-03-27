#ifndef TITLEGENERATORS_H
#define TITLEGENERATORS_H

#include <sodium.h>
#include <ctime>
#include<string>
#include <iomanip>
#include <sstream>

namespace TitleGenerator{
    inline std::string generatorEntryTitle(const std::string& content){
        std::time_t time = std::time(nullptr);
        std::stringstream ss;
        ss<<std::put_time(std::localtime(&time),"%d %B %H:%M");
        return ss.str()+" - "+content;
    }

    inline std::string generatorJournalTitle(){
        const char* states[] = {"Quiet", "Midnight", "Restless", "Fading", "Still", "Wandering", "Unspoken", "Distant", "Hushed", "Forgotten", "Hidden", "Veiled", "Solitary", "Drifting", "Fleeting", "Fragmented", "Boundless", "Echoing", "Silent", "Obscured"};
        const char* nouns[] = {"Reverie", "Fragment", "Echo", "Thought", "Canvas", "Memory", "Solitude", "Haze", "Ember", "Whisper", "Vesper", "Monologue", "Refuge", "Sanctuary", "Trace", "Illusion", "Paradox", "Resonance", "Abyss", "Elegy"};

        uint32_t s = randombytes_uniform(sizeof(states)/sizeof(states[0]));
        uint32_t n = randombytes_uniform(sizeof(nouns)/sizeof(nouns[0]));

        return std::string(states[s])+' '+std::string(nouns[n]);
    }
}

#endif // TITLEGENERATORS_H
