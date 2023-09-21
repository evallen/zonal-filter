# Research Notes & Todo

## Logging and Measurements

* **(DONE)** Come up with a mechanism to measure latency of a stream (sender -> receiver)
  over many packets. It would be nice to have an output file of packet send times, receive times,
  and overall delays for processing in a Python analysis script so we could create a histogram
  of latencies and take averages and all that.

  Desired output format for a given stream:

  ```csv
  send_time, recv_time, delay
  0.0050, 0.0051, 0.0001
  ...
  ```

  Arguments to such a 'StreamLatencyTracingHelper'
  * Sending application
  * Receiving application
  * Output filename

  (Need some way to tag each packet so you can match up send / receive times).

* Come up with a way to measure number of packets dropped (how to detect if any drop?)
  when packets go through a ProcessingBridgeNetDevice such as the zonal controller or the
  MACsec controller. It would be nice to have a similar way to analyze it in Python or something.

* Throughput ... do we have to measure this? We sort of dictate what it 'must be'. Think
  about this more. Maybe if, for example, you have _so many nodes_ on a zone that they
  can't all send at 100Mbps?

* **(DONE)** Fix MacSecTrxHelper to create nodes that use the generic ProcessingBridgeAbstractDevice 
  instead of the PointToPointNetDevice abstraction that we currently have.
