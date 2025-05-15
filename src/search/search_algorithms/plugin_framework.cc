#include "framework.h"
#include "search_common.h"

#include "../plugins/plugin.h"

using namespace std;

namespace plugin_framework {
class FrameworkFeature
    : public plugins::TypedFeature<SearchAlgorithm, framework::Framework> {
public:
    FrameworkFeature() : TypedFeature("framework") {
        document_title("Eager best-first search");
        document_synopsis("");

        add_option<shared_ptr<OpenListFactory>>("open", "open list");
        add_option<bool>(
            "reopen_closed",
            "reopen closed nodes",
            "false");
        add_option<shared_ptr<Evaluator>>(
            "f_eval",
            "set evaluator for jump statistics. "
            "(Optional; if no evaluator is used, jump statistics will not be displayed.)",
            plugins::ArgumentInfo::NO_DEFAULT);
        add_list_option<shared_ptr<Evaluator>>(
            "preferred",
            "use preferred operators of these evaluators",
            "[]");
        framework::add_eager_search_options_to_feature(
            *this, "framework");
    }

    virtual shared_ptr<framework::Framework>
    create_component(const plugins::Options &opts) const override {
        return plugins::make_shared_from_arg_tuples<framework::Framework>(
            opts.get<shared_ptr<OpenListFactory>>("open"),
            opts.get<bool>("reopen_closed"),
            opts.get<shared_ptr<Evaluator>>("f_eval", nullptr),
            opts.get_list<shared_ptr<Evaluator>>("preferred"),
            framework::get_eager_search_arguments_from_options(opts)
        );
    }
};

static plugins::FeaturePlugin<FrameworkFeature> _plugin;

}
