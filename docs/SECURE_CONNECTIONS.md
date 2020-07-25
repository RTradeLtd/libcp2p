# Secure Connections Overview

When connecting to a remote peer, we have a multiaddress that looks like `/ip4/127.0.0.1/tcp/9090/p2p/somepeerid`. `somepeerid` corresponds to the SHA256 hash of an ECC public key in PEM format. When connecting to the remote peer, we initiate the connection over an insecure connection asking for public key corresponding to the peerID. When we receive the public key, we calculate the peerID locally. If it matches we then initiate an ECDH key agreement. The new key from the key agreement is used to then encrypt/decrypt messages with the peer.

# Process

```
US                                                      REMOTE PEER

1) initiate connection to remote peer
-------------------------------------------------------->

2) ask peer for public key corresponding to peerID
-------------------------------------------------------->

3) reply to request with ECC public key in PEM format
<--------------------------------------------------------

4) compute sha256 hash
<--------------------|

5) validate sha256 hash
<---------------------|

6a) if sha256 hash is invalid disconnect
<--------------------------------------|

6b) if sha256 hash is valid continue
<----------------------------------|

7) go through ECDH key agreement phase
<------------------------------------------------------->

8) messages encrypted using AES192 and the ECDH private key
<------------------------------------------------------->

```