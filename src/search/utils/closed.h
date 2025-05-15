#include <unordered_set>
#include "../state_registry.h"

/*Closed list created for Novelty and Hamming Width Search based on Simon Dold's framework*/
class ClosedList {
private:
    std::unordered_set<State> closed;

public:
    // Check whether the given state is already in the closed list.
    bool is_closed(const State& s) const {
        return closed.count(s);
    }
    // Add the given state to the closed list.
    void mark_closed(const State& s) {
        closed.insert(s);
    }
    // Clear the closed list.
    // This function is used to clear the closed list when a new search is started.
    void clear() {
        closed.clear();
    }
    // Get the number of states in the closed list.
    size_t size() const {
        return closed.size();
    }
};
