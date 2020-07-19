// Copyright 2020 RTrade Technologies Ltd
//
// licensed under GNU AFFERO GENERAL PUBLIC LICENSE;
// you may not use this file except in compliance with the License;
// You may obtain the license via the LICENSE file in the repository root;
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#define IDENTITY "0x00"
#define IP4 "0x04"
#define TCP "0x06"
#define SHA1 "0x11"
#define SHA2_256 "0x12"
#define SHA2_512 "0x13"
#define SHA3_512 "0x14"
#define SHA3_384 "0x15"
#define SHA3_256 "0x16"
#define SHA3_224 "0x17"
#define SHAKE_128 "0x18"
#define SHAKE_256 "0x19"
#define KECCAK_224 "0x1a"
#define KECCAK_256 "0x1b"
#define KECCAK_384 "0x1c"
#define KECCAK_512 "0x1d"
#define BLAKE3 "0x1e"
#define DCCP "0x21"
#define MURMUR3_128 "0x22"
#define MURMUR3_32 "0x23"
#define IP6 "0x29"
#define IP6ZONE "0x2a"
#define PATH "0x2f"
#define MULTICODEC "0x30"
#define MULTIHASH "0x31"
#define MULTIADDR "0x32"
#define MULTIBASE "0x33"
#define DNS "0x35"
#define DNS4 "0x36"
#define DNS6 "0x37"
#define DNSADDR "0x38"
#define PROTOBUF "0x50"
#define CBOR "0x51"
#define RAW "0x55"
#define DBL_SHA2_256 "0x56"
#define RLP "0x60"
#define BENCODE "0x63"
#define DAG_PB "0x70"
#define DAG_CBOR "0x71"
#define LIBP2P_KEY "0x72"
#define GIT_RAW "0x78"
#define TORRENT_INFO "0x7b"
#define TORRENT_FILE "0x7c"
#define LEOFCOIN_BLOCK "0x81"
#define LEOFCOIN_TX "0x82"
#define LEOFCOIN_PR "0x83"
#define SCTP "0x84"
#define DAG_JOSE "0x85"
#define DAG_COSE "0x86"
#define ETH_BLOCK "0x90"
#define ETH_BLOCK_LIST "0x91"
#define ETH_TX_TRIE "0x92"
#define ETH_TX "0x93"
#define ETH_TX_RECEIPT_TRIE "0x94"
#define ETH_TX_RECEIPT "0x95"
#define ETH_STATE_TRIE "0x96"
#define ETH_ACCOUNT_SNAPSHOT "0x97"
#define ETH_STORAGE_TRIE "0x98"
#define BITCOIN_BLOCK "0xb0"
#define BITCOIN_TX "0xb1"
#define BITCOIN_WITNESS_COMMITMENT "0xb2"
#define ZCASH_BLOCK "0xc0"
#define ZCASH_TX "0xc1"
#define STELLAR_BLOCK "0xd0"
#define STELLAR_TX "0xd1"
#define MD4 "0xd4"
#define MD5 "0xd5"
#define BMT "0xd6"
#define DECRED_BLOCK "0xe0"
#define DECRED_TX "0xe1"
#define IPLD_NS "0xe2"
#define IPFS_NS "0xe3"
#define SWARM_NS "0xe4"
#define IPNS_NS "0xe5"
#define ZERONET "0xe6"
#define SECP256K1_PUB "0xe7"
#define BLS12_381_G1_PUB "0xea"
#define BLS12_381_G2_PUB "0xeb"
#define X25519_PUB "0xec"
#define ED25519_PUB "0xed"
#define DASH_BLOCK "0xf0"
#define DASH_TX "0xf1"
#define SWARM_MANIFEST "0xfa"
#define SWARM_FEED "0xfb"
#define UDP "0x0111"
#define P2P_WEBRTC_STAR "0x0113"
#define P2P_WEBRTC_DIRECT "0x0114"
#define P2P_STARDUST "0x0115"
#define P2P_CIRCUIT "0x0122"
#define DAG_JSON "0x0129"
#define UDT "0x012d"
#define UTP "0x012e"
#define UNIX "0x0190"
#define P2P "0x01a5"
#define IPFS "0x01a5"
#define HTTPS "0x01bb"
#define ONION "0x01bc"
#define ONION3 "0x01bd"
#define GARLIC64 "0x01be"
#define GARLIC32 "0x01bf"
#define TLS "0x01c0"
#define QUIC "0x01cc"
#define WS "0x01dd"
#define WSS "0x01de"
#define P2P_WEBSOCKET_STAR "0x01df"
#define HTTP "0x01e0"
#define JSON "0x0200"
#define MESSAGEPACK "0x0201"
#define LIBP2P_PEER_RECORD "0x0301"
#define SHA2_256_TRUNC254_PADDED "0x1012"
#define RIPEMD_128 "0x1052"
#define RIPEMD_160 "0x1053"
#define RIPEMD_256 "0x1054"
#define RIPEMD_320 "0x1055"
#define X11 "0x1100"
#define SM3_256 "0x534d"
#define BLAKE2B_8 "0xb201"
#define BLAKE2B_16 "0xb202"
#define BLAKE2B_24 "0xb203"
#define BLAKE2B_32 "0xb204"
#define BLAKE2B_40 "0xb205"
#define BLAKE2B_48 "0xb206"
#define BLAKE2B_56 "0xb207"
#define BLAKE2B_64 "0xb208"
#define BLAKE2B_72 "0xb209"
#define BLAKE2B_80 "0xb20a"
#define BLAKE2B_88 "0xb20b"
#define BLAKE2B_96 "0xb20c"
#define BLAKE2B_104 "0xb20d"
#define BLAKE2B_112 "0xb20e"
#define BLAKE2B_120 "0xb20f"
#define BLAKE2B_128 "0xb210"
#define BLAKE2B_136 "0xb211"
#define BLAKE2B_144 "0xb212"
#define BLAKE2B_152 "0xb213"
#define BLAKE2B_160 "0xb214"
#define BLAKE2B_168 "0xb215"
#define BLAKE2B_176 "0xb216"
#define BLAKE2B_184 "0xb217"
#define BLAKE2B_192 "0xb218"
#define BLAKE2B_200 "0xb219"
#define BLAKE2B_208 "0xb21a"
#define BLAKE2B_216 "0xb21b"
#define BLAKE2B_224 "0xb21c"
#define BLAKE2B_232 "0xb21d"
#define BLAKE2B_240 "0xb21e"
#define BLAKE2B_248 "0xb21f"
#define BLAKE2B_256 "0xb220"
#define BLAKE2B_264 "0xb221"
#define BLAKE2B_272 "0xb222"
#define BLAKE2B_280 "0xb223"
#define BLAKE2B_288 "0xb224"
#define BLAKE2B_296 "0xb225"
#define BLAKE2B_304 "0xb226"
#define BLAKE2B_312 "0xb227"
#define BLAKE2B_320 "0xb228"
#define BLAKE2B_328 "0xb229"
#define BLAKE2B_336 "0xb22a"
#define BLAKE2B_344 "0xb22b"
#define BLAKE2B_352 "0xb22c"
#define BLAKE2B_360 "0xb22d"
#define BLAKE2B_368 "0xb22e"
#define BLAKE2B_376 "0xb22f"
#define BLAKE2B_384 "0xb230"
#define BLAKE2B_392 "0xb231"
#define BLAKE2B_400 "0xb232"
#define BLAKE2B_408 "0xb233"
#define BLAKE2B_416 "0xb234"
#define BLAKE2B_424 "0xb235"
#define BLAKE2B_432 "0xb236"
#define BLAKE2B_440 "0xb237"
#define BLAKE2B_448 "0xb238"
#define BLAKE2B_456 "0xb239"
#define BLAKE2B_464 "0xb23a"
#define BLAKE2B_472 "0xb23b"
#define BLAKE2B_480 "0xb23c"
#define BLAKE2B_488 "0xb23d"
#define BLAKE2B_496 "0xb23e"
#define BLAKE2B_504 "0xb23f"
#define BLAKE2B_512 "0xb240"
#define BLAKE2S_8 "0xb241"
#define BLAKE2S_16 "0xb242"
#define BLAKE2S_24 "0xb243"
#define BLAKE2S_32 "0xb244"
#define BLAKE2S_40 "0xb245"
#define BLAKE2S_48 "0xb246"
#define BLAKE2S_56 "0xb247"
#define BLAKE2S_64 "0xb248"
#define BLAKE2S_72 "0xb249"
#define BLAKE2S_80 "0xb24a"
#define BLAKE2S_88 "0xb24b"
#define BLAKE2S_96 "0xb24c"
#define BLAKE2S_104 "0xb24d"
#define BLAKE2S_112 "0xb24e"
#define BLAKE2S_120 "0xb24f"
#define BLAKE2S_128 "0xb250"
#define BLAKE2S_136 "0xb251"
#define BLAKE2S_144 "0xb252"
#define BLAKE2S_152 "0xb253"
#define BLAKE2S_160 "0xb254"
#define BLAKE2S_168 "0xb255"
#define BLAKE2S_176 "0xb256"
#define BLAKE2S_184 "0xb257"
#define BLAKE2S_192 "0xb258"
#define BLAKE2S_200 "0xb259"
#define BLAKE2S_208 "0xb25a"
#define BLAKE2S_216 "0xb25b"
#define BLAKE2S_224 "0xb25c"
#define BLAKE2S_232 "0xb25d"
#define BLAKE2S_240 "0xb25e"
#define BLAKE2S_248 "0xb25f"
#define BLAKE2S_256 "0xb260"
#define SKEIN256_8 "0xb301"
#define SKEIN256_16 "0xb302"
#define SKEIN256_24 "0xb303"
#define SKEIN256_32 "0xb304"
#define SKEIN256_40 "0xb305"
#define SKEIN256_48 "0xb306"
#define SKEIN256_56 "0xb307"
#define SKEIN256_64 "0xb308"
#define SKEIN256_72 "0xb309"
#define SKEIN256_80 "0xb30a"
#define SKEIN256_88 "0xb30b"
#define SKEIN256_96 "0xb30c"
#define SKEIN256_104 "0xb30d"
#define SKEIN256_112 "0xb30e"
#define SKEIN256_120 "0xb30f"
#define SKEIN256_128 "0xb310"
#define SKEIN256_136 "0xb311"
#define SKEIN256_144 "0xb312"
#define SKEIN256_152 "0xb313"
#define SKEIN256_160 "0xb314"
#define SKEIN256_168 "0xb315"
#define SKEIN256_176 "0xb316"
#define SKEIN256_184 "0xb317"
#define SKEIN256_192 "0xb318"
#define SKEIN256_200 "0xb319"
#define SKEIN256_208 "0xb31a"
#define SKEIN256_216 "0xb31b"
#define SKEIN256_224 "0xb31c"
#define SKEIN256_232 "0xb31d"
#define SKEIN256_240 "0xb31e"
#define SKEIN256_248 "0xb31f"
#define SKEIN256_256 "0xb320"
#define SKEIN512_8 "0xb321"
#define SKEIN512_16 "0xb322"
#define SKEIN512_24 "0xb323"
#define SKEIN512_32 "0xb324"
#define SKEIN512_40 "0xb325"
#define SKEIN512_48 "0xb326"
#define SKEIN512_56 "0xb327"
#define SKEIN512_64 "0xb328"
#define SKEIN512_72 "0xb329"
#define SKEIN512_80 "0xb32a"
#define SKEIN512_88 "0xb32b"
#define SKEIN512_96 "0xb32c"
#define SKEIN512_104 "0xb32d"
#define SKEIN512_112 "0xb32e"
#define SKEIN512_120 "0xb32f"
#define SKEIN512_128 "0xb330"
#define SKEIN512_136 "0xb331"
#define SKEIN512_144 "0xb332"
#define SKEIN512_152 "0xb333"
#define SKEIN512_160 "0xb334"
#define SKEIN512_168 "0xb335"
#define SKEIN512_176 "0xb336"
#define SKEIN512_184 "0xb337"
#define SKEIN512_192 "0xb338"
#define SKEIN512_200 "0xb339"
#define SKEIN512_208 "0xb33a"
#define SKEIN512_216 "0xb33b"
#define SKEIN512_224 "0xb33c"
#define SKEIN512_232 "0xb33d"
#define SKEIN512_240 "0xb33e"
#define SKEIN512_248 "0xb33f"
#define SKEIN512_256 "0xb340"
#define SKEIN512_264 "0xb341"
#define SKEIN512_272 "0xb342"
#define SKEIN512_280 "0xb343"
#define SKEIN512_288 "0xb344"
#define SKEIN512_296 "0xb345"
#define SKEIN512_304 "0xb346"
#define SKEIN512_312 "0xb347"
#define SKEIN512_320 "0xb348"
#define SKEIN512_328 "0xb349"
#define SKEIN512_336 "0xb34a"
#define SKEIN512_344 "0xb34b"
#define SKEIN512_352 "0xb34c"
#define SKEIN512_360 "0xb34d"
#define SKEIN512_368 "0xb34e"
#define SKEIN512_376 "0xb34f"
#define SKEIN512_384 "0xb350"
#define SKEIN512_392 "0xb351"
#define SKEIN512_400 "0xb352"
#define SKEIN512_408 "0xb353"
#define SKEIN512_416 "0xb354"
#define SKEIN512_424 "0xb355"
#define SKEIN512_432 "0xb356"
#define SKEIN512_440 "0xb357"
#define SKEIN512_448 "0xb358"
#define SKEIN512_456 "0xb359"
#define SKEIN512_464 "0xb35a"
#define SKEIN512_472 "0xb35b"
#define SKEIN512_480 "0xb35c"
#define SKEIN512_488 "0xb35d"
#define SKEIN512_496 "0xb35e"
#define SKEIN512_504 "0xb35f"
#define SKEIN512_512 "0xb360"
#define SKEIN1024_8 "0xb361"
#define SKEIN1024_16 "0xb362"
#define SKEIN1024_24 "0xb363"
#define SKEIN1024_32 "0xb364"
#define SKEIN1024_40 "0xb365"
#define SKEIN1024_48 "0xb366"
#define SKEIN1024_56 "0xb367"
#define SKEIN1024_64 "0xb368"
#define SKEIN1024_72 "0xb369"
#define SKEIN1024_80 "0xb36a"
#define SKEIN1024_88 "0xb36b"
#define SKEIN1024_96 "0xb36c"
#define SKEIN1024_104 "0xb36d"
#define SKEIN1024_112 "0xb36e"
#define SKEIN1024_120 "0xb36f"
#define SKEIN1024_128 "0xb370"
#define SKEIN1024_136 "0xb371"
#define SKEIN1024_144 "0xb372"
#define SKEIN1024_152 "0xb373"
#define SKEIN1024_160 "0xb374"
#define SKEIN1024_168 "0xb375"
#define SKEIN1024_176 "0xb376"
#define SKEIN1024_184 "0xb377"
#define SKEIN1024_192 "0xb378"
#define SKEIN1024_200 "0xb379"
#define SKEIN1024_208 "0xb37a"
#define SKEIN1024_216 "0xb37b"
#define SKEIN1024_224 "0xb37c"
#define SKEIN1024_232 "0xb37d"
#define SKEIN1024_240 "0xb37e"
#define SKEIN1024_248 "0xb37f"
#define SKEIN1024_256 "0xb380"
#define SKEIN1024_264 "0xb381"
#define SKEIN1024_272 "0xb382"
#define SKEIN1024_280 "0xb383"
#define SKEIN1024_288 "0xb384"
#define SKEIN1024_296 "0xb385"
#define SKEIN1024_304 "0xb386"
#define SKEIN1024_312 "0xb387"
#define SKEIN1024_320 "0xb388"
#define SKEIN1024_328 "0xb389"
#define SKEIN1024_336 "0xb38a"
#define SKEIN1024_344 "0xb38b"
#define SKEIN1024_352 "0xb38c"
#define SKEIN1024_360 "0xb38d"
#define SKEIN1024_368 "0xb38e"
#define SKEIN1024_376 "0xb38f"
#define SKEIN1024_384 "0xb390"
#define SKEIN1024_392 "0xb391"
#define SKEIN1024_400 "0xb392"
#define SKEIN1024_408 "0xb393"
#define SKEIN1024_416 "0xb394"
#define SKEIN1024_424 "0xb395"
#define SKEIN1024_432 "0xb396"
#define SKEIN1024_440 "0xb397"
#define SKEIN1024_448 "0xb398"
#define SKEIN1024_456 "0xb399"
#define SKEIN1024_464 "0xb39a"
#define SKEIN1024_472 "0xb39b"
#define SKEIN1024_480 "0xb39c"
#define SKEIN1024_488 "0xb39d"
#define SKEIN1024_496 "0xb39e"
#define SKEIN1024_504 "0xb39f"
#define SKEIN1024_512 "0xb3a0"
#define SKEIN1024_520 "0xb3a1"
#define SKEIN1024_528 "0xb3a2"
#define SKEIN1024_536 "0xb3a3"
#define SKEIN1024_544 "0xb3a4"
#define SKEIN1024_552 "0xb3a5"
#define SKEIN1024_560 "0xb3a6"
#define SKEIN1024_568 "0xb3a7"
#define SKEIN1024_576 "0xb3a8"
#define SKEIN1024_584 "0xb3a9"
#define SKEIN1024_592 "0xb3aa"
#define SKEIN1024_600 "0xb3ab"
#define SKEIN1024_608 "0xb3ac"
#define SKEIN1024_616 "0xb3ad"
#define SKEIN1024_624 "0xb3ae"
#define SKEIN1024_632 "0xb3af"
#define SKEIN1024_640 "0xb3b0"
#define SKEIN1024_648 "0xb3b1"
#define SKEIN1024_656 "0xb3b2"
#define SKEIN1024_664 "0xb3b3"
#define SKEIN1024_672 "0xb3b4"
#define SKEIN1024_680 "0xb3b5"
#define SKEIN1024_688 "0xb3b6"
#define SKEIN1024_696 "0xb3b7"
#define SKEIN1024_704 "0xb3b8"
#define SKEIN1024_712 "0xb3b9"
#define SKEIN1024_720 "0xb3ba"
#define SKEIN1024_728 "0xb3bb"
#define SKEIN1024_736 "0xb3bc"
#define SKEIN1024_744 "0xb3bd"
#define SKEIN1024_752 "0xb3be"
#define SKEIN1024_760 "0xb3bf"
#define SKEIN1024_768 "0xb3c0"
#define SKEIN1024_776 "0xb3c1"
#define SKEIN1024_784 "0xb3c2"
#define SKEIN1024_792 "0xb3c3"
#define SKEIN1024_800 "0xb3c4"
#define SKEIN1024_808 "0xb3c5"
#define SKEIN1024_816 "0xb3c6"
#define SKEIN1024_824 "0xb3c7"
#define SKEIN1024_832 "0xb3c8"
#define SKEIN1024_840 "0xb3c9"
#define SKEIN1024_848 "0xb3ca"
#define SKEIN1024_856 "0xb3cb"
#define SKEIN1024_864 "0xb3cc"
#define SKEIN1024_872 "0xb3cd"
#define SKEIN1024_880 "0xb3ce"
#define SKEIN1024_888 "0xb3cf"
#define SKEIN1024_896 "0xb3d0"
#define SKEIN1024_904 "0xb3d1"
#define SKEIN1024_912 "0xb3d2"
#define SKEIN1024_920 "0xb3d3"
#define SKEIN1024_928 "0xb3d4"
#define SKEIN1024_936 "0xb3d5"
#define SKEIN1024_944 "0xb3d6"
#define SKEIN1024_952 "0xb3d7"
#define SKEIN1024_960 "0xb3d8"
#define SKEIN1024_968 "0xb3d9"
#define SKEIN1024_976 "0xb3da"
#define SKEIN1024_984 "0xb3db"
#define SKEIN1024_992 "0xb3dc"
#define SKEIN1024_1000 "0xb3dd"
#define SKEIN1024_1008 "0xb3de"
#define SKEIN1024_1016 "0xb3df"
#define SKEIN1024_1024 "0xb3e0"
#define POSEIDON_BLS12_381_A2_FC1 "0xb401"
#define POSEIDON_BLS12_381_A2_FC1_SC "0xb402"
#define FIL_COMMITMENT_UNSEALED "0xf101"
#define FIL_COMMITMENT_SEALED "0xf102"
#define HOLOCHAIN_ADR_V0 "0x807124"
#define HOLOCHAIN_ADR_V1 "0x817124"
#define HOLOCHAIN_KEY_V0 "0x947124"
#define HOLOCHAIN_KEY_V1 "0x957124"
#define HOLOCHAIN_SIG_V0 "0xa27124"
#define HOLOCHAIN_SIG_V1 "0xa37124"
#define CODECS_COUNT 446

const char *CODECS_LIST[CODECS_COUNT] = {IDENTITY, IP4, TCP, SHA1, SHA2_256, SHA2_512, SHA3_512, SHA3_384, SHA3_256, SHA3_224, SHAKE_128, SHAKE_256, KECCAK_224, KECCAK_256, KECCAK_384, KECCAK_512, BLAKE3, DCCP, MURMUR3_128, MURMUR3_32, IP6, IP6ZONE, PATH, MULTICODEC, MULTIHASH, MULTIADDR, MULTIBASE, DNS, DNS4, DNS6, DNSADDR, PROTOBUF, CBOR, RAW, DBL_SHA2_256, RLP, BENCODE, DAG_PB, DAG_CBOR, LIBP2P_KEY, GIT_RAW, TORRENT_INFO, TORRENT_FILE, LEOFCOIN_BLOCK, LEOFCOIN_TX, LEOFCOIN_PR, SCTP, DAG_JOSE, DAG_COSE, ETH_BLOCK, ETH_BLOCK_LIST, ETH_TX_TRIE, ETH_TX, ETH_TX_RECEIPT_TRIE, ETH_TX_RECEIPT, ETH_STATE_TRIE, ETH_ACCOUNT_SNAPSHOT, ETH_STORAGE_TRIE, BITCOIN_BLOCK, BITCOIN_TX, BITCOIN_WITNESS_COMMITMENT, ZCASH_BLOCK, ZCASH_TX, STELLAR_BLOCK, STELLAR_TX, MD4, MD5, BMT, DECRED_BLOCK, DECRED_TX, IPLD_NS, IPFS_NS, SWARM_NS, IPNS_NS, ZERONET, SECP256K1_PUB, BLS12_381_G1_PUB, BLS12_381_G2_PUB, X25519_PUB, ED25519_PUB, DASH_BLOCK, DASH_TX, SWARM_MANIFEST, SWARM_FEED, UDP, P2P_WEBRTC_STAR, P2P_WEBRTC_DIRECT, P2P_STARDUST, P2P_CIRCUIT, DAG_JSON, UDT, UTP, UNIX, P2P, IPFS, HTTPS, ONION, ONION3, GARLIC64, GARLIC32, TLS, QUIC, WS, WSS, P2P_WEBSOCKET_STAR, HTTP, JSON, MESSAGEPACK, LIBP2P_PEER_RECORD, SHA2_256_TRUNC254_PADDED, RIPEMD_128, RIPEMD_160, RIPEMD_256, RIPEMD_320, X11, SM3_256, BLAKE2B_8, BLAKE2B_16, BLAKE2B_24, BLAKE2B_32, BLAKE2B_40, BLAKE2B_48, BLAKE2B_56, BLAKE2B_64, BLAKE2B_72, BLAKE2B_80, BLAKE2B_88, BLAKE2B_96, BLAKE2B_104, BLAKE2B_112, BLAKE2B_120, BLAKE2B_128, BLAKE2B_136, BLAKE2B_144, BLAKE2B_152, BLAKE2B_160, BLAKE2B_168, BLAKE2B_176, BLAKE2B_184, BLAKE2B_192, BLAKE2B_200, BLAKE2B_208, BLAKE2B_216, BLAKE2B_224, BLAKE2B_232, BLAKE2B_240, BLAKE2B_248, BLAKE2B_256, BLAKE2B_264, BLAKE2B_272, BLAKE2B_280, BLAKE2B_288, BLAKE2B_296, BLAKE2B_304, BLAKE2B_312, BLAKE2B_320, BLAKE2B_328, BLAKE2B_336, BLAKE2B_344, BLAKE2B_352, BLAKE2B_360, BLAKE2B_368, BLAKE2B_376, BLAKE2B_384, BLAKE2B_392, BLAKE2B_400, BLAKE2B_408, BLAKE2B_416, BLAKE2B_424, BLAKE2B_432, BLAKE2B_440, BLAKE2B_448, BLAKE2B_456, BLAKE2B_464, BLAKE2B_472, BLAKE2B_480, BLAKE2B_488, BLAKE2B_496, BLAKE2B_504, BLAKE2B_512, BLAKE2S_8, BLAKE2S_16, BLAKE2S_24, BLAKE2S_32, BLAKE2S_40, BLAKE2S_48, BLAKE2S_56, BLAKE2S_64, BLAKE2S_72, BLAKE2S_80, BLAKE2S_88, BLAKE2S_96, BLAKE2S_104, BLAKE2S_112, BLAKE2S_120, BLAKE2S_128, BLAKE2S_136, BLAKE2S_144, BLAKE2S_152, BLAKE2S_160, BLAKE2S_168, BLAKE2S_176, BLAKE2S_184, BLAKE2S_192, BLAKE2S_200, BLAKE2S_208, BLAKE2S_216, BLAKE2S_224, BLAKE2S_232, BLAKE2S_240, BLAKE2S_248, BLAKE2S_256, SKEIN256_8, SKEIN256_16, SKEIN256_24, SKEIN256_32, SKEIN256_40, SKEIN256_48, SKEIN256_56, SKEIN256_64, SKEIN256_72, SKEIN256_80, SKEIN256_88, SKEIN256_96, SKEIN256_104, SKEIN256_112, SKEIN256_120, SKEIN256_128, SKEIN256_136, SKEIN256_144, SKEIN256_152, SKEIN256_160, SKEIN256_168, SKEIN256_176, SKEIN256_184, SKEIN256_192, SKEIN256_200, SKEIN256_208, SKEIN256_216, SKEIN256_224, SKEIN256_232, SKEIN256_240, SKEIN256_248, SKEIN256_256, SKEIN512_8, SKEIN512_16, SKEIN512_24, SKEIN512_32, SKEIN512_40, SKEIN512_48, SKEIN512_56, SKEIN512_64, SKEIN512_72, SKEIN512_80, SKEIN512_88, SKEIN512_96, SKEIN512_104, SKEIN512_112, SKEIN512_120, SKEIN512_128, SKEIN512_136, SKEIN512_144, SKEIN512_152, SKEIN512_160, SKEIN512_168, SKEIN512_176, SKEIN512_184, SKEIN512_192, SKEIN512_200, SKEIN512_208, SKEIN512_216, SKEIN512_224, SKEIN512_232, SKEIN512_240, SKEIN512_248, SKEIN512_256, SKEIN512_264, SKEIN512_272, SKEIN512_280, SKEIN512_288, SKEIN512_296, SKEIN512_304, SKEIN512_312, SKEIN512_320, SKEIN512_328, SKEIN512_336, SKEIN512_344, SKEIN512_352, SKEIN512_360, SKEIN512_368, SKEIN512_376, SKEIN512_384, SKEIN512_392, SKEIN512_400, SKEIN512_408, SKEIN512_416, SKEIN512_424, SKEIN512_432, SKEIN512_440, SKEIN512_448, SKEIN512_456, SKEIN512_464, SKEIN512_472, SKEIN512_480, SKEIN512_488, SKEIN512_496, SKEIN512_504, SKEIN512_512, SKEIN1024_8, SKEIN1024_16, SKEIN1024_24, SKEIN1024_32, SKEIN1024_40, SKEIN1024_48, SKEIN1024_56, SKEIN1024_64, SKEIN1024_72, SKEIN1024_80, SKEIN1024_88, SKEIN1024_96, SKEIN1024_104, SKEIN1024_112, SKEIN1024_120, SKEIN1024_128, SKEIN1024_136, SKEIN1024_144, SKEIN1024_152, SKEIN1024_160, SKEIN1024_168, SKEIN1024_176, SKEIN1024_184, SKEIN1024_192, SKEIN1024_200, SKEIN1024_208, SKEIN1024_216, SKEIN1024_224, SKEIN1024_232, SKEIN1024_240, SKEIN1024_248, SKEIN1024_256, SKEIN1024_264, SKEIN1024_272, SKEIN1024_280, SKEIN1024_288, SKEIN1024_296, SKEIN1024_304, SKEIN1024_312, SKEIN1024_320, SKEIN1024_328, SKEIN1024_336, SKEIN1024_344, SKEIN1024_352, SKEIN1024_360, SKEIN1024_368, SKEIN1024_376, SKEIN1024_384, SKEIN1024_392, SKEIN1024_400, SKEIN1024_408, SKEIN1024_416, SKEIN1024_424, SKEIN1024_432, SKEIN1024_440, SKEIN1024_448, SKEIN1024_456, SKEIN1024_464, SKEIN1024_472, SKEIN1024_480, SKEIN1024_488, SKEIN1024_496, SKEIN1024_504, SKEIN1024_512, SKEIN1024_520, SKEIN1024_528, SKEIN1024_536, SKEIN1024_544, SKEIN1024_552, SKEIN1024_560, SKEIN1024_568, SKEIN1024_576, SKEIN1024_584, SKEIN1024_592, SKEIN1024_600, SKEIN1024_608, SKEIN1024_616, SKEIN1024_624, SKEIN1024_632, SKEIN1024_640, SKEIN1024_648, SKEIN1024_656, SKEIN1024_664, SKEIN1024_672, SKEIN1024_680, SKEIN1024_688, SKEIN1024_696, SKEIN1024_704, SKEIN1024_712, SKEIN1024_720, SKEIN1024_728, SKEIN1024_736, SKEIN1024_744, SKEIN1024_752, SKEIN1024_760, SKEIN1024_768, SKEIN1024_776, SKEIN1024_784, SKEIN1024_792, SKEIN1024_800, SKEIN1024_808, SKEIN1024_816, SKEIN1024_824, SKEIN1024_832, SKEIN1024_840, SKEIN1024_848, SKEIN1024_856, SKEIN1024_864, SKEIN1024_872, SKEIN1024_880, SKEIN1024_888, SKEIN1024_896, SKEIN1024_904, SKEIN1024_912, SKEIN1024_920, SKEIN1024_928, SKEIN1024_936, SKEIN1024_944, SKEIN1024_952, SKEIN1024_960, SKEIN1024_968, SKEIN1024_976, SKEIN1024_984, SKEIN1024_992, SKEIN1024_1000, SKEIN1024_1008, SKEIN1024_1016, SKEIN1024_1024, POSEIDON_BLS12_381_A2_FC1, POSEIDON_BLS12_381_A2_FC1_SC, FIL_COMMITMENT_UNSEALED, FIL_COMMITMENT_SEALED, HOLOCHAIN_ADR_V0, HOLOCHAIN_ADR_V1, HOLOCHAIN_KEY_V0, HOLOCHAIN_KEY_V1, HOLOCHAIN_SIG_V0, HOLOCHAIN_SIG_V1};

