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

## Basic Zonal Topology

**Network topology:**
 One or more zones with one or more endpoints each
 connected to a zonal gateway in a tree-like structure.

For example, a zonal network with `topology = {2, 3}`
 (two zones, one with 2 endpoints and one with 3) would look
 like this:

```
     ZONE ONE                       |                ZONE TWO
                                    |
n1.1 ----|                          |                          |---- n2.1
         |                                                     |
        n1.0 == n1.M ===== n1.N == nGW == n2.N ===== n2.M -- n2.0 -- n2.2
         |                                                     |
n1.2 ----|                          |                          |---- n2.3
```

where nodes are represented as:
 * `nGW`     - is the gateway
 * `n1.*`    - is the zone-one nodes (n1.1)
 * `n2.*`    - is the zone-two nodes (n2.1)
 * `n*.M`    - is the MACsec transceiver for a zonal controller (n1.M)
 * `n*.N`    - is the MACsec transceiver for the gateway on a given zone (n1.N) (NOTE * )
 * `n*.0`    - is the zonal controller for a zone (n1.0)

and where connections are respresented as:
 * `---`     - "Endpoint" Automotive Ethernet (e.g., 100Mbps)
 * `===`     - "Backbone" Automotive Ethernet (e.g., 1000Mbps)

Depending on the topology specified in the header file,
the simulation could have more zones with different numbers
of endpoints, etc.

_NOTE * : 
   Currently, we simulate our network with a MACsec transceiver on each side
   of each backbone connection (i.e., an inter-zonal message gets encrypted
   when leaving a zonal controller, decrypted when entering the gateway, encrypted
   when leaving the gateway, and then decrypted again when entering a zonal
   controller). 

   It is possible that we could implement the MACsec transceivers so that 
   we only have one per zone (such as one on each ZC, and allow the messsages
   to go through the GW encrypted). 

   This should be the topic of a future simulation, potentially implemented 
   with a switch in the configuration._

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
