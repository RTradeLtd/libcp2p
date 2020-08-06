# Hello Protocol

The hello protocol is an optional protocol that can be used by libcp2p nodes to introduce themselves to others. Essentially it is a way of exchanging information about us, what protocol we support, etc... At a minimum it is used to exchange PeerID and public key information. Not all libcp2p nodes should be expected to use the hello protocol.

When sending a hello protocol request to a peer, your request should be your information. This helps to reduce overall round trips to accomplish a full hello protocol exchange. The response from your peer will be their hello protocol information.

Validation of the data received from a hello protocol exchange isn't required to be validated, but it is strongly advised to validate it. The best way to do this would be by inserting the information to our peerstore, as it will conduct validation to prevent inserting bad data.

# Process

## Code

put this into mermaidjs to generate the diagram

```mermaidjs
sequenceDiagram
	A->>+B: MESSAGE_INT
	note right of B: MESSAGE_INT contains peer identifier and public key information
	note right of B: MESSAGE_INT also informs the receiving peer to send back its info
	B->>+B: insert A's information into peerstore
	B->>+A: MESSAGE_FIN
	note left of A: MESSAGE_FIN contians peer identifier and public key information
	note left of A: MESSAGE_FIN however doesn't cause the receiving peer to send back its info
	A->>+A: insert B's information into peerstore
```

## Diagram

![](https://gateway.temporal.cloud/ipfs/QmRcHy2eRiLSSHXjANSTnW9PhMXBGgkfwgrsCtLaYLKrL8)