# Minimal RPC

There is a very minimal RPC framework included for sending messages between peers. It is not intended as a full on RPC system, and simply as a way to help two peers communicate. You can implement your own options ontop of this RPC framework by using the `ARBITRARY` message type, which allows for implementation defined behavior.

# Messages

## Types

The following message types are denoted as an enum `MESSAGE_TYPES`

| Type | Purpose |
|------|---------|
| MESSAGE_WANT_PEER_ID | Informs a peer we want information for a particular peerid. This is typically used for two purposes. One when connecting to a peer and establishing an ECDH key agreement it allows us to request the public key the peer. The other is used for exchanging information about other peers |
| MESSAGE_HAVE_PEER_ID | Sent in response to `MESSAGE_WANT_PEER_ID` and indicates we have the information about a peer that was requested |
| MESSAGE_WANT_PUB_KEY | Informs a peer we want the public key corresponding to a PeerID |
| MESSAGE_HAVE_PUB_KEY | Sent in resposne to a `MESSAGE_WANT_PUB_KEY` and indicates we have the public key being request
| MESSAGE_START_ECDH | Informs a peer we want to start an ECDH key agreement process |
| MESSAGE_BEGIN_ECDH | Sent ins response to a `MESSAGE_START_ECDH` and informs the peer to begin the process |
| MESSAGE_ARBITRARY | Arbitrary message type allowing for implementation defined behavior

## Encoding

Messages are encoded on the wire using CBOR encoding. When sending a message the first byte is used to indicate the size of the message data. Essentailly a single message looks like:

```
| 1 Byte            | X Bytes     |
| size of remainder | actual data |

```