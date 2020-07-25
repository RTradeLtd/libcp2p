# Secure Connections Overview

When connecting to a remote peer, we have a multiaddress that looks like `/ip4/127.0.0.1/tcp/9090/p2p/somepeerid`. `somepeerid` corresponds to the SHA256 hash of an ECC public key in PEM format. When connecting to the remote peer, we initiate the connection over an insecure connection asking for public key corresponding to the peerID. When we receive the public key, we calculate the peerID locally. If it matches we then initiate an ECDH key agreement. The new key from the key agreement is used to then encrypt/decrypt messages with the peer.

# Why Not Just Use TLS

The main reason for not using TLS directly is that it is a resource intensive process and requires using trusted CA issued certificates. By just using ECDSA keys and ECDH for shared secret negotiation, we can have a lightweight method of providing secure communications. Eventually a TLS implementation may be integrated if it proves to be needed.

# Process

## Code

put this into mermaidjs to generate the diagram

```mermaidjs
sequenceDiagram
	Peer1->>+Peer2: Connect to multiaddress of peer2
	Peer1->>+Peer2: Ask for public key corresponding to the multiaddress
	Peer2->>+Peer1: Reply with ECC public key
	Peer1->>+Peer1: Compute sha256 hash of ECC public key and convert to PeerID format
	Peer1->>+Peer1: Verify that the peerID is the same, otherwise disconnected
	Peer1->>+Peer2: Conduct ECDH key agreement to derive shared secret key
	Peer2->>+Peer1: 
    Note over Peer1,Peer2: After successful key agreement all messages exchange via encryption
	Note over Peer1,Peer2: Secret key for AES message encryption is sha256 hash of ECDH priv key
	Note over Peer1,Peer2: We take the sha256 hash of the private key in PEM format
```					

## Diagram

![](https://gateway.temporal.cloud/ipfs/QmR39BfPRcyXawA7X7eXrhmsCAEMYM1ZfPzZjYmg7buntW)