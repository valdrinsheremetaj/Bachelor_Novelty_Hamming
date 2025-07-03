#!/usr/bin/env python3

import sys
import os
import itertools
import custom_parser
import project

if "--validate" in sys.argv:
    sys.argv.remove("--validate")

REPO = project.get_repo_base()
BENCHMARKS_DIR = os.environ["DOWNWARD_BENCHMARKS"]
SCP_LOGIN = "sherem0000@login12.scicore.unibas.ch"
REMOTE_REPOS_DIR = "/infai/sherem0000/projects"
# If REVISION_CACHE is None, the default "./data/revision-cache/" is used.
REVISION_CACHE = os.environ.get("DOWNWARD_REVISION_CACHE")
print(f"Using revision cache: {REVISION_CACHE}")
if not project.REMOTE:
    SUITE = project.SUITE_OPTIMAL_STRIPS
    ENV = project.BaselSlurmEnvironment(email="my.name@myhost.ch")
else:
    SUITE = [
   

    
    "movie:prob01.pddl",
    

]


    ENV = project.LocalEnvironment(processes=2)


CONFIGS = [
    ("fw-hw1", ["--search", 'framework(open=single(eval=blind()), Widthtype=0, width_k=1)']),
    ("fw-nw1", ["--search", 'framework(open=single(eval=blind()), Widthtype=1, width_k=1)']),
    ("fw-hybrid1", ["--search", 'framework(open=single(eval=blind()), Widthtype=2, width_k=1)']),
    ("fw-or1", ["--search", 'framework(open=single(eval=blind()), Widthtype=3, width_k=1)']),
]




BUILD_OPTIONS = []
DRIVER_OPTIONS = ["--overall-time-limit", "30m"]
DRIVER_OPTIONS = [opt for opt in DRIVER_OPTIONS if opt != "--validate"]

REV_NICKS = [
    ("hw-vs-nw", ""),
]
ATTRIBUTES = [
    "error",
    "run_dir",
    "search_start_time",
    "search_start_memory",
    "total_time",
    "coverage",
    "expansions",
    "evaluations",   
    "generated",
    "memory",
    "width_k",
    project.EVALUATIONS_PER_TIME,
]

exp = project.FastDownwardExperiment(environment=ENV, revision_cache=REVISION_CACHE)
for config_nick, config in CONFIGS:
    for rev, rev_nick in REV_NICKS:
        algo_name = f"{rev_nick}:{config_nick}" if rev_nick else config_nick
        exp.add_algorithm(
            algo_name,
            REPO,
            rev,
            config,
            build_options=BUILD_OPTIONS,
            driver_options=DRIVER_OPTIONS,
        )
exp.add_suite(BENCHMARKS_DIR, SUITE)

exp.add_parser(exp.EXITCODE_PARSER)
exp.add_parser(exp.TRANSLATOR_PARSER)
exp.add_parser(exp.SINGLE_SEARCH_PARSER)
exp.add_parser(custom_parser.get_parser())
exp.add_parser(exp.PLANNER_PARSER)

exp.add_step("build", exp.build)
exp.add_step("start", exp.start_runs)
exp.add_step("parse", exp.parse)
exp.add_fetcher(name="fetch")

if not project.REMOTE:
    project.add_scp_step(exp, SCP_LOGIN, REMOTE_REPOS_DIR)

project.add_absolute_report(
    exp, attributes=ATTRIBUTES, filter=[project.add_evaluations_per_time]
)

attributes = ["expansions"]
pairs = [
    ("fw-hw1", "fw-nw1"),
    ("fw-hw1", "fw-hybrid1"),
    ("fw-nw1", "fw-hybrid1"),
    ("fw-hw1", "fw-or1"),
]



suffix = "-rel" if project.RELATIVE else ""
for algo1, algo2 in pairs:
    for attr in attributes:
        exp.add_report(
            project.ScatterPlotReport(
                relative=project.RELATIVE,
                get_category=None if project.TEX else lambda run1, run2: run1["domain"],
                attributes=[attr],
                filter_algorithm=[algo1, algo2],
                filter=[project.add_evaluations_per_time],
                format="tex" if project.TEX else "png",
            ),
            name=f"{exp.name}-{algo1}-vs-{algo2}-{attr}{suffix}",
        )

#project.add_compress_exp_dir_step(exp)

exp.run_steps()
