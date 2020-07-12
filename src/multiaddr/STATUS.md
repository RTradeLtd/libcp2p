# STATUS:
* Compatibility: Windows/Mac/Linux/Openbsd based systems (probably more)
* Finished Base58.h Encryption/Decryption Algorhytm
* Finished Endian.h Library for automating compiling for Linux/Mac/Windows
* Finished Varhexutils - Different conversion algorhytms between bytes and strings etc.
* Finsihed Varint.h - Conversion from Hex
* Finished Protocols.h - Library for loading protocols & manipulating them easily.
* Finished: Multiaddr.h (easy library for developers)
* Finished: Protoutils.h Converting from string to hex/binary, analizing strings/bin for validity,parsing ipv4,etc.
* Finished: Readme - Documentation on how to use everything so any new developer can easily use all of this
# Secondary things that might need to be implemented:
* Protocols, everything except IP4,TCP,UDP,IPFS - These we actually need.
* Few issues that are not urgent:
  - Decapsulation of tunneled protocols doesn't work, this will be fixed if needed in multiaddr.h/m_decapsulate
  - Endianness Check - protoutils.h/in protocols UDP,TCP
