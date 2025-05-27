#include <unordered_set>
#include "../state_registry.h"

// Create partial states for Novelty
/*Closed list created for Novelty and Hamming Width Search based on Simon Dold's framework*/
class ClosedList {
private:
    std::vector<State> closed; // used for Hamming width search
    //std::vector<PartialState> partial_closed; // used for Novelty width search later
public:
                        // ==== Hamming Width ====

    // Check whether the given state is already in the closed list.
    bool is_closed(const State& s) const {
        return std::find(closed.begin(), closed.end(), s) != closed.end();
    }
    // Add the given state to the closed list.
    void mark_closed(const State& s) {
        if (!is_closed(s))
            closed.push_back(s);

    }
    // Clear the closed list.
    void clear() {
        closed.clear();
    }
    // Get the number of states in the closed list.
    size_t size() const {
        return closed.size();
    }
};
