from pathlib import Path

import project

from lab.experiment import Experiment

ATTRIBUTES = [
    "error",
    "expansions",
    "run_dir",
    "planner_time",
    "coverage",
    "cost",
    "evaluations",
    "generated",
    "memory",
    "width_k",
    project.EVALUATIONS_PER_TIME,
]


exp = Experiment()
exp.add_step(
    "remove-combined-properties", project.remove_file, Path(exp.eval_dir) / "properties"
)

project.fetch_algorithms(
    exp,
    "fw-hw1",
    "fw-nw1",
    "fw-hybrid1",
    "fw-or1",
)

filters = [project.add_evaluations_per_time]

project.add_absolute_report(
    exp, attributes=ATTRIBUTES, filter=filters, name=f"{exp.name}"
)


exp.run_steps()
