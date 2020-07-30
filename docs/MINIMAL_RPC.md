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

Messages are encoded on the wire using CBOR encoding. When sending a message the first byte is used to indicate the size of the message data. Essentailly a single message looks like, where `X` is determined by the first byte sent:

```
| 1 Byte            | X Bytes     |
| size of remainder | actual data |

```

Note that the first byte **isn't** CBOR encoded data, while the remainder of the message is CBOR encoded. We do this for simplicity sakes, making it require slightly less work to process the message, as if we first the first byte encoded into CBOR, it would require slightly more work involved in processing.

## Fields

Every message contains three fields before being encoded into CBOR

1) `type` which is a MESSAGE_TYPES_T enum
2) `len` which is the size of `data`
3) `data` which is the actual message data

All fields must contain data. If you really do not have actual message data to send, simply set `data` to a null terminator (`\0`) and `len` to 1.

## Sending And Receiving

To simplify the process of sending and receiving messages `network/messages.h` contains two convenience functions:

* 1) `handle_receive`
  * abstracts the details of receiving a message
  * allocates memory internally for the returned struct
* 2) `handle_send`
  * abstracts the details of sending a message

The usage of these two functions removes the need for all message preprocessing. Note however when sending messages, you'll need to provide the `message_t` struct, but `handle_send` will take care of CBOR encoding the data, and properly sending it on the wire.

## Limitations

At the moment individual RPC messages can't be larger than 8192 bytes (8KiB), although this may be removed in the future.