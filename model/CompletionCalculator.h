#ifndef DCHARVAULT_COMPLETIONCALCULATOR_H
#define DCHARVAULT_COMPLETIONCALCULATOR_H

#include<QString>

struct CompletionCalculator {
    int completed = 0;
    int total = 0;

    double precentage() const {
        return (total>0) ? (static_cast<double>(completed)*100/total) : 0.0;
    }

    enum class State {
        NoTracking,
        InProgress,
        Complete
    };

    State state() const {
        if (total==0) return State::NoTracking;
        if (completed==total) return State::Complete;
        return State::InProgress;
    }
};

class CompletionCalculator {
public:
    static CompletionResult calculate(const QString& richTextHtml);
};

#endif //DCHARVAULT_COMPLETIONCALCULATOR_H
