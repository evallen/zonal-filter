## TIMELINE

**WEEK 1**
* Monday - Tuesday. Implement firewall idea. 
	* Watch 1.5 hour lecture each day.
* Wednesday - Thursday. Implement cryptography alternative idea. 
	* Watch 1.5 hr lecture each day.
* Friday. Data analysis; playing with data more. Clean up TSN stuff.
	* Assignment 4, ECE5560.
* Saturday. Begin writing up paper.
	* Finish Assignment 4, ECE5560. 
* Sunday. SKIP

**WEEK 2**
* Monday - Tuesday. Write technical method section draft;
		    results draft.
* Wednesday - Thursday. Write introduction, related work, conclusion.
* Friday. Polish / flex / submit for initial review. **DEC 1 DRAFT DUE**

**WEEK 3, WEEK 4**
* Polish, run more experiments, make nice!
**DEC 15 FINAL PAPER SUBMISSION DUE**

## TSN Features that I want to use
* Per-stream Filtering and Policing (PSFP) - for firewall? maybe not
* Traffic shaping? Potentially TAS or maybe CBS (or combination?)
	* Not really sure what ATS does or if it's useful
* FRER? Need to look into more - nah
* Maybe not frame preemption since support is limited - we're aiming to simulate
	a _realistic_ IVN with _realistic_ TSN features
* Not Cut-through switching

* => traffic shaping, basically

## Questions
* Is there a better way to do filtering in INET? Look up
	* It looks like no. Probably have to insert a spot ourselves. 
	* Could put it in the 802.1Q PSFP section or somewhere else?
	* => See below

## Firewall Implementation Plan

* Two types of interfaces on a switch: 
	* "ECU interface" - interface to an ECU. Should be guarded (in/out).
	* "SW interface" - interface to another switch. Not checked.

* Need to implement firewall to only allow packets with given IDs out
  from an "ECU interface" and only allow packets with given IDs into
  an "ECU interface".

* Need all packets to have a "type"
	* Implement with a tag? sure, for now
		* Really, should be a field within the packet.
	* In the future, could make it an actual field in the payload
	  (or just add a byte or two to the overall payload). 
	* Override UdpSourceApplication to include this tag? < TODO

### TODO
* **DONE** Create new child of UdpSourceApplication to put tags on packets.
* **DONE** Label all Udp streams with the correct tag in the omnetpp.ini.
* Create filter module to go into the ZGs that ensures:
	* All inbound packets have type tag that matches their 
          source interface as defined in a given configuration
	  if the source interface is an ECU interface.
	* All outbound packets have type tag that matches their
          destination interface as defined in a given configuration
          if the destination interface is an ECU interface.
* Define the security configuration for each ZG with ECUs. 
* Add processing delay overhead
