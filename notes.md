## TIMELINE

**WEEK 1**
* **DONE** Monday - Tuesday. Implement firewall idea. 
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

## Alternative Cryptography Plans
* Idea 1 - Asymmetric signing / verifying
	* Each node signs messages with asymmetric method 
          such as RSA, each node verifies all messages
          with similar method.
* Idea 2 - Symmetric MACs
	* Suppose each pair of nodes has a symmetric keypair
          (not very realistic, but this would hopefully give
           us a 'best-case' bound of cryptography).
	* Each node creates a MAC for each message, attaches it,
          and verifies it at the receiving side. 

* Important - make sure it's relevant to modern literature.
	* Want it to reflect recent ideas / papers so we can
	  show that our idea compares well against recent ideas
	  (instead of something that was given up years ago).

### TODO

## Alternative Cryptography Idea TODO
* **DONE** Decide on alternative cryptography scheme plan
* For now - assume each node sending a message must do
  *some* operation (signing / MAC creation, etc) and attaches
  *some* additional data (signature / MAC). Assume each node
  receiving a message does the same and strips that part of
  the data.

## Future simulation ideas for paper
* Current firewall idea assumes everything is done with ultra-fast
  TCAMs, which may be difficult to actually implement
* Would be nice to show how fast it would be if you have to divert
  some traffic to SW / other filter techniques, and evaluate that as
  well
* Final comparison could be (full TCAM firewall / hybrid firewall /
  cryptography... )

## Firewall TODO
* **DONE** Create new child of UdpSourceApplication to put tags on packets.
* **DONE** Label all Udp streams with the correct tag in the omnetpp.ini.
* **DONE** Create filter module to go into the ZGs that ensures:
	* All inbound packets have type tag that matches their 
          source interface as defined in a given configuration
	  if the source interface is an ECU interface.
	* All outbound packets have type tag that matches their
          destination interface as defined in a given configuration
          if the destination interface is an ECU interface.
* **DONE** Define the security configuration for each ZG with ECUs. 
* **DONE** Add processing delay overhead
