# zonal-filter

Evaluation and development repository for research into performant authentication and packet filtering in Automotive Ethernet zonal networks.

## Quick Tour

This repository is a single module designed for use within the [ns-3](https://www.nsnam.org/)
network simulator.

Experiments can each be found in the [`examples/`](examples/) directory with supporting code 
in the [`helper/`](helper/) and [`model/`](model/) directories.

Results can be found in the [`results/`](results/) directory for each experiment.
For example, you can view the results of the first Zonal Latency Measurement
experiment by viewing 
[`results/zonal-latency-measurement/stream.csv`](results/zonal-latency-measurement/stream.csv).

For more detail, please see the source code documentation in each file.
For example, see information on the first latency experiment 
[here](examples/zonal-latency-measurement.cc).

## Installation

To install this repository, please install the [ns-3](https://www.nsnam.org/) 
network simulator from Git first, and then clone this repository in the
`contrib/` directory of `ns-3` to install it as a module:

```bash
git clone https://github.com/nsnam/ns-3-dev-git ns-3
git clone https://github.com/evallen/zonal-filter ns-3/contrib/zonal-research
```

From the top level `ns-3` directory, configure `ns-3`:
```bash
./ns3 configure
```

Create a directory you wish your results to go into, such as
```bash
mkdir contrib/zonal-research/results/my-test
```

Then build and run the experiments in this directory, such as
```bash
./ns3 run 'contrib/zonal-research/examples/zonal-latency-measurement.cc --verbose' \
  --cwd contrib/zonal-research/results/my-test/
```
