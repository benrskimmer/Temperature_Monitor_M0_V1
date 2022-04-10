/*
 * IPAddress.c
 *
 * Created: 7/28/2021 2:03:17 AM
 *  Author: Ben
 */


//#include <Arduino.h>
#include "IPAddress.h"
//#include <Print.h>
//#include <StreamString.h>


//ip_addr_t _ip;

// Access the raw byte array containing the address.  Because this returns a pointer
// to the internal structure rather than a copy of the address this function should only
// be used when you know that the usage of the returned uint8_t* will be transient and not
// stored.
// uint8_t* raw_address() {
// 	return reinterpret_cast<uint8_t*>(&v4());
// }
// const uint8_t* raw_address() const {
// 	return reinterpret_cast<const uint8_t*>(&v4());
// }

//void IP_ctor32 (uint32_t);

u32_t IP_v4(ip_addr_t* _ip)
{
	return ip_2_ip4(_ip)->addr;
}

bool IP_isV4(ip_addr_t _ip)
{
	return IP_IS_V4_VAL(_ip);
}

void IP_setV4(ip_addr_t* _ip)
{
	IP_SET_TYPE_VAL(*_ip, IPADDR_TYPE_V4);
}

void IPAddress_From_IP(ip_addr_t* to, ip_addr_t* from)
{
	ip_addr_copy(*to, *from);
}

void IPAddress_None(ip_addr_t* to) {
	to = IP_ANY_TYPE; // lwIP's v4-or-v6 generic address
}

// bool IP_isSet () const { // this is what this function was previously
// 	return !ip_addr_isany(&_ip) && ((*this) != IPADDR_NONE);
// }

bool IP_isSet (ip_addr_t _ip) {
	return !ip_addr_isany(&_ip);// && ((*this) != IPADDR_NONE);
}

void IPAddress(ip_addr_t* this, uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
	IP_setV4(this);
	((uint8_t*) &ip_2_ip4(this)->addr)[0] = first_octet;
	((uint8_t*) &ip_2_ip4(this)->addr)[1] = second_octet;
	((uint8_t*) &ip_2_ip4(this)->addr)[2] = third_octet;
	((uint8_t*) &ip_2_ip4(this)->addr)[3] = fourth_octet;
}

// void IP_ctor32(uint32_t address) {
// 	IP_setV4();
// 	IP_v4() = address;
// }

ip4_addr_t IPAddress_From_UVAR8(ip_addr_t* this, uint8_t *address) {
	IP_setV4(this);
	((uint8_t*) &ip_2_ip4(this)->addr)[0] = address[0];
	((uint8_t*) &ip_2_ip4(this)->addr)[1] = address[1];
	((uint8_t*) &ip_2_ip4(this)->addr)[2] = address[2];
	((uint8_t*) &ip_2_ip4(this)->addr)[3] = address[3];
}


void IP_PrintAddress(ip_addr_t* ip_address) {
	uint8_t* decimal_ip = (uint8_t*)(&(ip_2_ip4(ip_address)->addr));
	printf("%d.%d.%d.%d", decimal_ip[0], decimal_ip[1], decimal_ip[2], decimal_ip[3]);
}


// bool IP_fromString(const char *address) {
// 	if (!fromString4(address)) {
// 		#if LWIP_IPV6
// 		return fromString6(address);
// 		#else
// 		return false;
// 		#endif
// 	}
// 	return true;
// }

// bool IP_fromString4(const char *address) {
// 	// TODO: (IPv4) add support for "a", "a.b", "a.b.c" formats
// 
// 	uint16_t acc = 0; // Accumulator
// 	uint8_t dots = 0;
// 
// 	while (*address)
// 	{
// 		char c = *address++;
// 		if (c >= '0' && c <= '9')
// 		{
// 			acc = acc * 10 + (c - '0');
// 			if (acc > 255) {
// 				// Value out of [0..255] range
// 				return false;
// 			}
// 		}
// 		else if (c == '.')
// 		{
// 			if (dots == 3) {
// 				// Too much dots (there must be 3 dots)
// 				return false;
// 			}
// 			(*this)[dots++] = acc;
// 			acc = 0;
// 		}
// 		else
// 		{
// 			// Invalid char
// 			return false;
// 		}
// 	}
// 
// 	if (dots != 3) {
// 		// Too few dots (there must be 3 dots)
// 		return false;
// 	}
// 	(*this)[3] = acc;
// 
// 	setV4();
// 	return true;
// }

// IPAddress& IP_operator=(const uint8_t *address) {
// 	setV4();
// 	v4() = *reinterpret_cast<const uint32_t*>(address);
// 	return *this;
// }
// 
// IPAddress& IP_operator=(uint32_t address) {
// 	setV4();
// 	v4() = address;
// 	return *this;
// }
// 
// bool IP_operator==(const uint8_t* addr) const {
// 	return isV4() && v4() == *reinterpret_cast<const uint32_t*>(addr);
// }
// 
// size_t IP_printTo(Print& p) const {
// 	size_t n = 0;
// 
// 	if (!isSet())
// 	return p.print(F("(IP unset)"));
// 
// 	#if LWIP_IPV6
// 	if (isV6()) {
// 		int count0 = 0;
// 		for (int i = 0; i < 8; i++) {
// 			uint16_t bit = PP_NTOHS(raw6()[i]);
// 			if (bit || count0 < 0) {
// 				n += p.printf("%x", bit);
// 				if (count0 > 0)
// 				// no more hiding 0
// 				count0 = -8;
// 			} else
// 			count0++;
// 			if ((i != 7 && count0 < 2) || count0 == 7)
// 			n += p.print(':');
// 		}
// 		return n;
// 	}
// 	#endif
// 
// 	for(int i = 0; i < 4; i++) {
// 		n += p.print((*this)[i], DEC);
// 		if (i != 3)
// 		n += p.print('.');
// 	}
// 	return n;
// }

// String IP_toString() const
// {
// 	StreamString sstr;
// 	#if LWIP_IPV6
// 	if (isV6())
// 	sstr.reserve(40); // 8 shorts x 4 chars each + 7 colons + nullterm
// 	else
// 	#endif
// 	sstr.reserve(16); // 4 bytes with 3 chars max + 3 dots + nullterm, or '(IP unset)'
// 	printTo(sstr);
// 	return sstr;
// }

// bool IP_isValid(const String& arg) {
// 	return IPAddress().fromString(arg);
// }

// bool IP_isValid(const char* arg) {
// 	return IPAddress().fromString(arg);
// }

// const IPAddress INADDR_ANY; // generic "0.0.0.0" for IPv4 & IPv6
// const IPAddress INADDR_NONE(255,255,255,255);

void IP_clear(ip_addr_t* this) {
	this = IP_ANY_TYPE;
}

/**************************************/

// #if LWIP_IPV6
// 
// bool IP_fromString6(const char *address) {
// 	// TODO: test test test
// 
// 	uint32_t acc = 0; // Accumulator
// 	int dots = 0, doubledots = -1;
// 
// 	while (*address)
// 	{
// 		char c = tolower(*address++);
// 		if (isalnum(c)) {
// 			if (c >= 'a')
// 			c -= 'a' - '0' - 10;
// 			acc = acc * 16 + (c - '0');
// 			if (acc > 0xffff)
// 			// Value out of range
// 			return false;
// 		}
// 		else if (c == ':') {
// 			if (*address == ':') {
// 				if (doubledots >= 0)
// 				// :: allowed once
// 				return false;
// 				// remember location
// 				doubledots = dots + !!acc;
// 				address++;
// 			}
// 			if (dots == 7)
// 			// too many separators
// 			return false;
// 			raw6()[dots++] = PP_HTONS(acc);
// 			acc = 0;
// 		}
// 		else
// 		// Invalid char
// 		return false;
// 	}
// 
// 	if (doubledots == -1 && dots != 7)
// 	// Too few separators
// 	return false;
// 	raw6()[dots++] = PP_HTONS(acc);
// 
// 	if (doubledots != -1) {
// 		for (int i = dots - doubledots - 1; i >= 0; i--)
// 		raw6()[8 - dots + doubledots + i] = raw6()[doubledots + i];
// 		for (int i = doubledots; i < 8 - dots + doubledots; i++)
// 		raw6()[i] = 0;
// 	}
// 
// 	setV6();
// 	return true;
// }
// 
// #endif

IPAddress_uint32_t(ip_addr_t ip) {
	return IP_isV4(ip)? IP_v4(&ip): (uint32_t)0;
}