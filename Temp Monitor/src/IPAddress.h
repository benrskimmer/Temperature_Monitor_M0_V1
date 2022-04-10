/*
 IPAddress.h - Base class that provides IPAddress
 Copyright (c) 2011 Adrian McEwen.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef IPAddress_h
#define IPAddress_h

#include <stdint.h>
//#include <WString.h>
//#include "Printable.h"

#include "lwip/init.h"
#include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"

// #if !LWIP_IPV6
// struct ip_addr: ipv4_addr { };
// #endif // !LWIP_IPV6

// to display a netif id with printf:
#define NETIFID_STR        "%c%c%u"
#define NETIFID_VAL(netif) \
        ((netif)? (netif)->name[0]: '-'),     \
        ((netif)? (netif)->name[1]: '-'),     \
        ((netif)? netif_get_index(netif): 42)

// A class to make it easier to handle and pass around IP addresses
// IPv6 update:
// IPAddress is now a decorator class for lwIP's ip_addr_t
// fully backward compatible with legacy IPv4-only Arduino's
// with unchanged footprint when IPv6 is disabled

//class IPAddress: public Printable {

static void IP_ctor32 (uint32_t);




//    public:
// Constructors
void IPAddress_None(ip_addr_t* to);
void IPAddress_From_IP(ip_addr_t* to, ip_addr_t* from);
void IPAddress(ip_addr_t* this, uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
// ip4_addr_t IPAddress(uint32_t address) { IP_ctor32(address); }
// ip4_addr_t IPAddress(u32_t address) { IP_ctor32(address); }
// ip4_addr_t IPAddress(int address) { IP_ctor32(address); }
ip4_addr_t IPAddress_From_UVAR8(ip_addr_t* this, uint8_t *address);

// bool IP_fromString(const char *address);
// bool IP_fromString(const String &address) { return IP_fromString(address.c_str()); }

// Overloaded cast operator to allow IPAddress objects to be used where a pointer
// to a four-byte uint8_t array is expected
// operator uint32_t() const { return IP_isV4()? IP_v4(): (uint32_t)0; }
// operator uint32_t()       { return IP_isV4()? IP_v4(): (uint32_t)0; }
// operator u32_t()    const { return IP_isV4()? IP_v4():    (u32_t)0; }
// operator u32_t()          { return IP_isV4()? IP_v4():    (u32_t)0; }
	
IPAddress_uint32_t(ip_addr_t ip);

void IP_PrintAddress(ip_addr_t* address);

bool IP_isSet ();
// operator bool () const { return IP_isSet(); } // <-
// operator bool ()       { return IP_isSet(); } // <- both are needed

// generic IPv4 wrapper to uint32-view like arduino loves to see it
//const u32_t& IP_v4() const { return ip_2_ip4(&_ip)->addr; } // for raw_address(const)
u32_t IP_v4(ip_addr_t* _ip);

// bool operator==(const IPAddress& addr) const {
//     return ip_addr_cmp(&_ip, &addr._ip);
// }
// bool operator!=(const IPAddress& addr) const {
//     return !ip_addr_cmp(&_ip, &addr._ip);
// }

// bool operator==(uint32_t addr) const {
//     return IP_isV4() && IP_v4() == addr;
// }
// bool operator==(u32_t addr) const {
//     return IP_isV4() && IP_v4() == addr;
// }
// bool operator!=(uint32_t addr) const {
//     return !(IP_isV4() && IP_v4() == addr);
// }
// bool operator!=(u32_t addr) const {
//     return !(IP_isV4() && IP_v4() == addr);
// }
// bool operator==(const uint8_t* addr) const;
// 
// int operator>>(int n) const {
//     return IP_isV4()? IP_v4() >> n: 0;
// }

// Overloaded index operator to allow getting and setting individual octets of the address
// uint8_t operator[](int index) const {
//     return IP_isV4()? *(raw_address() + index): 0;
// }
// uint8_t& operator[](int index) {
//     IP_setV4();
//     return *(raw_address() + index);
// }

// Overloaded copy operators to allow initialisation of IPAddress objects from other types
// IPAddress& operator=(const uint8_t *address);
// IPAddress& operator=(uint32_t address);
// IPAddress& operator=(const IPAddress&) = default;

//virtual size_t IP_printTo(Print& p) const;
/*String toString() const;*/

void IP_clear();

/*
check if input string(arg) is a valid IPV4 address or not.
return true on valid.
return false on invalid.
*/
//static bool isValid(const String& arg);
//static bool IP_isValid(const char* arg);

// friend class EthernetClass;
// friend class UDP;
// friend class Client;
// friend class Server;
// friend class DhcpClass;
// friend class DNSClient;

/*
lwIP address compatibility
*/
// IPAddress(const ipv4_addr& fw_addr)   { IP_setV4(); IP_v4() = fw_addr.addr; }
// IPAddress(const ipv4_addr* fw_addr)   { IP_setV4(); IP_v4() = fw_addr->addr; }
// 
// IPAddress& operator=(const ipv4_addr& fw_addr)   { IP_setV4(); IP_v4() = fw_addr.addr;  return *this; }
// IPAddress& operator=(const ipv4_addr* fw_addr)   { IP_setV4(); IP_v4() = fw_addr->addr; return *this; }
// 
// operator       ip_addr_t () const { return  _ip; }
// operator const ip_addr_t*() const { return &_ip; }
// operator       ip_addr_t*()       { return &_ip; }

bool IP_isV4(ip_addr_t _ip);
void IP_setV4(ip_addr_t* _ip);

/*bool IP_isLocal () const { return ip_addr_islinklocal(&_ip); }*/

// #if LWIP_IPV6
// 
// IPAddress(const ip_addr_t& lwip_addr) { ip_addr_copy(_ip, lwip_addr); }
// IPAddress(const ip_addr_t* lwip_addr) { ip_addr_copy(_ip, *lwip_addr); }
// 
// IPAddress& operator=(const ip_addr_t& lwip_addr) { ip_addr_copy(_ip, lwip_addr); return *this; }
// IPAddress& operator=(const ip_addr_t* lwip_addr) { ip_addr_copy(_ip, *lwip_addr); return *this; }
// 
// uint16_t* IP_raw6()
// {
//     IP_setV6();
//     return reinterpret_cast<uint16_t*>(ip_2_ip6(&_ip));
// }
// 
// const uint16_t* IP_raw6() const
// {
//     return IP_isV6()? reinterpret_cast<const uint16_t*>(ip_2_ip6(&_ip)): nullptr;
// }
// 
// // when not IPv6, ip_addr_t == ip4_addr_t so this one would be ambiguous
// // required otherwise
// operator const ip4_addr_t*() const { return IP_isV4()? ip_2_ip4(&_ip): nullptr; }
// 
// bool IP_isV6() const { return IP_IS_V6_VAL(_ip); }
// void IP_setV6() { IP_SET_TYPE_VAL(_ip, IPADDR_TYPE_V6); }
// 
// //    protected:
// bool IP_fromString6(const char *address); // this was a protected method...
// 
// #else
// 
// // allow portable code when IPv6 is not enabled
// 
// uint16_t* IP_raw6() { return nullptr; }
// const uint16_t* IP_raw6() const { return nullptr; }
// bool IP_isV6() const { return false; }
// void IP_setV6() { }
// 
// #endif

//    protected:
// bool IP_fromString4(const char *address); // this was a protected method...
// 
// 
// extern const IPAddress INADDR_ANY;
// extern const IPAddress INADDR_NONE;

#endif
