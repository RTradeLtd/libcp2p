# Peer Identifiers

To construct a Peer Identifier (aka PeerID, peerID, etc..), you take the hash of an ECC Public Key in PEM format, convert it to a multihash, and then encode the multihash using multibase. The result of this operation is your "PeerID". Laid out step by step:

1) Get an ECC Public Key in PEM format
2) Hash the public key
3) Convert the hash into a multihash
4) Base encoded the multihash using multibase

# Supported Algorithms

In theory any hashing and base encoding algorithm can be used as long as they are supported by the multihash and multibase standards, collectively grouped under the multiformats umbrella. At the moment libcp2p uses SHA256, and base32. This is currently hard coded into the codebase, but will be made flexible later on


# NOTE

Currently multiaddr parsing doesn't support multibase encoded strings, so you must ensure the peerID has the multibase identifier removed