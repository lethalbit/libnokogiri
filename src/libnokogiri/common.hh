// SPDX-License-Identifier: LGPL-3.0-or-later
/* common.hh - Common templates and type definitions */
#pragma once
#if !defined(LIBNOKOGIRI_COMMON_HH)
#define LIBNOKOGIRI_COMMON_HH

#include <libnokogiri/internal/defs.hh>
#include <libnokogiri/internal/mmap.hh>
#include <libnokogiri/internal/fd.hh>

namespace libnokogiri {
	/*! \enum libnokogiri::link_type_t
		\brief Defines the link layer of the interface

		This table and the descriptions of each link type has been directly lifted from [https://www.tcpdump.org/linktypes.html](https://www.tcpdump.org/linktypes.html).

		Note: See the site linked above for details on the `link_type_t::User0` through `link_type_t::User15` values and other notes about reserved ranges.

		TODO: Waiting on the tcpdump mailing list to respond to https://lists.sandelman.ca/pipermail/tcpdump-workers/2020-October/001548.html to see if we can add needed
		link types.
	*/
	enum class link_type_t : std::uint16_t {
		BSDLoopback                = 0x0000U, /*!< BSD loopback encapsulation; the link layer header is a 4-byte field, in host byte order, containing a value of 2 for IPv4 packets, a value of either 24, 28, or 30 for IPv6 packets, a value of 7 for OSI packets, or a value of 23 for IPX packets. All of the IPv6 values correspond to IPv6 packets; code reading files should check for all of them. Note that `host byte order` is the byte order of the machine on which the packets are captured; if a live capture is being done, `host byte order` is the byte order of the machine capturing the packets, but if a `savefile` is being read, the byte order is not necessarily that of the machine reading the capture file. */
		Ethernet                   = 0x0001U, /*!< [IEEE 802.3](https://ieeexplore.ieee.org/document/7428776/) Ethernet (10Mb, 100Mb, 1000Mb, and up). */
		/* 0x0002U : Undefined, presumed reserved */
		AX25                       = 0x0003U, /*!< [AX.25](http://www.ax25.net/AX25.2.2-Jul%2098-2.pdf) packet, with nothing preceding it. */
		/* 0x0004U - 0x0005U : Undefined, presumed reserved */
		IEEE802_5                  = 0x0006U, /*!< IEEE 802.5 Token Ring. */
		ARCNET_BSD                 = 0x0007U, /*!< ARCNET Data Packets, as described by the ARCNET Trade Association standard [ATA 878.1-1999](https://web.archive.org/web/20100414154838/http://www.arcnet.com/resources/ata8781.pdf), but without the Starting Delimiter, Information Length, or Frame Check Sequence fields, and with only the first ISU of the Destination Identifier. For most packet types, ARCNET Trade Association draft standard [ATA 878.2](https://web.archive.org/web/20100414154914/http://www.arcnet.com/resources/ata8782.pdf) is also used. See also [RFC 1051](https://tools.ietf.org/html/rfc1051) and [RFC 1201](https://tools.ietf.org/html/rfc1201); for RFC 1051 frames, ATA 878.2 is not used. */
		SLIP                       = 0x0008U, /*!< SLIP, encapsulated with a [LINKTYPE_SLIP header](https://www.tcpdump.org/linktypes/LINKTYPE_SLIP.html). */
		PPP                        = 0x0009U, /*!< PPP, as per [RFC 1661](https://tools.ietf.org/html/rfc1661) and [RFC 1662](https://tools.ietf.org/html/rfc1662); if the first 2 bytes are 0xff and 0x03, it's PPP in HDLC-like framing, with the PPP header following those two bytes, otherwise it's PPP without framing, and the packet begins with the PPP header. The data in the frame is not octet-stuffed or bit-stuffed. */
		FDDI                       = 0x000AU, /*!< FDDI, as specified by ANSI INCITS 239-1994. */
		/* 0x000BU - 0x0031U : Undefined, presumed reserved */
		PPP_HDLC                   = 0x0032U, /*!< PPP in HDLC-like framing, as per [RFC 1662](https://tools.ietf.org/html/rfc1662), or Cisco PPP with HDLC framing, as per [section 4.3.1 of RFC 1547](https://tools.ietf.org/html/rfc1547#section-4.3.1); the first byte will be 0xFF for PPP in HDLC-like framing, and will be 0x0F or 0x8F for Cisco PPP with HDLC framing. The data in the frame is not octet-stuffed or bit-stuffed. */
		PPPoE                      = 0x0033U, /*!< PPPoE; the packet begins with a PPPoE header, as per [RFC 2516](https://tools.ietf.org/html/rfc2516). */
		/* 0x0034U - 0x0063U : Undefined, presumed reserved */
		ATM_RFC1483                = 0x0064U, /*!< [RFC 1483](https://tools.ietf.org/html/rfc1483) LLC/SNAP-encapsulated ATM; the packet begins with an [ISO 8802-2](https://www.iso.org/standard/30174.html) (formerly known as IEEE 802.2) LLC header. */
		Raw                        = 0x0065U, /*!< Raw IP; the packet begins with an IPv4 or IPv6 header, with the `version` field of the header indicating whether it's an IPv4 or IPv6 header. */
		/* 0x0066U - 0x0067U : Undefined, presumed reserved */
		CiscoHDLC                  = 0x0068U, /*!< Cisco PPP with HDLC framing, as per [section 4.3.1 of RFC 1547](https://tools.ietf.org/html/rfc1547#section-4.3.1). */
		IEEE802_11                 = 0x0069U, /*!< [IEEE 802.11](https://ieeexplore.ieee.org/document/7786995/) wireless LAN. */
		/* 0x0068U : Undefined, presumed reserved */
		LAPFFrameRelay             = 0x006BU, /*!< Frame Relay LAPF frames, beginning with a [ITU-T Recommendation Q.922](https://www.itu.int/rec/T-REC-Q.922/en/) LAPF header starting with the address field, and without an FCS at the end of the frame. */
		OpenBSDLoopback            = 0x006CU, /*!< OpenBSD loopback encapsulation; the link-layer header is a 4-byte field, in network byte order, containing a value of 2 for IPv4 packets, a value of either 24, 28, or 30 for IPv6 packets, a value of 7 for OSI packets, or a value of 23 for IPX packets. All of the IPv6 values correspond to IPv6 packets; code reading files should check for all of them. */
		/* 0x006DU - 0x0070U : Undefined, presumed reserved */
		LinuxSLL                   = 0x0071U, /*!< [Linux "cooked" capture encapsulation](https://www.tcpdump.org/linktypes/LINKTYPE_LINUX_SLL.html). */
		AppleLocalTalk             = 0x0072U, /*!< Apple LocalTalk; the packet begins with an AppleTalk LocalTalk Link Access Protocol header, as described in chapter 1 of [Inside AppleTalk, Second Edition](https://web.archive.org/web/20040719034527/http://developer.apple.com:80/macos/opentransport/docs/dev/Inside_AppleTalk.pdf). */
		/* 0x0073U - 0x0074U : Undefined, presumed reserved */
		OpenBSDPFLog               = 0x0075U, /*!< OpenBSD pflog; the link-layer header contains a `struct pfloghdr` structure, as defined by the host on which the file was saved. (This differs from operating system to operating system and release to release; there is nothing in the file to indicate what the layout of that structure is.) */
		/* 0x0076U - 0x0067U : Undefined, presumed reserved */
		IEEE802_11_Prism            = 0x0077U, /*!< [Prism monitor mode information](https://www.tcpdump.org/linktypes/LINKTYPE_IEEE802_11_PRISM.html) followed by an 802.11 header. */
		/* 0x0078U - 0x0079U : Undefined, presumed reserved */
		IPOverFC                   = 0x007AU, /*!< [RFC 2625](https://tools.ietf.org/html/rfc2625) IP-over-Fibre Channel, with the link-layer header being the Network_Header as described in that RFC. */
		SunATM                     = 0x007BU, /*!< ATM traffic, encapsulated as per [the scheme used by SunATM devices](https://www.tcpdump.org/linktypes/LINKTYPE_SUNATM.html). */
		/* 0x007CU - 0x007EU : Undefined, presumed reserved */
		IEEE802_11_Radiotap        = 0x007FU, /*!< [Radiotap link-layer information](http://www.radiotap.org/) followed by an 802.11 header. */
		/* 0x0080U : Undefined, presumed reserved */
		ARCNETLinux                = 0x0081U, /*!< ARCNET Data Packets, as described by the ARCNET Trade Association standard [ATA 878.1-1999](https://web.archive.org/web/20100414154838/http://www.arcnet.com/resources/ata8781.pdf), but without the Starting Delimiter, Information Length, or Frame Check Sequence fields, with only the first ISU of the Destination Identifier, and with an extra two-ISU `offset` field following the Destination Identifier. For most packet types, ARCNET Trade Association draft standard [ATA 878.2](https://web.archive.org/web/20100414154914/http://www.arcnet.com/resources/ata8782.pdf) is also used; however, no exception frames are supplied, and reassembled frames, rather than fragments, are supplied. See also [RFC 1051](https://tools.ietf.org/html/rfc1051) and [RFC 1201](https://tools.ietf.org/html/rfc1201); for RFC 1051 frames, ATA 878.2 is not used. */
		/* 0x0082U - 0x0089U : Undefined, presumed reserved */
		AppleIPOverIEEE1394        = 0x008AU, /*!< [Apple IP-over-IEEE 1394 cooked header](https://www.tcpdump.org/linktypes/LINKTYPE_APPLE_IP_OVER_IEEE1394.html). */
		MTP2WithPHeader            = 0x008BU, /*!< Signaling System 7 Message Transfer Part Level 2, as specified by [ITU-T Recommendation Q.703](https://www.itu.int/rec/T-REC-Q.703/en/), preceded by a pseudo-header. */
		MTP2                       = 0x008CU, /*!< Signaling System 7 Message Transfer Part Level 2, as specified by [ITU-T Recommendation Q.703](https://www.itu.int/rec/T-REC-Q.703/en/). */
		MTP3                       = 0x008DU, /*!< Signaling System 7 Message Transfer Part Level 3, as specified by [ITU-T Recommendation Q.704](https://www.itu.int/rec/T-REC-Q.704/en/), with no MTP2 header preceding the MTP3 packet. */
		SCCP                       = 0x008EU, /*!< Signaling System 7 Signaling Connection Control Part, as specified by [ITU-T Recommendation Q.711](https://www.itu.int/rec/T-REC-Q.711/en/), [ITU-T Recommendation Q.712](https://www.itu.int/rec/T-REC-Q.712/en/), [ITU-T Recommendation Q.713](https://www.itu.int/rec/T-REC-Q.713/en/), and [ITU-T Recommendation Q.714](https://www.itu.int/rec/T-REC-Q.714/en/), with no MTP3 or MTP2 headers preceding the SCCP packet. */
		DOCSIS                     = 0x008FU, /*!< DOCSIS MAC frames, as described by the [DOCSIS 3.1 MAC and Upper Layer Protocols Interface Specification](https://apps.cablelabs.com/specification/CM-SP-MULPIv3.1) or earlier specifications for MAC frames. */
		LinuxIrDA                  = 0x0090U, /*!< Linux-IrDA packets, with a [LINKTYPE_LINUX_IRDA header](https://www.tcpdump.org/linktypes/LINKTYPE_LINUX_IRDA.html), with the payload for IrDA frames beginning with by the IrLAP header as defined by [IrDA Data Specifications](https://www.irda.org/library-of-specs), including the IrDA Link Access Protocol specification. */
		/* 0x0091U - 0x0092U : Undefined, presumed reserved */
		User0                      = 0x0093U, /*!< Reserved for private use; See note above. */
		User1                      = 0x0094U, /*!< Reserved for private use; See note above. */
		User2                      = 0x0095U, /*!< Reserved for private use; See note above. */
		User3                      = 0x0096U, /*!< Reserved for private use; See note above. */
		User4                      = 0x0097U, /*!< Reserved for private use; See note above. */
		User5                      = 0x0098U, /*!< Reserved for private use; See note above. */
		User6                      = 0x0099U, /*!< Reserved for private use; See note above. */
		User7                      = 0x009AU, /*!< Reserved for private use; See note above. */
		User8                      = 0x009BU, /*!< Reserved for private use; See note above. */
		User9                      = 0x009CU, /*!< Reserved for private use; See note above. */
		User10                     = 0x009DU, /*!< Reserved for private use; See note above. */
		User11                     = 0x009EU, /*!< Reserved for private use; See note above. */
		User12                     = 0x009FU, /*!< Reserved for private use; See note above. */
		User13                     = 0x00A0U, /*!< Reserved for private use; See note above. */
		User14                     = 0x00A1U, /*!< Reserved for private use; See note above. */
		User15                     = 0x00A2U, /*!< Reserved for private use; See note above. */
		IEEE802_11_AVS             = 0x00A3U, /*!< [AVS monitor mode information](http://web.archive.org/web/20040803232023/http://www.shaftnet.org/~pizza/software/capturefrm.txt) followed by an 802.11 header. */
		/* 0x00A4U : Undefined, presumed reserved */
		BACNet_MS_TP               = 0x00A5U, /*!< BACnet MS/TP frames, as specified by section 9.3 MS/TP Frame Format of [ANSI/ASHRAE Standard 135, BACnet® - A Data Communication Protocol for Building Automation and Control Networks](http://www.ashrae.org/resources--publications/bookstore/standard-135), including the preamble and, if present, the Data CRC. */
		PPP_PPPD                   = 0x00A6U, /*!< PPP in HDLC-like encapsulation, like LINKTYPE_PPP_HDLC, but with the 0xff address byte replaced by a direction indication - 0x00 for incoming and 0x01 for outgoing. */
		/* 0x00A7U - 0x00A8U : Undefined, presumed reserved */
		GPRS_LLC                   = 0x00A9U, /*!< General Packet Radio Service Logical Link Control, as defined by [3GPP TS 04.64](https://portal.3gpp.org/desktopmodules/Specifications/SpecificationDetails.aspx?specificationId=224). */
		GPF_T                      = 0x00AAU, /*!< Transparent-mapped generic framing procedure, as specified by [ITU-T Recommendation G.7041/Y.1303](https://www.itu.int/rec/T-REC-G.7041/en). */
		GPF_F                      = 0x00ABU, /*!< Frame-mapped generic framing procedure, as specified by [ITU-T Recommendation G.7041/Y.1303](https://www.itu.int/rec/T-REC-G.7041/en). */
		/* 0x00ACU - 0x00B0U : Undefined, presumed reserved */
		LinuxLAPD                  = 0x00B1U, /*!< Link Access Procedures on the D Channel (LAPD) frames, as specified by [ITU-T Recommendation Q.920](https://www.itu.int/rec/T-REC-Q.920/en) and [ITU-T Recommendation Q.921](https://www.itu.int/rec/T-REC-Q.921/en), captured via vISDN, with a [LINKTYPE_LINUX_LAPD header](https://www.tcpdump.org/linktypes/LINKTYPE_LINUX_LAPD.html), followed by the Q.921 frame, starting with the address field. */
		/* 0x00B2U - 0x00B5U : Undefined, presumed reserved */
		MFR                        = 0x00B6U, /*!< [FRF.16.1](https://web.archive.org/web/20160427004354/https://www.broadband-forum.org/technical/download/FRF.16/frf16.1.pdf) Multi-Link Frame Relay frames, beginning with an [FRF.12](https://web.archive.org/web/20181021041543/https://www.broadband-forum.org/technical/download/FRF.12/frf12.pdf) Interface fragmentation format fragmentation header. */
		/* 0x00B7U - 0x00BAU : Undefined, presumed reserved */
		BluetoothHCI_H4            = 0x00BBU, /*!< Bluetooth HCI UART transport layer; the frame contains an HCI packet indicator byte, as specified by the UART Transport Layer portion of the most recent Bluetooth Core specification, followed by an HCI packet of the specified packet type, as specified by the Host Controller Interface Functional Specification portion of the most recent Bluetooth Core Specification. */
		/* 0x00BC : Undefined, presumed reserved */
		USBLinux                   = 0x00BDU, /*!< USB packets, beginning with a Linux USB header, as specified by the struct usbmon_packet in the Documentation/usb/usbmon.txt file in the Linux source tree. Only the first 48 bytes of that header are present. All fields in the header are in host byte order. When performing a live capture, the host byte order is the byte order of the machine on which the packets are captured. When reading a pcap file, the byte order is the byte order for the file, as specified by the file's magic number; when reading a pcapng file, the byte order is the byte order for the section of the pcapng file, as specified by the Section Header Block. */
		/* 0x00BEU - 0x00BFU : Undefined, presumed reserved */
		PPI                        = 0x00C0U, /*!< Per-Packet Information information, as specified by the Per-Packet Information Header Specification, followed by a packet with the LINKTYPE_ value specified by the pph_dlt field of that header. */
		/* 0x00C1U - 0x00C2U : Undefined, presumed reserved */
		IEEE802_15_4_WithFCS       = 0x00C3U, /*!< IEEE 802.15.4 Low-Rate Wireless Networks, with each packet having the FCS at the end of the frame. */
		SITA                       = 0x00C4U, /*!< Various link-layer types, with a pseudo-header, for SITA. */
		ERF                        = 0x00C5U, /*!< Various link-layer types, with a pseudo-header, for Endace DAG cards; encapsulates Endace ERF records. */
		/* 0x00C6U - 0x00C8U : Undefined, presumed reserved */
		BluetoothHCI_H4WithPhdr    = 0x00C9U, /*!< Bluetooth HCI UART transport layer; the frame contains a 4-byte direction field, in network byte order (big-endian), the low-order bit of which is set if the frame was sent from the host to the controller and clear if the frame was received by the host from the controller, followed by an HCI packet indicator byte, as specified by the UART Transport Layer portion of the most recent [Bluetooth Core specification](https://web.archive.org/web/20130314201541/https://www.bluetooth.org/Technical/Specifications/adopted.htm), followed by an HCI packet of the specified packet type, as specified by the Host Controller Interface Functional Specification portion of the most recent Bluetooth Core Specification. */
		AX25_KIDD                  = 0x00CAU, /*!< [AX.25](http://www.ax25.net/AX25.2.2-Jul%2098-2.pdf) packet, with a 1-byte [KISS](http://www.ax25.net/kiss.aspx) header containing a type indicator. */
		LAPD                       = 0x00CBU, /*!< Link Access Procedures on the D Channel (LAPD) frames, as specified by [ITU-T Recommendation Q.920](https://www.itu.int/rec/T-REC-Q.920/en) and [ITU-T Recommendation Q.921](https://www.itu.int/rec/T-REC-Q.921/en), starting with the address field, with no pseudo-header. */
		PPPWithDir                 = 0x00CCU, /*!< PPP, as per [RFC 1661](https://tools.ietf.org/html/rfc1661) and [RFC 1662](https://tools.ietf.org/html/rfc1662), preceded with a one-byte pseudo-header with a zero value meaning "received by this host" and a non-zero value meaning "sent by this host"; if the first 2 bytes are 0xff and 0x03, it's PPP in HDLC-like framing, with the PPP header following those two bytes, otherwise it's PPP without framing, and the packet begins with the PPP header. The data in the frame is not octet-stuffed or bit-stuffed. */
		CiscoHDLCWithDir           = 0x00CDU, /*!< Cisco PPP with HDLC framing, as per [section 4.3.1 of RFC 1547](https://tools.ietf.org/html/rfc1547#section-4.3.1), preceded with a one-byte pseudo-header with a zero value meaning "received by this host" and a non-zero value meaning "sent by this host". */
		FrameRelayWithDir          = 0x00CEU, /*!< Frame Relay LAPF frames, beginning with a one-byte pseudo-header with a zero value meaning "received by this host" (DCE->DTE) and a non-zero value meaning "sent by this host" (DTE->DCE), followed by an [ITU-T Recommendation Q.922](https://www.itu.int/rec/T-REC-Q.922/en/) LAPF header starting with the address field, and without an FCS at the end of the frame. */
		LAPBWithDir                = 0x00CFU, /*!< Link Access Procedure, Balanced (LAPB), as specified by [ITU-T Recommendation X.25](https://www.itu.int/rec/T-REC-X.25/en), preceded with a one-byte pseudo-header with a zero value meaning "received by this host" (DCE->DTE) and a non-zero value meaning "sent by this host" (DTE->DCE). */
		/* 0x00D0U : Undefined, presumed reserved */
		Ipmb_linux                 = 0x00D1U, /*!< [IPMB](https://www.intel.com/content/www/us/en/products/docs/servers/ipmi/ipmi-second-gen-interface-spec-v2-rev1-1.html) over an [I2C](https://www.nxp.com/docs/en/user-guide/UM10204.pdf) circuit, with a [Linux-specific pseudo-header](https://www.tcpdump.org/linktypes/LINKTYPE_IPMB_LINUX.html). */
		/* 0x00D2U - 0x00D6U : Undefined, presumed reserved */
		IEEE802_15_4_nonask_phy    = 0x00D7U, /*!< [IEEE 802.15.4](https://ieeexplore.ieee.org/document/7460875/) Low-Rate Wireless Networks, with each packet having the FCS at the end of the frame, and with the PHY-level data for the O-QPSK, BPSK, GFSK, MSK, and RCC DSS BPSK PHYs (4 octets of 0 as preamble, one octet of SFD, one octet of frame length + reserved bit) preceding the MAC-layer data (starting with the frame control field). */
		/* 0x00D9U - 0x00DBU : Undefined, presumed reserved */
		UUSBLinuxMmapped           = 0x00DCU, /*!< USB packets, beginning with a Linux USB header, as specified by the struct usbmon_packet in the Documentation/usb/usbmon.txt file in the Linux source tree. All 64 bytes of the header are present. All fields in the header are in host byte order. When performing a live capture, the host byte order is the byte order of the machine on which the packets are captured. When reading a pcap file, the byte order is the byte order for the file, as specified by the file's magic number; when reading a pcapng file, the byte order is the byte order for the section of the pcapng file, as specified by the Section Header Block. For isochronous transfers, the ndesc field specifies the number of isochronous descriptors that follow. */
		/* 0x00DDU - 0x00DFU : Undefined, presumed reserved */
		FC2                        = 0x00E0U, /*!< Fibre Channel FC-2 frames, beginning with a Frame_Header. */
		FC2WithFrameDelims         = 0x00E1U, /*!< Fibre Channel FC-2 frames, beginning an encoding of the SOF, followed by a Frame_Header, and ending with an encoding of the SOF. The encodings represent the frame delimiters as 4-byte sequences representing the corresponding ordered sets, with K28.5 represented as 0xBC, and the D symbols as the corresponding byte values; for example, SOFi2, which is K28.5 - D21.5 - D1.2 - D21.2, is represented as 0xBC 0xB5 0x55 0x55. */
		IPNET                      = 0x00E2U, /*!< [Solaris ipnet pseudo-header](https://www.tcpdump.org/linktypes/LINKTYPE_IPNET.html), followed by an IPv4 or IPv6 datagram. */
		CANSocketCan               = 0x00E3U, /*!< CAN (Controller Area Network) frames, with [a pseudo-header](https://www.tcpdump.org/linktypes/LINKTYPE_CAN_SOCKETCAN.html) followed by the frame payload. */
		IpV4                       = 0x00E4U, /*!< Raw IPv4; the packet begins with an IPv4 header. */
		IpV6                       = 0x00E5U, /*!< Raw IPv6; the packet begins with an IPv6 header. */
		IEEE802_15_4_nofcs         = 0x00E6U, /*!< [IEEE 802.15.4](https://ieeexplore.ieee.org/document/7460875/) Low-Rate Wireless Network, without the FCS at the end of the frame. */
		DBus                       = 0x00E7U, /*!< Raw [D-Bus](http://www.freedesktop.org/wiki/Software/dbus) [messages](https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-messages), starting with the endianness flag, followed by the message type, etc., but without the [authentication handshake](https://dbus.freedesktop.org/doc/dbus-specification.html#auth-protocol) before the message sequence. */
		/* 0x00E8U - 0x00EAU : Undefined, presumed reserved */
		DVB_CI                     = 0x00EBU, /*!< DVB-CI (DVB Common Interface for communication between a PC Card module and a DVB receiver), with the message format specified by the [PCAP format for DVB-CI specification](https://www.kaiser.cx/pcap-dvbci.html). */
		Mux27010                   = 0x00ECU, /*!< [Variant of 3GPP TS 27.010 multiplexing protocol](https://www.tcpdump.org/linktypes/LINKTYPE_MUX27010.html) (similar to, but not the same as, 27.010). */
		Stanag5066DPDU             = 0x00EDU, /*!< D_PDUs as described by NATO standard STANAG 5066, starting with the synchronization sequence, and including both header and data CRCs. The current version of STANAG 5066 is backwards-compatible with [the 1.0.2 version](https://web.archive.org/web/20051004082010/http://www.armymars.net/ArmyMARS/HF-Email/resources/stanag5066.pdf), although newer versions are classified. */
		/* 0x00EEU : Undefined, presumed reserved */
		NFLog                      = 0x00EFU, /*!< [Linux netlink NETLINK NFLOG socket log messages](https://www.tcpdump.org/linktypes/LINKTYPE_NFLOG.html). */
		NetAnalyzer                = 0x00F0U, /*!< [Pseudo-header for Hilscher Gesellschaft für Systemautomation mbH netANALYZER devices](https://www.tcpdump.org/linktypes/LINKTYPE_NETANALYZER.html), followed by an Ethernet frame, beginning with the MAC header and ending with the FCS. */
		NetAnalyzerTransparent     = 0x00F1U, /*!< [Pseudo-header for Hilscher Gesellschaft für Systemautomation mbH netANALYZER devices](https://www.tcpdump.org/linktypes/LINKTYPE_NETANALYZER.html), followed by an Ethernet frame, beginning with the preamble, SFD, and MAC header, and ending with the FCS. */
		IPoIB                      = 0x00F2U, /*!< IP-over-InfiniBand, as specified by [RFC 4391 section 6](https://tools.ietf.org/html/rfc4391#section-6). */
		MPEG2Transport             = 0x00F3U, /*!< MPEG-2 Transport Stream transport packets, as specified by ISO 13818-1/[ITU-T Recommendation H.222.0](https://www.itu.int/rec/T-REC-H.222.0) (see table 2-2 of section 2.4.3.2 "Transport Stream packet layer"). */
		NG40                       = 0x00F4U, /*!< [Pseudo-header for ng4T GmbH's UMTS Iub/Iur-over-ATM and Iub/Iur-over-IP format as used by their ng40 protocol tester](https://www.tcpdump.org/linktypes/LINKTYPE_NG40.html), followed by frames for the Frame Protocol as specified by [3GPP TS 25.427](https://portal.3gpp.org/desktopmodules/Specifications/SpecificationDetails.aspx?specificationId=1201) for dedicated channels and [3GPP TS 25.435](https://portal.3gpp.org/desktopmodules/Specifications/SpecificationDetails.aspx?specificationId=1207) for common/shared channels in the case of ATM AAL2 or UDP traffic, by SSCOP packets as specified by [ITU-T Recommendation Q.2110](https://www.itu.int/rec/T-REC-Q.2110-199407-I/en) for ATM AAL5 traffic, and by NBAP packets for SCTP traffic. */
		NFC_LLCP                   = 0x00F5U, /*!< [Pseudo-header for NFC LLCP packet captures](https://www.tcpdump.org/linktypes/LINKTYPE_NFC_LLCP.html), followed by frame data for the LLCP Protocol as specified by [NFCForum-TS-LLCP_1.1](http://www.nfc-forum.org/specs/spec_list/). */
		/* 0x00F6U : Undefined, presumed reserved */
		InfiniBand                 = 0x00F7U, /*!< Raw InfiniBand frames, starting with the Local Routing Header, as specified in Chapter 5 "Data packet format" of [InfiniBand™ Architectural Specification Release 1.2.1 Volume 1 - General Specifications](https://www.afs.enea.it/asantoro/V1r1_2_1.Release_12062007.pdf). */
		SCTP                       = 0x00F8U, /*!< SCTP packets, as defined by [RFC 4960](https://tools.ietf.org/html/rfc4960), with no lower-level protocols such as IPv4 or IPv6. */
		USBPcap                    = 0x00F9U, /*!< USB packets, beginning with a [USBPcap header](https://desowin.org/usbpcap/captureformat.html). */
		RTACSerial                 = 0x00FAU, /*!< [Serial-line packet header for the Schweitzer Engineering Laboratories "RTAC" product](https://www.tcpdump.org/linktypes/LINKTYPE_RTAC_SERIAL.html), followed by a payload for one of a number of industrial control protocols. */
		BluetoothLELL              = 0x00FBU, /*!< Bluetooth Low Energy air interface Link Layer packets, in the format described in section 2.1 "PACKET FORMAT" of volume 6 of the [Bluetooth Specification Version 4.0](https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=229737) (see PDF page 2200), but without the Preamble. */
		/* 0x00FCU : Undefined, presumed reserved */
		Netlink                    = 0x00FDU, /*!< [Linux Netlink capture encapsulation](https://www.tcpdump.org/linktypes/LINKTYPE_NETLINK.html). */
		BluetoothLinuxMonitor     = 0x00FEU, /*!< [Bluetooth Linux Monitor encapsulation of traffic for the BlueZ stack](https://www.tcpdump.org/linktypes/LINKTYPE_BLUETOOTH_LINUX_MONITOR.html). */
		Bluetooth_bredr_bb         = 0x00FFU, /*!< [Bluetooth Basic Rate and Enhanced Data Rate baseband packets](http://www.whiterocker.com/bt/LINKTYPE_BLUETOOTH_BREDR_BB.html). */
		BluetoothLELLWithPHDR      = 0x0100U, /*!< [Bluetooth Low Energy link-layer packets](https://www.tcpdump.org/linktypes/LINKTYPE_BLUETOOTH_LE_LL_WITH_PHDR.html). */
		PROFIBUSDatalink           = 0x0101U, /*!< PROFIBUS data link layer packets, as specified by IEC standard 61158-4-3, beginning with the start delimiter, ending with the end delimiter, and including all octets between them. */
		PKTAP                      = 0x0102U, /*!< [Apple PKTAP capture encapsulation](https://www.tcpdump.org/linktypes/LINKTYPE_PKTAP.html). */
		EoPON                      = 0x0103U, /*!< Ethernet-over-passive-optical-network packets, starting with the last 6 octets of the modified preamble as specified by 65.1.3.2 "Transmit" in Clause 65 of Section 5 of [IEEE 802.3](https://ieeexplore.ieee.org/document/7428776/), followed immediately by an Ethernet frame. */
		IPMI_hpm_2                 = 0x0104U, /*!< IPMI trace packets, as specified by Table 3-20 "Trace Data Block Format" in the [PICMG HPM.2 specification](http://www.picmg.org/v2internal/specifications2.cfm?thetype=One&thebusid=12). The time stamps for packets in this format must match the time stamps in the Trace Data Blocks. */
		ZWaveR1R2                  = 0x0105U, /*!< [Z-Wave RF profile R1 and R2 packets](https://www.tcpdump.org/linktypes/LINKTYPE_ZWAVE_R1_R2.html), as specified by [ITU-T Recommendation G.9959](https://www.itu.int/rec/T-REC-G.9959/en), with some MAC layer fields moved. */
		ZWaveR3                    = 0x0106U, /*!< [Z-Wave RF profile R3 packets](https://www.tcpdump.org/linktypes/LINKTYPE_ZWAVE_R3.html), as specified by [ITU-T Recommendation G.9959](https://www.itu.int/rec/T-REC-G.9959/en), with some MAC layer fields moved. */
		WattStopperDLM             = 0x0107U, /*!< Formats for [WattStopper Digital Lighting Management](https://www.tcpdump.org/linktypes/LINKTYPE_WATTSTOPPER_DLM.html) (DLM) and Legrand Nitoo Open protocol common packet structure captures. */
		ISO14443                   = 0x0108U, /*!< Messages between ISO 14443 contactless smartcards (Proximity Integrated Circuit Card, PICC) and card readers (Proximity Coupling Device, PCD), with the message format specified by [the PCAP format for ISO14443 specification](https://www.kaiser.cx/pcap-iso14443.html). */
		RDS                        = 0x0109U, /*!< Radio data system (RDS) groups, as per IEC 62106, encapsulated in [this form](https://www.tcpdump.org/linktypes/LINKTYPE_RDS.html). */
		USBDarwin                  = 0x010AU, /*!< USB packets, beginning with a [Darwin (macOS, etc.) USB header](https://www.tcpdump.org/linktypes/LINKTYPE_USB_DARWIN.html). */
		/* 0x010BU : Undefined, presumed reserved */
		SDLC                       = 0x010CU, /*!< SDLC packets, as specified by Chapter 1, "DLC Links", section "Synchronous Data Link Control (SDLC)" of [Systems Network Architecture Formats, GA27-3136-20](https://publibfp.dhe.ibm.com/epubs/pdf/d50a5007.pdf), without the flag fields, zero-bit insertion, or Frame Check Sequence field, containing SNA path information units (PIUs) as the payload. */
		/* 0x010DU : Undefined, presumed reserved */
		LoRaTap                    = 0x010EU, /*!< [LoRaTap pseudo-header](https://github.com/eriknl/LoRaTap/blob/master/README.md), followed by the payload, which is typically the PHYPayload from the [LoRaWan specification](https://lora-alliance.org/lorawan-for-developers). */
		VSOCK                      = 0x010FU, /*!< [Protocol for communication between host and guest machines in VMware and KVM hypervisors](https://www.tcpdump.org/linktypes/LINKTYPE_VSOCK.html). */
		NordicBLE                  = 0x0110U, /*!< Messages to and from a Nordic Semiconductor nRF Sniffer for Bluetooth LE packets, beginning with a [pseudo-header](https://www.tcpdump.org/linktypes/LINKTYPE_NORDIC_BLE.html). */
		DOCSIS31_XRA31             = 0x0111U, /*!< DOCSIS packets and bursts, preceded by a [pseudo-header giving metadata about the packet](https://support.excentis.com/index.php?/Knowledgebase/Article/View/159). */
		Ethernet_mpacket           = 0x0112U, /*!< mPackets, as specified by [IEEE 802.3br](https://ieeexplore.ieee.org/document/7900321/) Figure 99-4, starting with the preamble and always ending with a CRC field. */
		DisplayportAux             = 0x0113U, /*!< DisplayPort AUX channel monitoring data as specified by VESA DisplayPort(DP) Standard preceded by a [pseudo-header](https://www.tcpdump.org/linktypes/LINKTYPE_DISPLAYPORT_AUX.html). */
		LinuxSLL2                  = 0x0114U, /*!< [Linux "cooked" capture encapsulation v2](https://www.tcpdump.org/linktypes/LINKTYPE_LINUX_SLL2.html). */
		/* 0x0115U : Undefined, presumed reserved */
		Openvizsla                 = 0x0116U, /*!< [Openvizsla FPGA-based USB sniffer](https://www.tcpdump.org/linktypes/LINKTYPE_OPENVIZSLA.html). */
		EBHSCR                     = 0x0117U, /*!< [Elektrobit High Speed Capture and Replay (EBHSCR) format](https://web.archive.org/web/20200502132428/https://d23rjziej2pu9i.cloudfront.net/wp-content/uploads/2019/03/04164842/EB_High_Speed_Capture_Replay_protocol.pdf). */
		VPPDispatch                = 0x0118U, /*!< Records in traces from the [http://fd.io](https://fd.io/) VPP graph dispatch tracer, in [the the graph dispatcher trace format](https://fdio-vpp.readthedocs.io/en/latest/gettingstarted/developers/vnet.html#graph-dispatcher-pcap-tracing). */
		DSATagBRCM                 = 0x0119U, /*!< Ethernet frames, with a [switch tag](https://www.tcpdump.org/linktypes/LINKTYPE_DSA_TAG_BRCM.html) inserted between the source address field and the type/length field in the Ethernet header. */
		DSATagBRCMPrepend          = 0x011AU, /*!< Ethernet frames, with a [switch tag](https://www.tcpdump.org/linktypes/LINKTYPE_DSA_TAG_BRCM_PREPEND.html) inserted before the destination address in the Ethernet header. */
		IEEE802_15_4_Tap           = 0x011BU, /*!< [IEEE 802.15.4](https://ieeexplore.ieee.org/document/7460875/) Low-Rate Wireless Networks, with a [pseudo-header containing TLVs with metadata](https://github.com/jkcko/ieee802.15.4-tap/blob/master/IEEE%20802.15.4%20TAP%20Link%20Type%20Specification.pdf) preceding the 802.15.4 header. */
		DSATagDSA                  = 0x011CU, /*!< Ethernet frames, with a [switch tag](https://www.tcpdump.org/linktypes/LINKTYPE_DSA_TAG_DSA.html) inserted between the source address field and the type/length field in the Ethernet header. */
		DSATagEDSA                 = 0x011DU, /*!< Ethernet frames, with a programmable Ethernet type [switch tag](https://www.tcpdump.org/linktypes/LINKTYPE_DSA_TAG_EDSA.html) inserted between the source address field and the type/length field in the Ethernet header. */
		ELEE                       = 0x011EU, /*!< Payload of lawful intercept packets using [the ELEE protocol](http://xml2rfc.tools.ietf.org/cgi-bin/xml2rfc.cgi?url=http://socket.hr/draft-dfranusic-opsawg-elee-00.xml&modeAsFormat=html/ascii). The packet begins with the ELEE header; it does not include any transport-layer or lower-layer headers for protcols used to transport ELEE packets. */
		ZWaveSerial                = 0x011FU, /*!< Serial frames transmitted between a host and a Z-Wave chip over an RS-232 or USB serial connection, as described in section 5 of the [Z-Wave Serial API Host Application Programming Guide](https://www.silabs.com/documents/login/user-guides/INS12350-Serial-API-Host-Appl.-Prg.-Guide.pdf). */
		USB2                       = 0x0120U, /*!< USB 2.0, 1.1, or 1.0 packet, beginning with a PID, as described by Chapter 8 "Protocol Layer" of the the [Universal Serial Bus Specification Revision 2.0](https://www.usb.org/document-library/usb-20-specification). */
		ATSC_ALP                   = 0x0121U, /*!< ATSC Link-Layer Protocol frames, as described in section 5 of the A/330 Link-Layer Protocol specification, found at the [ATSC 3.0 standards page](https://www.atsc.org/atsc-documents/type/3-0-standards/), beginning with a Base Header. */
	};

	/*! \struct libnokogiri::version_t
		\brief pcap version information

		This structure is just a glorified container for holding version information.

		Both pcapng and the standard pcap formats both use this.
	*/
	struct version_t final {
	private:
		std::uint16_t _major;
		std::uint16_t _minor;
	public:
		constexpr version_t() noexcept :
			_major{0U}, _minor{0U}
			{ /* NOP */ }

		constexpr version_t(std::uint16_t major, std::uint16_t minor) noexcept :
			_major{major}, _minor{minor}
			{ /* NOP */ }

		version_t(std::nullptr_t) noexcept { /* NOP */ }

		/*! Gets the major version of the section header */
		[[nodiscard]]
		std::uint32_t major() const noexcept { return _major; }
		/*! Gets the minor version of the section header */
		[[nodiscard]]
		std::uint32_t minor() const noexcept { return _minor; }

		/*! Check if two versions are equal */
		[[nodiscard]]
		bool operator==(const version_t& ver) const noexcept {
			return (_major == ver.major() && _minor == ver.minor());
		}
		/*! Check if two versions are not equal */
		[[nodiscard]]
		bool operator!=(const version_t& ver)  const noexcept {
			return !operator==(ver);
		}

		/*! Check if one version is greater than another */
		[[nodiscard]]
		bool operator>(const version_t& ver) const noexcept {
			return (
				(_major > ver.major() && _minor >= ver.minor()) ||
				(_major >= ver.major() && _minor > ver.minor()));
		}
		/*! Check if one version is greater than or equal to another */
		[[nodiscard]]
		bool operator>=(const version_t& ver) const noexcept {
			return (operator>(ver) || operator==(ver));
		}

		/*! Check if one version is less than another */
		[[nodiscard]]
		bool operator<(const version_t& ver) const noexcept {
			return (
				(_major < ver.major() &&  _minor <= ver.minor()) ||
				(_minor <= ver.major() && _minor < ver.minor()));
		}
		/*! Check if one version is less than or equal to another */
		[[nodiscard]]
		bool operator<=(const version_t& ver) const noexcept {
			return (operator<(ver) || operator==(ver));
		}
	};


	/* IEC Units*/
	/*! Expand value to Kibibytes (n * 1024) */
	constexpr uint64_t operator ""_KiB(const unsigned long long value) noexcept { return uint64_t(value) * 1024; }
	/*! Expand value to Mebibytes (n * 1024^2) */
	constexpr uint64_t operator ""_MiB(const unsigned long long value) noexcept { return uint64_t(value) * 1048576; }
	/*! Expand value to Gibibytes (n * 1024^3) */
	constexpr uint64_t operator ""_GiB(const unsigned long long value) noexcept { return uint64_t(value) * 1073741824; }
	/*! Expand value to Tebibytes (n * 1024^4) */
	constexpr uint64_t operator ""_TiB(const unsigned long long value) noexcept { return uint64_t(value) * 1099511627776; }
	/*! Expand value to Pebibytes (n * 1024^5) */
	constexpr uint64_t operator ""_PiB(const unsigned long long value) noexcept { return uint64_t(value) * 1125899906842624; }

	/* SI Units */
	/*! Expand value to Kilobytes (n * 1000) */
	constexpr uint64_t operator ""_KB(const unsigned long long value) noexcept { return uint64_t(value) * 1000; }
	/*! Expand value to Megabytes (n * 1000^2) */
	constexpr uint64_t operator ""_MB(const unsigned long long value) noexcept { return uint64_t(value) * 1000000; }
	/*! Expand value to Gigabytes (n * 1000^3) */
	constexpr uint64_t operator ""_GB(const unsigned long long value) noexcept { return uint64_t(value) * 1000000000; }
	/*! Expand value to Terabytes (n * 1000^4) */
	constexpr uint64_t operator ""_TB(const unsigned long long value) noexcept { return uint64_t(value) * 1000000000000; }
	/*! Expand value to Petabytes (n * 1000^5) */
	constexpr uint64_t operator ""_PB(const unsigned long long value) noexcept { return uint64_t(value) * 1000000000000000; }
}

#endif /* LIBNOKOGIRI_COMMON_HH */
