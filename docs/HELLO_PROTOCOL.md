# Hello Protocol

The hello protocol is an optional protocol that can be used by libcp2p nodes to introduce themselves to others. Essentially it is a way of exchanging information about us, what protocol we support, etc... At a minimum it is used to exchange PeerID and public key information. Not all libcp2p nodes should be expected to use the hello protocol.

When sending a hello protocol request to a peer, your request should be your information. This helps to reduce overall round trips to accomplish a full hello protocol exchange. The response from your peer will be their hello protocol information.

Validation of the data received from a hello protocol exchange isn't required to be validated, but it is strongly advised to validate it. The best way to do this would be by inserting the information to our peerstore, as it will conduct validation to prevent inserting bad data.

# Usage

* 1) Allocate memory for `message_hello_t` and populate with desired values
* 2) CBOR encoded `message_hello_t` using `cbor_encode_hello_t`
* 3) Allocate memory for `message_t`
* 4) Populate the `data` member of `message_t` with the CBOR encoded data from encoding `message_hello_t`
* 5) Populate the `len` member of `message_t` with the length of the CBOR encoded data from encoding `message_hello_t`
* 6) Encode `message_t`
* 7) Send encoded `message_t` to peer