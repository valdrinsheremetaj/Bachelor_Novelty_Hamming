#ifndef CLOSED_LIST_H
#define CLOSED_LIST_H

#include <unordered_set>
#include "../state_registry.h"
#include "../state_partial.h"
#include "width_type.h"


struct Closed {
    std::vector<State> closed;                  
    std::vector<PartialState> partial_closed;   

    void clear() {
        closed.clear();
        partial_closed.clear();
    }

    // Hamming
    bool is_closed(const State &s) const {
        return std::find(closed.begin(), closed.end(), s) != closed.end();
    }
    void mark_closed(const State &s) {
        if (!is_closed(s))
            closed.push_back(s);
    }

    // Novelty
    bool is_closed(const PartialState &ps) const {
        return std::find(partial_closed.begin(), partial_closed.end(), ps) != partial_closed.end();
    }
    void mark_closed(const PartialState &ps) {
        if (!is_closed(ps))
            partial_closed.push_back(ps);
    }
};

    
    #endif