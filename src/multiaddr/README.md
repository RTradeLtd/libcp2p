# c-multiaddr

multiaddr for IPFS in C.

### Multiaddr provides easy networking protocols nesting, easy encapsulation of extra protocols, easy tunneling, etc.

# Usage:

#### All you need to include is multiaddr.h

## Maddr struct:

* char * string; // String that contains addresses such as /ip4/192.168.1.1/
* uint8_t * bytes; // uint8_t * that contains the enecoded address
* size_t bsize; //size_t that contains the real bytes size (Use it whenever using the bytes so you don't input trash!)

## New Multi Address From String(multiaddress_new_from_string)

struct MultiAddress* a = multiaddress_new_from_string("/ip4/127.0.0.1/tcp/8080/");

## Obtaining the byte buffer(.bytes, .bsize[0]):

  printf("TEST BYTES: %s\n",Var_To_Hex(a->bsize, a->bytes));
  Var_To_Hex = Byte Buffer to Hex String
  Hex_To_Var = Hex String to Byte Buffer

## Encapsulation & Decapsulation(m_encapsulate, m_decapsulate)

#### Remember, Decapsulation happens from right to left, never in reverse, if you have /ip4/udp/ipfs/ if you decapsulate "udp" you will also take out ipfs!

* Now the string is: /ip4/192.168.1.1/
* multiaddress_encapsulate(a,"/udp/3333/"); //Adds udp/3333/
* Now the string is: /ip4/192.168.1.1/udp/3333/
* multiaddress_decapsulate(a,"udp"); //Removes udp protocol and its address
* Now the string is: /ip4/192.168.1.1/
* multiaddress_encapsulate(a,"/tcp/8080");
* Now the string is: /ip4/192.168.1.1/tcp/8080/ 

# Constructing a multiaddress from bytes:

* struct MultiAddress* beta;
* beta = multiaddress_new_from_bytes(a->bytes,a->bsize); //This will already construct back to the string too!
* printf("B STRING: %s\n",beta->string);  //So after encapsulation and decapsulation atm this string would 
* contain: /ip4/192.168.1.1/tcp/8080/ 
 
