# Peer Identifiers

To construct a Peer Identifier (aka PeerID, peerID, etc..), you take the hash of an ECC Public Key in DER format, convert it to a multihash, and then encode the multihash using multibase. The result of this operation is your "PeerID". Laid out step by step:

1) Get an ECC Public Key in DER format
2) Hash the public key
3) Convert the hash into a multihash
4) Base encoded the multihash using multibase

# Supported Algorithms

In theory any hashing and base encoding algorithm can be used as long as they are supported by the multihash and multibase standards, collectively grouped under the multiformats umbrella. At the moment libcp2p uses SHA256, and base32. This is currently hard coded into the codebase, but will be made flexible later on

# Process (Generation)

The following diagrams how you can generate a PeerID given a public key

## MermaidJS Code

```
stateDiagram
	[*] -->  ECC_KEY
  note right of ECC_KEY: this is an ECC public key
  ECC_KEY --> ECC_KEY_DER
  note right of ECC_KEY_DER: convert ECC public key into DER format
  ECC_KEY_DER --> SHA256_HASH
  note right of SHA256_HASH: compute the sha256 hash of the DER encoded key
  SHA256_HASH --> MULTIHASH_SHA256
  note right of MULTIHASH_SHA256: take the sha256 hash and turn it into a multihash
  MULTIHASH_SHA256 --> MULTIBASE_BASE32_ENCODE
  note right of MULTIBASE_BASE32_ENCODE: encode the multihash using multibase with base32 encoding
```

## Diagram


![](https://gateway.temporal.cloud/ipfs/QmZAfE9sHXa3gMhatzCVxaUVcMriMZ8EfYTmCTnjVo83JM)

# NOTE

Currently multiaddr parsing doesn't support multibase encoded strings, so you must ensure the peerID has the multibase identifier removed