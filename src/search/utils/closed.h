#ifndef CLOSED_LIST_H
#define CLOSED_LIST_H

#include <unordered_set>
#include "../state_registry.h"
#include "../state_partial.h"
#include "width_type.h"


class Closed {
    private:
        Widthtype width_type;
        std::vector<State> closed;              // for Hamming width
        std::vector<PartialState> partial_closed; // for Novelty width
    
    public:
        explicit Closed(Widthtype wt)
            : width_type(wt) {}
    
                            // ==== Common Interface ====
    
        void clear() {
            closed.clear();
            partial_closed.clear();
        }
    
        size_t size() const {
            if (width_type == Widthtype::Hamming)
                return closed.size();
            else if (width_type == Widthtype::Novelty)
                return partial_closed.size();
            return 0;
        }
    
                            // ==== Hamming Width ====
    
        bool is_closed(const State &s) const {
            return width_type == Widthtype::Hamming &&
                   std::find(closed.begin(), closed.end(), s) != closed.end();
        }
    
        void mark_closed(const State &s) {
            if (width_type == Widthtype::Hamming && !is_closed(s)) {
                closed.push_back(s);
            }
        }
    
                            // ==== Novelty Width ====
    
        bool is_closed(const PartialState &ps) const {
            return width_type == Widthtype::Novelty &&
                   std::find(partial_closed.begin(), partial_closed.end(), ps) != partial_closed.end();
        }
    
        void mark_closed(const PartialState &ps) {
            if (width_type == Widthtype::Novelty && !is_closed(ps)) {
                partial_closed.push_back(ps);
            }
        }
    };
    
    #endif