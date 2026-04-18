/* AMALGAMATED OUTPUT START (entry: /home/josh/tor-connect/torlib/inc.c) */
/* BEGIN FILE: /home/josh/tor-connect/torlib/inc.c */
/* BEGIN FILE: /home/josh/tor-connect/torlib/Cell.h */
/*-
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * Copyright (c) 2009, Moxie Marlinspike
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



#pragma once
/* BEGIN FILE: /home/josh/tor-connect/torlib/Util.h */
 
#include <memory>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <locale>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <tuple>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <array>
#include <deque>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <iterator>
#include <numeric>
#include <time.h>
#include <sys/types.h>

#define NOMINMAX // added for compartibility with OpenSSL 3.x as it includes windows.h without NOMINMAX and ruins compilation -- sowle
#include <openssl/aes.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/dh.h>
#include <openssl/bn.h>
#include <openssl/modes.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/assert.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/format.hpp>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t limb;
typedef unsigned char unc;
typedef unsigned int uni;

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>
namespace pl = net::placeholders;
namespace sys = boost::system;
namespace ssl = boost::asio::ssl;
namespace fs = boost::filesystem;
using timer = boost::asio::steady_timer;
using log_lv = boost::log::trivial::severity_level;
using sec = boost::posix_time::seconds;


using std::string;
using std::vector;
using std::shared_ptr;
using std::array;
using std::map;
using std::copy;
using std::reverse;
using std::make_tuple;
//using std::make_unique; 
using std::tuple;
using std::min;
using std::ifstream;
using std::get;
using std::stringstream;
using std::pair;
using std::unique_ptr;
using std::make_shared;

#define  TEST_ERR(err,connectFunc) if (err) {net::post(net::detail::bind_handler(connectFunc, err));return;}

class Util {
private:
 public:
  static void Int64ToArrayBigEndian(unc*a, u64 i);
  static void Int32ToArrayBigEndian(unc*a, u32 i);
  static void Int16ToArrayBigEndian(unc*a, u16 i);
  static uint16_t BigEndianArrayToShort(unc*buf);
  static uint32_t BigEndianArrayToInt(unc*buf);
  static void HexDump(unc* buf, size_t length);
  static void Base16Encode(char *dest, size_t destlen, const char *src, size_t srclen);
  static uint16_t GetRandomId();
  static uint32_t GetRandom();  
  static uint8_t GetRandom8();
  static bool IsBase64(unsigned char c);
  static string Base64Decode(string const& encoded_string);
};

/* END FILE: /home/josh/tor-connect/torlib/Util.h */

#define POSITION_COMMAND 4
#define POSITION_PAYLOAD_SIZE 5
#define POSITION_PAYLOAD 7
#define PAYLOAD_SIZE 509
#define CELL_SIZE 514


//
// 5.4.
//
// The payload of a RELAY_TRUNCATED or DESTROY cell contains a single octet,
// describing why the circuit is being closed or truncated.
//

enum cell_destroy_reason : uint8_t
{
	none = 0, // no reason given
	protocol = 1, // tor protocol violation
	internal = 2, // internal error
	requested = 3, // a client sent a TRUNCATE command
	hibernating = 4, // not currently operating; trying to save bandwidth
	resource_limit = 5, // out of memory, sockets, or circuit IDs
	connection_failed = 6, // unable to reach relay
	onion_router_identity = 7, // connected to relay, but its OR identity was not as expected
	onion_router_connection_closed = 8, // the OR connection that was carrying this circuit died
	finished = 9, // the circuit has expired for being dirty or old
	timeout = 10, // circuit construction took too long
	destroyed = 11, // the circuit was destroyed w/o client TRUNCATE
	no_such_service = 12, // request for unknown hidden service
};

enum cell_command : uint8_t
{
	//
	// cell commands.
	//
	padding = 0,
	create = 1,
	created = 2,
	relay = 3,
	destroy = 4,
	create_fast = 5,
	created_fast = 6,
	versions = 7,
	netinfo = 8,
	relay_early = 9,
	create2 = 10,
	created2 = 11,

	//
	// relay commands.
	//
	relay_begin = 1,
	relay_data = 2,
	relay_end = 3,
	relay_connected = 4,
	relay_sendme = 5,
	relay_extend = 6,
	relay_extended = 7,
	relay_truncate = 8,
	relay_truncated = 9,
	relay_drop = 10,
	relay_resolve = 11,
	relay_resolved = 12,
	relay_begin_dir = 13,
	relay_extend2 = 14,
	relay_extended2 = 15,

	//
	// rendezvous commands.
	//
	relay_command_establish_intro = 32,
	relay_command_establish_rendezvous = 33,
	relay_command_introduce1 = 34,
	relay_command_introduce2 = 35,
	relay_command_rendezvous1 = 36,
	relay_command_rendezvous2 = 37,
	relay_command_intro_established = 38,
	relay_command_rendezvous_established = 39,
	relay_command_introduce_ack = 40,

	//
	// variable-length cell commands.
	//
	vpadding = 128,
	certs = 129,
	auth_challenge = 130,
	authenticate = 131,
	authorize = 132,
};

class Cell {
protected:
	unc buffer[CELL_SIZE];
	size_t index;
public:

	Cell(u32 circuit_id, cell_command command);
	Cell();

	void Append(u16 val);
	void Append(u32 val);
	void Append(unc val);
	void Append(string& val);
	void Append(unc* segment, size_t length);

	u32 ReadInt();
	unc ReadByte();
	string ReadString();

	unc* GetBuffer();
	size_t GetBufferSize();

	unc* GetPayload();
	size_t GetPayloadSize();

	unc GetCommand();
	bool IsRelayCell();
	bool IsPaddingCell();
	bool IsVariableLen();
	unc* GetPayloadPart(int begin, int end);
	void SetCommand(u8 command);
	void SetId(u32 circuit_id);
	void SetPayloadSize(u16 payload_size);
  size_t GetCurentIndex();

	virtual ~Cell() {}
};

/* END FILE: /home/josh/tor-connect/torlib/Cell.h */
/* BEGIN FILE: /home/josh/tor-connect/torlib/Curve25519.h */
/*
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * Copyright 2008, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * curve25519-donna: Curve25519 elliptic curve, public key function
 *
 * http://code.google.com/p/curve25519-donna/
 *
 * Adam Langley <agl@imperialviolet.org>
 *
 * Derived from public domain C code by Daniel J. Bernstein <djb@cr.yp.to>
 *
 * More information about curve25519 can be found here
 *   http://cr.yp.to/ecdh.html
 *
 * djb's sample implementation of curve25519 is written in a special assembly
 * language called qhasm and uses the floating point registers.
 *
 * This is, almost, a clean room reimplementation from the curve25519 paper. It
 * uses many of the tricks described therein. Only the crecip function is taken
 * from the sample implementation.
 *
 * Copyright(c) 2016 Petr Benes https://github.com/wbenny/mini-tor
 * 
 */




#pragma once
#include <cstdint>

typedef uint8_t u8;
typedef int32_t s32;
typedef int64_t limb;

class Curve25519
{
private:
	static void fsum(limb* output, const limb* in);
	static void fdifference(limb* output, const limb* in);
	static void fscalar_product(limb* output, const limb* in, const limb scalar);
	static void fproduct(limb* output, const limb* in2, const limb* in);
	static void freduce_degree(limb* output);
	static inline limb div_by_2_26(const limb v);
	static inline limb div_by_2_25(const limb v);
	static void freduce_coefficients(limb* output);
	static void fmul(limb* output, const limb* in, const limb* in2);
	static void fsquare_inner(limb* output, const limb* in);
	static void fsquare(limb* output, const limb* in);
	static void fexpand(limb* output, const u8* input);
	static s32 s32_eq(s32 a, s32 b);
	static s32 s32_gte(s32 a, s32 b);
	static void fcontract(u8* output, limb* input_limbs);
	static void fmonty(limb* x2, limb* z2,  /* output 2Q */
		limb* x3, limb* z3,  /* output Q + Q' */
		limb* x, limb* z,    /* input Q */
		limb* xprime, limb* zprime,  /* input Q' */
		const limb* qmqp /* input Q - Q' */);
	static void swap_conditional(limb a[19], limb b[19], limb iswap);
	static void cmult(limb* resultx, limb* resultz, const u8* n, const limb* q);
	static void crecip(limb* out, const limb* z);
public:
	static int curve25519_donna(uint8_t* mypublic, const uint8_t* secret, const uint8_t* basepoint);

};

/* END FILE: /home/josh/tor-connect/torlib/Curve25519.h */
/* BEGIN FILE: /home/josh/tor-connect/torlib/HTTPClient.h */
/*-
* Copyright (c) 2021, Zano project, https://zano.org/
* Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 4. Neither the name of this program nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/



#pragma once

class HTTPClient : public std::enable_shared_from_this<HTTPClient>
{
    tcp::resolver resolver_;
    beast::tcp_stream stream_;
    
    beast::flat_buffer buffer_; // (Must persist between reads)
    http::request<http::empty_body> req_;    
    int timeout_op = 0;
    http::response<http::string_body> res_;
    bool error_operations;

    void OnResolve(beast::error_code ec, tcp::resolver::results_type results);
    void OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type);
    void OnWrite(beast::error_code ec, std::size_t bytes_transferred);
    void OnRead(beast::error_code ec, std::size_t bytes_transferred);
    void OnFail(beast::error_code ec, char const* what);

public:
    explicit HTTPClient(net::io_context& ioc) : resolver_(net::make_strand(ioc)), stream_(net::make_strand(ioc)) {}
    void RunClient(const string host, const int port, const string target, const int timeout = 0 , const int version = 11);
    string GetData();    
};
/* END FILE: /home/josh/tor-connect/torlib/HTTPClient.h */
/* BEGIN FILE: /home/josh/tor-connect/torlib/NetConnect.h */
/*-
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



#pragma once

#define BUFF_SIZE 1024

typedef std::function<void(const sys::error_code& error)> ConnectFunction;

class NetConnect
{
	tcp::resolver _resolver;
	ssl::stream<net::ip::tcp::socket> socket;
	net::streambuf request;
	net::streambuf response;

	void ResolvedComplete(ConnectFunction connectFunc, tcp::resolver::iterator endpoint_iter, const sys::error_code& err);
	bool VerifyCertificate(bool preverified, ssl::verify_context& ctx);
	
	void ConnectComplete(ConnectFunction connectFunc, const sys::error_code& err);
	void Handshake(ConnectFunction connectFunc, const sys::error_code& err);

	void SentVersionComplete(ConnectFunction connectFunc, const sys::error_code& err);
	void ReadVersionComplete(ConnectFunction connectFunc, const sys::error_code& err);
	void ReadVersionContentComplete(int len, ConnectFunction connectFunc, const sys::error_code& err);
	
	void ReadHederComplete1(ConnectFunction connectFunc, const sys::error_code& err);
	void ReadHederComplete2(ConnectFunction connectFunc, const sys::error_code& err);
	void ReadCertificatesComplete1(int len, ConnectFunction connectFunc, const sys::error_code& err);
	void ReadCertificatesComplete2(int len, ConnectFunction connectFunc, const sys::error_code& err);

	void ReadHederNetInfoComplete(ConnectFunction connectFunc, const sys::error_code& err);
	void ReadNetInfoComplete(int len, ConnectFunction connectFunc, const sys::error_code& err);

	void WriteCellComplete(ConnectFunction connectFunc, const sys::error_code& err);
	void ReadHeaderComplete(ConnectFunction connectFunc, shared_ptr<Cell> cell, const sys::error_code& err);
	void ReadCellComplete(ConnectFunction connectFunc, shared_ptr<Cell> cell, const sys::error_code& err);
	void DummyFunc(const sys::error_code& error);

	tuple<u32, u8, u16> ParsingHeader();
	void HexDumpResponse();
public:	
	NetConnect(net::io_service& io_service, ssl::context& context);
	void Connect(const string& host, int port, ConnectFunction connectFunc);
	void SendVersion(ConnectFunction connectFunc);
	void Close();
	tcp::endpoint GetLocalEndpoint();
	long GetEndpointLong();
	void WriteCell(Cell& cell, ConnectFunction connectFunc);
	void ReadCell(shared_ptr<Cell> cell, ConnectFunction connectFunc);	
	void ShutDown();
	~NetConnect();
};

/* END FILE: /home/josh/tor-connect/torlib/NetConnect.h */
/* BEGIN FILE: /home/josh/tor-connect/torlib/OnionRouter.h */
/*-
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#pragma once
/* BEGIN FILE: /home/josh/tor-connect/torlib/RelayCell.h */
/*-
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * Copyright (c) 2009, Moxie Marlinspike
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



#pragma once

#define TOR_MAX_CELL_PAYLOAD_DATA 117

#define DIGEST_OFFSET 10
#define DIGEST_LENGTH 4
#define RELAY_PAYLOAD_OFFSET 14
#define RELAY_BYTES_LEN 2
#define HANDSHAKE_DATA 18
#define RELAY_TYPE_OFFSET 5
#define STREAM_ID_OFFSET 7

#define MAX_PAYLOAD_LENGTH 512 - 14

#define RELAY_END_TYPE 0x03

class RelayCell :
    public Cell
{
private:
    void AppendData(u16 streamId, cell_command relay_command, size_t length);
public:
    RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command);
    RelayCell(u16 streamId, cell_command command, cell_command relay_command);
    RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command, unc* data, int length);
    RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command, string& data, bool nullTerminatd);
    RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command, unc payload);
    RelayCell(Cell& cell);
    void SetDigest(unc* digest);
    void GetDigest(unc* buf);
    unc* GetRelayPayload();
    int GetRelayPayloadLength();
    unc GetRelayType();
    bool IsRelayEnd();
    bool SetLengthRelayPayload(size_t size_data);
    u16 GetStreamId();
};

/* END FILE: /home/josh/tor-connect/torlib/RelayCell.h */

static constexpr uint8_t const_server[] = {
  'S', 'e', 'r', 'v', 'e', 'r'
};

static constexpr uint8_t const_protoid[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1'
};

static constexpr uint8_t const_t_mac[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1',
  ':', 'm', 'a', 'c'
};

static constexpr uint8_t const_t_key[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1',
  ':', 'k', 'e', 'y', '_', 'e', 'x', 't', 'r', 'a', 'c', 't'
};

static constexpr uint8_t const_t_verify[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1',
  ':', 'v', 'e', 'r', 'i', 'f', 'y'
};

static constexpr uint8_t const_m_expand[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1',
  ':', 'k', 'e', 'y', '_', 'e', 'x', 'p', 'a', 'n', 'd'
};


#define TOTAL_KEY_MATERIAL (20*3+16*2)
#define DIGEST_LEN 20
#define KEY_LEN 128/8

#define KEY_SIZE 32

class OnionRouter
{
private:
	SHA_CTX forward_digest;
	SHA_CTX backward_digest;

	AES_KEY forward_key;
	AES_KEY backward_key;

	unc forward_iv[AES_BLOCK_SIZE];
	unc forward_ec[AES_BLOCK_SIZE];
	uni forward_num;

	unc back_iv[AES_BLOCK_SIZE];
	unc back_ec[AES_BLOCK_SIZE];
	uni back_num;
	
	int identity_key_len=0;
	RSA* GetKey(std::string current_key);

	pair<vector<u8>, vector<u8>> pair_key;
	pair<vector<u8>, vector<u8>> pair_key2;
	
	const u8 basepoint_9[32] = { 9 };

	void AesEncrypt(Cell& cell, AES_KEY* key, unc* iv, unc* ec, uni* num);	
	void SetDigest(RelayCell& cell);
	void VerifyDigest(RelayCell& cell);
	void CalculateDigest(SHA_CTX* digest, RelayCell& cell, unc* result);
	bool GeneratKeyMaterialUnc(unc* handshake_data, unc* other_public_key, unc* verify);	
public:
	OnionRouter();
	string nickname;
	string identity;
	string digest;
	string publication_date;
	string publication_time;
	string ip;
	int or_port;
	int dir_port;
	//
	// router_status_entry_r_item_count = 9
	//
	int item_count;

	string onion_key;
	string signing_key;
	string ntor_onion_key;
	string service_key; // for introduction point

	vector<u8> key_material;
	
	string GetBase16EncodedIdentity(string identity_key);
	bool SetOnionKey(string current_key);
	bool SetSignigKey(string current_key);
	bool SetNtorOnionKey(string current_key);
	bool SetServiceKey(string current_key);
	void GeneratPairKeys();
	u8* GetPublicKey();
	size_t GetPublicKeySize();
	bool GeneratKeyMaterial(shared_ptr<Cell> node);
	bool GeneratKeyMaterial(RelayCell& node);
	void SetKeyMaterial(unc* key_material);
	void Encrypt(RelayCell& cell, bool set_digest=true);
	void Decrypt(RelayCell& cell, bool verify_digest=true);
	
	string ConvertToString(char* a, int size);

};

/* END FILE: /home/josh/tor-connect/torlib/OnionRouter.h */
/* BEGIN FILE: /home/josh/tor-connect/torlib/Parser.h */
/*-
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * Copyright (c) 2016 Petr Benes https://github.com/wbenny/mini-tor
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#pragma once

enum class entry_type
{
    entry_r = 'r',
    entry_a = 'a',
    entry_s = 's',
    entry_v = 'v',
    entry_w = 'w',
    entry_p = 'p',
};

enum class entry_r_type
{
    //
    // start counting from 1,
    // because there is the "r" control word
    // on the index 0.
    //
    entry_r_nickname = 1,
    entry_r_identity,
    entry_r_digest,
    entry_r_publication_date,
    entry_r_publication_time,
    entry_r_ip,
    entry_r_or_port,
    entry_r_dir_port,

    //
    // router_status_entry_r_item_count = 9
    //
    entry_r_item_count,
};

enum class control_words
{
    not_determined,
    onion_key,
    signing_key,
    begin_public_key,
    end_public_key,
    ntor_onion_key,
};



class Parser
{
private:
	const string preamble_control_words = "valid-until";
    
    map<control_words, string> list_control_words;    
    const string directory_footer_control_words = "directory-footer";
    const string accept_ports = "accept";
    const string reject_ports = "reject";
    bool PortSearch(string in_line, int port_search);
public:
    vector<string> ParsString(const string& in_str, string del);
    tm GetValidUntil(const string& in_str);    
    shared_ptr<OnionRouter> GetOnionRouter(vector<string>& in_data, bool random, int or_port, int dir_port, string sh_ip, vector<string> flags, int search_port = 0);
    bool SetOnionRouterKeys(shared_ptr<OnionRouter> onion_node, vector<string>& in_data);
    vector<string> SearchOnionRouter(vector<string>& in_data, bool random, int or_port, int dir_port, string sh_ip, vector<string> flags, int search_port = 0);
    Parser();
};

/* END FILE: /home/josh/tor-connect/torlib/Parser.h */
/* BEGIN FILE: /home/josh/tor-connect/torlib/torlib.h */
/*-
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#pragma once

/* BEGIN FILE: /home/josh/tor-connect/torlib/tor_lib_iface.h */
/*-
* Copyright (c) 2021, Zano project, https://zano.org/
* Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
* Copyright (c) 2009, Moxie Marlinspike
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 4. Neither the name of this program nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/

#pragma once 
#include <vector>
#include <boost/log/trivial.hpp>

namespace tools
{
  namespace tor
  {

    struct t_transport_state_notifier
    {
      virtual void notify_state_change(const std::string& state_code, const std::string& details = std::string()) {};
    };


    struct t_tranport
    {
      virtual bool Init(boost::log::trivial::severity_level log_level = boost::log::trivial::info) = 0;
      virtual int Connect(const std::string ip, const int port, const int timeout = 0) = 0;
      virtual bool Close() = 0;
      virtual bool Send(const std::string& buff) = 0;
      virtual bool Receive(std::string& buff, const int timeout = 0) = 0;
      virtual void SetNotifier(t_transport_state_notifier* pnotifier) = 0;
      virtual void TransferExternalConsensus(std::vector<std::string>& local_consensus_data) = 0;
      virtual void WithdrawExternalConsensus(std::vector<std::string>& local_consensus_data) = 0;

    };

  }
}

#define TOR_LIB_STATE_INITIALIZING                  "STATE_INITIALIZING"
#define TOR_LIB_STATE_DOWNLOADING_CONSENSUS         "STATE_DOWNLOADING_CONSENSUS"
#define TOR_LIB_STATE_MAKING_TUNNEL_A               "STATE_MAKING_TUNNEL_A"
#define TOR_LIB_STATE_MAKING_TUNNEL_B               "STATE_MAKING_TUNNEL_B"
#define TOR_LIB_STATE_CREATING_STREAM               "STATE_CREATING_STREAM"
#define TOR_LIB_STATE_FAILED                        "STATE_FAILED"
#define TOR_LIB_STATE_SUCCESS                       "STATE_SUCCESS"








/* END FILE: /home/josh/tor-connect/torlib/tor_lib_iface.h */

enum class link_specifier_type : u8
{
	ipv4 = 0,
	ipv6 = 1,
	legacy_id = 2,
};


class TorLib: public tools::tor::t_tranport
{
private:	
	bool GetConsensus();
	bool GetKeysNode(int n_node);

  bool ConnectToNode(int n_node, int search_port = 0);

	//int connection_handle;
	vector<tuple<string, string, int, int>> DA;
	Parser parser;
	
	//string GetDataFromUrl(const string host, const int port, const string target);
	string GetDataFromUrlAsync(const string host, const int port, const string target);

	net::io_service io_service;
	shared_ptr<net::io_service::work> work;
	unique_ptr<net::deadline_timer> dtimer;
	unique_ptr<NetConnect> net_connect;
	bool operation_completed = false;
	bool error_last_operation = false;
	map<int, shared_ptr<OnionRouter>> onion_routers;

	bool SendNodeInfo(ConnectFunction connectFunc);
	void ReadNodeInfo(ConnectFunction connectFunc, const sys::error_code& err);
	void SendInfoComplete(ConnectFunction connectFunc, shared_ptr<Cell> node_info, const sys::error_code& err);	
	void LogErr(const sys::error_code& err);
	
	bool CreateNtor(int n_node, ConnectFunction connectFunc);
	void ReadCNtor(int n_node, ConnectFunction connectFunc, const sys::error_code& err);	
	void CreateNtorComplete(int n_node, ConnectFunction connectFunc, shared_ptr<Cell> node_info, const sys::error_code& err);

	bool CreateExtendNtor(int n_node, ConnectFunction connectFunc);
	void ReadExtendNtor(int n_node, ConnectFunction connectFunc, const sys::error_code& err);
	void CreateExtendNtorComplete(int n_node, ConnectFunction connectFunc, shared_ptr<Cell> node, const sys::error_code& err);

	bool CreateStream(int n_node, u16 id_stream, string host, int port, int timeout, ConnectFunction connectFunc);
	void ReadStreamNode(int n_node, ConnectFunction connectFunc, const sys::error_code& err);
	void CreateStreamComplete(int n_node, ConnectFunction connectFunc, shared_ptr<Cell> node, const sys::error_code& err);

	string stream_host;
	int stream_port = 0;
	int n_stream = 0;
  int timeout_global = 0;
	vector<std::string> data_consensus;
	string data_result;
  uint64_t last_consensus_receive_time = 0;

	u32 circuit_id = 1;
  tools::tor::t_transport_state_notifier notifier_dummy;
  tools::tor::t_transport_state_notifier* pnotifier;

	bool SendData(string reqest/* , ConnectFunction connectFunc*/);
	void ReadStreamData(int n_node, ConnectFunction connectFunc, const sys::error_code& err);
	void ReadStreamOne(int n_node, ConnectFunction connectFunc);		
	void ReadStreamComplete(int n_node, ConnectFunction connectFunc, shared_ptr<Cell> node, const sys::error_code& err);	
	void OnTimeout(const sys::error_code& err);
public:
  TorLib();
	~TorLib();
  // ------------- t_tranport ------------- 
  virtual bool Init(log_lv log_level = boost::log::trivial::info);
  virtual int Connect(const string ip, const int port, const int timeout=0);
  virtual bool Close();
  virtual bool Send(const string& path);
  virtual bool Receive(string& buff, const int timeout=0);
  virtual void SetNotifier(tools::tor::t_transport_state_notifier* pnotifier);
  virtual void TransferExternalConsensus(std::vector<std::string>& local_consensus_data);
  virtual void WithdrawExternalConsensus(std::vector<std::string>& local_consensus_data);
};

/* END FILE: /home/josh/tor-connect/torlib/torlib.h */
/* BEGIN FILE: /home/josh/tor-connect/torlib/tor_wrapper.h */
/*-
* Copyright (c) 2021, Zano project, https://zano.org/
* Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
* Copyright (c) 2009, Moxie Marlinspike
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 4. Neither the name of this program nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/

#pragma once 

#include <string>
#include <memory>

namespace tools
{
  namespace tor
  {
    class tor_transport
    {
    public:

      tor_transport();
      bool connect(const std::string& ip, int port, int timeout, int recev_timeout, const std::string& bind_ip);
      bool is_connected();
      bool send(const void* data, size_t sz);
      bool send(const std::string& buff);
      bool recv_n(std::string& buff, size_t sz);
      bool disconnect();
      void set_notifier(tools::tor::t_transport_state_notifier* pn);

    private:
      std::shared_ptr<tools::tor::t_tranport> m_ptransport;
      bool m_is_connected;
      int m_recev_timeout;
      std::string m_intermediate_buff;
      tools::tor::t_transport_state_notifier* m_pn;
    };
  }
}

/* END FILE: /home/josh/tor-connect/torlib/tor_wrapper.h */
/* BEGIN FILE: /home/josh/tor-connect/torlib/Cell.cpp */
/*-
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * Copyright (c) 2009, Moxie Marlinspike
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */




Cell::Cell(u32 circuit_id, cell_command command) {
	memset(buffer, 0, sizeof(buffer));
	Util::Int32ToArrayBigEndian(buffer, circuit_id);
	buffer[POSITION_COMMAND] = command;	
	index = POSITION_PAYLOAD_SIZE;
}
Cell::Cell() {
	index = POSITION_PAYLOAD;
}

unsigned char Cell::GetCommand() {
	return buffer[POSITION_COMMAND];
}

void Cell::Append(u16 val) {
	Util::Int16ToArrayBigEndian(buffer + index, val);
	index += 2;
}

void Cell::Append(u32 val) {
	Util::Int32ToArrayBigEndian(buffer + index, val);
	index += 4;
}

void Cell::Append(unsigned char val) {
	buffer[index++] = val;
}

void Cell::Append(string& val) {
	memcpy(buffer + index, reinterpret_cast<unc*>(const_cast<char*>(val.c_str())), val.length());
	index += val.length();
}

void Cell::Append(unc* segment, size_t length) {
	memcpy(buffer + index, segment, length);
	index += length;
}

u32 Cell::ReadInt() {
	u32 val = Util::BigEndianArrayToInt(buffer + index);
	index += 4;
	return val;
}

unsigned char Cell::ReadByte() {
	return buffer[index++];
}

string Cell::ReadString() {
	unc len = buffer[index++];
	string val(reinterpret_cast<const char*>(buffer)+ index, static_cast<size_t>(len));
	index += static_cast<int>(len);
	return val;
}

unsigned char* Cell::GetPayload() {
	return buffer + POSITION_PAYLOAD_SIZE;
}

size_t Cell::GetPayloadSize() {
	return sizeof(buffer) - POSITION_PAYLOAD_SIZE;
}

unsigned char* Cell::GetBuffer() {
	return buffer;
}

size_t Cell::GetBufferSize() {
	if (IsVariableLen())
		return sizeof(buffer);
	else return CELL_SIZE;
}
size_t Cell::GetCurentIndex() {
	return index;
}

bool Cell::IsRelayCell() {
	return GetCommand() == cell_command::relay;
}

bool Cell::IsPaddingCell() {
	return GetCommand() == cell_command::padding;
}
bool Cell::IsVariableLen() {
	return GetCommand() == cell_command::versions || GetCommand() >= 128;
}
unc* Cell::GetPayloadPart(int begin, int end) {	
	size_t len = end-begin;	
	if (len <= 0|| (begin+len)>=CELL_SIZE) return nullptr;
	unc* segment = static_cast<unc*>(malloc(len));
	memcpy(segment, buffer + begin, len);
	return segment;
}
void Cell::SetCommand(u8 command) {
	buffer[POSITION_COMMAND] = command;
}
void Cell::SetId(u32 circuit_id) {
	
	uint8_t d[4] = { 0 };
	for (int i = 0; i < 4; ++i)
		d[i] = (reinterpret_cast<u8*>(&circuit_id))[3 - i];	
	memcpy(buffer,d, 4);
}
void Cell::SetPayloadSize(u16 payload_size)
{
	uint8_t d[2] = { 0 };
	for (int i = 0; i < 2; ++i)
		d[i] = (reinterpret_cast<u8*>(&payload_size))[1 - i];
	memcpy(buffer + POSITION_PAYLOAD_SIZE, d, 2);
}
/* END FILE: /home/josh/tor-connect/torlib/Cell.cpp */
/* BEGIN FILE: /home/josh/tor-connect/torlib/Curve25519.cpp */
/* 
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * Copyright 2008, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * curve25519-donna: Curve25519 elliptic curve, public key function
 *
 * http://code.google.com/p/curve25519-donna/
 *
 * Adam Langley <agl@imperialviolet.org>
 *
 * Derived from public domain C code by Daniel J. Bernstein <djb@cr.yp.to>
 *
 * More information about curve25519 can be found here
 *   http://cr.yp.to/ecdh.html
 *
 * djb's sample implementation of curve25519 is written in a special assembly
 * language called qhasm and uses the floating point registers.
 *
 * This is, almost, a clean room reimplementation from the curve25519 paper. It
 * uses many of the tricks described therein. Only the crecip function is taken
 * from the sample implementation.
 *
 * Copyright(c) 2016 Petr Benes https://github.com/wbenny/mini-tor
 */



#include <iostream>
#include <cstdint>
#include <cstring>

 /* Field element representation:
  *
  * Field elements are written as an array of signed, 64-bit limbs, least
  * significant first. The value of the field element is:
  *   x[0] + 2^26*x[1] + x^51*x[2] + 2^102*x[3] + ...
  *
  * i.e. the limbs are 26, 25, 26, 25, ... bits wide. */

  /* Sum two numbers: output += in */
void Curve25519::fsum(limb* output, const limb* in) {
	unsigned i;
	for (i = 0; i < 10; i += 2) {
		output[0 + i] = output[0 + i] + in[0 + i];
		output[1 + i] = output[1 + i] + in[1 + i];
	}
}

/* Find the difference of two numbers: output = in - output
 * (note the order of the arguments!). */
void Curve25519::fdifference(limb* output, const limb* in) {
	unsigned i;
	for (i = 0; i < 10; ++i) {
		output[i] = in[i] - output[i];
	}
}

/* Multiply a number by a scalar: output = in * scalar */
void Curve25519::fscalar_product(limb* output, const limb* in, const limb scalar) {
	unsigned i;
	for (i = 0; i < 10; ++i) {
		output[i] = in[i] * scalar;
	}
}

/* Multiply two numbers: output = in2 * in
 *
 * output must be distinct to both inputs. The inputs are reduced coefficient
 * form, the output is not.
 *
 * output[x] <= 14 * the largest product of the input limbs. */
void Curve25519::fproduct(limb* output, const limb* in2, const limb* in) {
	output[0] = ((limb)((s32)in2[0])) * ((s32)in[0]);
	output[1] = ((limb)((s32)in2[0])) * ((s32)in[1]) +
		((limb)((s32)in2[1])) * ((s32)in[0]);
	output[2] = 2 * ((limb)((s32)in2[1])) * ((s32)in[1]) +
		((limb)((s32)in2[0])) * ((s32)in[2]) +
		((limb)((s32)in2[2])) * ((s32)in[0]);
	output[3] = ((limb)((s32)in2[1])) * ((s32)in[2]) +
		((limb)((s32)in2[2])) * ((s32)in[1]) +
		((limb)((s32)in2[0])) * ((s32)in[3]) +
		((limb)((s32)in2[3])) * ((s32)in[0]);
	output[4] = ((limb)((s32)in2[2])) * ((s32)in[2]) +
		2 * (((limb)((s32)in2[1])) * ((s32)in[3]) +
			((limb)((s32)in2[3])) * ((s32)in[1])) +
		((limb)((s32)in2[0])) * ((s32)in[4]) +
		((limb)((s32)in2[4])) * ((s32)in[0]);
	output[5] = ((limb)((s32)in2[2])) * ((s32)in[3]) +
		((limb)((s32)in2[3])) * ((s32)in[2]) +
		((limb)((s32)in2[1])) * ((s32)in[4]) +
		((limb)((s32)in2[4])) * ((s32)in[1]) +
		((limb)((s32)in2[0])) * ((s32)in[5]) +
		((limb)((s32)in2[5])) * ((s32)in[0]);
	output[6] = 2 * (((limb)((s32)in2[3])) * ((s32)in[3]) +
		((limb)((s32)in2[1])) * ((s32)in[5]) +
		((limb)((s32)in2[5])) * ((s32)in[1])) +
		((limb)((s32)in2[2])) * ((s32)in[4]) +
		((limb)((s32)in2[4])) * ((s32)in[2]) +
		((limb)((s32)in2[0])) * ((s32)in[6]) +
		((limb)((s32)in2[6])) * ((s32)in[0]);
	output[7] = ((limb)((s32)in2[3])) * ((s32)in[4]) +
		((limb)((s32)in2[4])) * ((s32)in[3]) +
		((limb)((s32)in2[2])) * ((s32)in[5]) +
		((limb)((s32)in2[5])) * ((s32)in[2]) +
		((limb)((s32)in2[1])) * ((s32)in[6]) +
		((limb)((s32)in2[6])) * ((s32)in[1]) +
		((limb)((s32)in2[0])) * ((s32)in[7]) +
		((limb)((s32)in2[7])) * ((s32)in[0]);
	output[8] = ((limb)((s32)in2[4])) * ((s32)in[4]) +
		2 * (((limb)((s32)in2[3])) * ((s32)in[5]) +
			((limb)((s32)in2[5])) * ((s32)in[3]) +
			((limb)((s32)in2[1])) * ((s32)in[7]) +
			((limb)((s32)in2[7])) * ((s32)in[1])) +
		((limb)((s32)in2[2])) * ((s32)in[6]) +
		((limb)((s32)in2[6])) * ((s32)in[2]) +
		((limb)((s32)in2[0])) * ((s32)in[8]) +
		((limb)((s32)in2[8])) * ((s32)in[0]);
	output[9] = ((limb)((s32)in2[4])) * ((s32)in[5]) +
		((limb)((s32)in2[5])) * ((s32)in[4]) +
		((limb)((s32)in2[3])) * ((s32)in[6]) +
		((limb)((s32)in2[6])) * ((s32)in[3]) +
		((limb)((s32)in2[2])) * ((s32)in[7]) +
		((limb)((s32)in2[7])) * ((s32)in[2]) +
		((limb)((s32)in2[1])) * ((s32)in[8]) +
		((limb)((s32)in2[8])) * ((s32)in[1]) +
		((limb)((s32)in2[0])) * ((s32)in[9]) +
		((limb)((s32)in2[9])) * ((s32)in[0]);
	output[10] = 2 * (((limb)((s32)in2[5])) * ((s32)in[5]) +
		((limb)((s32)in2[3])) * ((s32)in[7]) +
		((limb)((s32)in2[7])) * ((s32)in[3]) +
		((limb)((s32)in2[1])) * ((s32)in[9]) +
		((limb)((s32)in2[9])) * ((s32)in[1])) +
		((limb)((s32)in2[4])) * ((s32)in[6]) +
		((limb)((s32)in2[6])) * ((s32)in[4]) +
		((limb)((s32)in2[2])) * ((s32)in[8]) +
		((limb)((s32)in2[8])) * ((s32)in[2]);
	output[11] = ((limb)((s32)in2[5])) * ((s32)in[6]) +
		((limb)((s32)in2[6])) * ((s32)in[5]) +
		((limb)((s32)in2[4])) * ((s32)in[7]) +
		((limb)((s32)in2[7])) * ((s32)in[4]) +
		((limb)((s32)in2[3])) * ((s32)in[8]) +
		((limb)((s32)in2[8])) * ((s32)in[3]) +
		((limb)((s32)in2[2])) * ((s32)in[9]) +
		((limb)((s32)in2[9])) * ((s32)in[2]);
	output[12] = ((limb)((s32)in2[6])) * ((s32)in[6]) +
		2 * (((limb)((s32)in2[5])) * ((s32)in[7]) +
			((limb)((s32)in2[7])) * ((s32)in[5]) +
			((limb)((s32)in2[3])) * ((s32)in[9]) +
			((limb)((s32)in2[9])) * ((s32)in[3])) +
		((limb)((s32)in2[4])) * ((s32)in[8]) +
		((limb)((s32)in2[8])) * ((s32)in[4]);
	output[13] = ((limb)((s32)in2[6])) * ((s32)in[7]) +
		((limb)((s32)in2[7])) * ((s32)in[6]) +
		((limb)((s32)in2[5])) * ((s32)in[8]) +
		((limb)((s32)in2[8])) * ((s32)in[5]) +
		((limb)((s32)in2[4])) * ((s32)in[9]) +
		((limb)((s32)in2[9])) * ((s32)in[4]);
	output[14] = 2 * (((limb)((s32)in2[7])) * ((s32)in[7]) +
		((limb)((s32)in2[5])) * ((s32)in[9]) +
		((limb)((s32)in2[9])) * ((s32)in[5])) +
		((limb)((s32)in2[6])) * ((s32)in[8]) +
		((limb)((s32)in2[8])) * ((s32)in[6]);
	output[15] = ((limb)((s32)in2[7])) * ((s32)in[8]) +
		((limb)((s32)in2[8])) * ((s32)in[7]) +
		((limb)((s32)in2[6])) * ((s32)in[9]) +
		((limb)((s32)in2[9])) * ((s32)in[6]);
	output[16] = ((limb)((s32)in2[8])) * ((s32)in[8]) +
		2 * (((limb)((s32)in2[7])) * ((s32)in[9]) +
			((limb)((s32)in2[9])) * ((s32)in[7]));
	output[17] = ((limb)((s32)in2[8])) * ((s32)in[9]) +
		((limb)((s32)in2[9])) * ((s32)in[8]);
	output[18] = 2 * ((limb)((s32)in2[9])) * ((s32)in[9]);
}

/* Reduce a long form to a short form by taking the input mod 2^255 - 19.
 *
 * On entry: |output[i]| < 14*2^54
 * On exit: |output[0..8]| < 280*2^54 */
void Curve25519::freduce_degree(limb* output) {
	/* Each of these shifts and adds ends up multiplying the value by 19.
	 *
	 * For output[0..8], the absolute entry value is < 14*2^54 and we add, at
	 * most, 19*14*2^54 thus, on exit, |output[0..8]| < 280*2^54. */
	output[8] += output[18] << 4;
	output[8] += output[18] << 1;
	output[8] += output[18];
	output[7] += output[17] << 4;
	output[7] += output[17] << 1;
	output[7] += output[17];
	output[6] += output[16] << 4;
	output[6] += output[16] << 1;
	output[6] += output[16];
	output[5] += output[15] << 4;
	output[5] += output[15] << 1;
	output[5] += output[15];
	output[4] += output[14] << 4;
	output[4] += output[14] << 1;
	output[4] += output[14];
	output[3] += output[13] << 4;
	output[3] += output[13] << 1;
	output[3] += output[13];
	output[2] += output[12] << 4;
	output[2] += output[12] << 1;
	output[2] += output[12];
	output[1] += output[11] << 4;
	output[1] += output[11] << 1;
	output[1] += output[11];
	output[0] += output[10] << 4;
	output[0] += output[10] << 1;
	output[0] += output[10];
}

#if (-1 & 3) != 3
#error "This code only works on a two's complement system"
#endif

/* return v / 2^26, using only shifts and adds.
 *
 * On entry: v can take any value. */
inline limb Curve25519::div_by_2_26(const limb v)
{
	/* High word of v; no shift needed. */
	const uint32_t highword = (uint32_t)(((uint64_t)v) >> 32);
	/* Set to all 1s if v was negative; else set to 0s. */
	const int32_t sign = ((int32_t)highword) >> 31;
	/* Set to 0x3ffffff if v was negative; else set to 0. */
	const int32_t roundoff = ((uint32_t)sign) >> 6;
	/* Should return v / (1<<26) */
	return (v + roundoff) >> 26;
}

/* return v / (2^25), using only shifts and adds.
 *
 * On entry: v can take any value. */
inline limb Curve25519::div_by_2_25(const limb v)
{
	/* High word of v; no shift needed*/
	const uint32_t highword = (uint32_t)(((uint64_t)v) >> 32);
	/* Set to all 1s if v was negative; else set to 0s. */
	const int32_t sign = ((int32_t)highword) >> 31;
	/* Set to 0x1ffffff if v was negative; else set to 0. */
	const int32_t roundoff = ((uint32_t)sign) >> 7;
	/* Should return v / (1<<25) */
	return (v + roundoff) >> 25;
}

/* Reduce all coefficients of the short form input so that |x| < 2^26.
 *
 * On entry: |output[i]| < 280*2^54 */
void Curve25519::freduce_coefficients(limb* output) {
	unsigned i;

	output[10] = 0;

	for (i = 0; i < 10; i += 2) {
		limb over = div_by_2_26(output[i]);
		/* The entry condition (that |output[i]| < 280*2^54) means that over is, at
		 * most, 280*2^28 in the first iteration of this loop. This is added to the
		 * next limb and we can approximate the resulting bound of that limb by
		 * 281*2^54. */
		output[i] -= over << 26;
		output[i + 1] += over;

		/* For the first iteration, |output[i+1]| < 281*2^54, thus |over| <
		 * 281*2^29. When this is added to the next limb, the resulting bound can
		 * be approximated as 281*2^54.
		 *
		 * For subsequent iterations of the loop, 281*2^54 remains a conservative
		 * bound and no overflow occurs. */
		over = div_by_2_25(output[i + 1]);
		output[i + 1] -= over << 25;
		output[i + 2] += over;
	}
	/* Now |output[10]| < 281*2^29 and all other coefficients are reduced. */
	output[0] += output[10] << 4;
	output[0] += output[10] << 1;
	output[0] += output[10];

	output[10] = 0;

	/* Now output[1..9] are reduced, and |output[0]| < 2^26 + 19*281*2^29
	 * So |over| will be no more than 2^16. */
	{
		limb over = div_by_2_26(output[0]);
		output[0] -= over << 26;
		output[1] += over;
	}

	/* Now output[0,2..9] are reduced, and |output[1]| < 2^25 + 2^16 < 2^26. The
	 * bound on |output[1]| is sufficient to meet our needs. */
}

/* A helpful wrapper around fproduct: output = in * in2.
 *
 * On entry: |in[i]| < 2^27 and |in2[i]| < 2^27.
 *
 * output must be distinct to both inputs. The output is reduced degree
 * (indeed, one need only provide storage for 10 limbs) and |output[i]| < 2^26. */
void Curve25519::fmul(limb* output, const limb* in, const limb* in2) {
	limb t[19];
	fproduct(t, in, in2);
	/* |t[i]| < 14*2^54 */
	freduce_degree(t);
	freduce_coefficients(t);
	/* |t[i]| < 2^26 */
	memcpy(output, t, sizeof(limb) * 10);
}

/* Square a number: output = in**2
 *
 * output must be distinct from the input. The inputs are reduced coefficient
 * form, the output is not.
 *
 * output[x] <= 14 * the largest product of the input limbs. */
void Curve25519::fsquare_inner(limb* output, const limb* in) {
	output[0] = ((limb)((s32)in[0])) * ((s32)in[0]);
	output[1] = 2 * ((limb)((s32)in[0])) * ((s32)in[1]);
	output[2] = 2 * (((limb)((s32)in[1])) * ((s32)in[1]) +
		((limb)((s32)in[0])) * ((s32)in[2]));
	output[3] = 2 * (((limb)((s32)in[1])) * ((s32)in[2]) +
		((limb)((s32)in[0])) * ((s32)in[3]));
	output[4] = ((limb)((s32)in[2])) * ((s32)in[2]) +
		4 * ((limb)((s32)in[1])) * ((s32)in[3]) +
		2 * ((limb)((s32)in[0])) * ((s32)in[4]);
	output[5] = 2 * (((limb)((s32)in[2])) * ((s32)in[3]) +
		((limb)((s32)in[1])) * ((s32)in[4]) +
		((limb)((s32)in[0])) * ((s32)in[5]));
	output[6] = 2 * (((limb)((s32)in[3])) * ((s32)in[3]) +
		((limb)((s32)in[2])) * ((s32)in[4]) +
		((limb)((s32)in[0])) * ((s32)in[6]) +
		2 * ((limb)((s32)in[1])) * ((s32)in[5]));
	output[7] = 2 * (((limb)((s32)in[3])) * ((s32)in[4]) +
		((limb)((s32)in[2])) * ((s32)in[5]) +
		((limb)((s32)in[1])) * ((s32)in[6]) +
		((limb)((s32)in[0])) * ((s32)in[7]));
	output[8] = ((limb)((s32)in[4])) * ((s32)in[4]) +
		2 * (((limb)((s32)in[2])) * ((s32)in[6]) +
			((limb)((s32)in[0])) * ((s32)in[8]) +
			2 * (((limb)((s32)in[1])) * ((s32)in[7]) +
				((limb)((s32)in[3])) * ((s32)in[5])));
	output[9] = 2 * (((limb)((s32)in[4])) * ((s32)in[5]) +
		((limb)((s32)in[3])) * ((s32)in[6]) +
		((limb)((s32)in[2])) * ((s32)in[7]) +
		((limb)((s32)in[1])) * ((s32)in[8]) +
		((limb)((s32)in[0])) * ((s32)in[9]));
	output[10] = 2 * (((limb)((s32)in[5])) * ((s32)in[5]) +
		((limb)((s32)in[4])) * ((s32)in[6]) +
		((limb)((s32)in[2])) * ((s32)in[8]) +
		2 * (((limb)((s32)in[3])) * ((s32)in[7]) +
			((limb)((s32)in[1])) * ((s32)in[9])));
	output[11] = 2 * (((limb)((s32)in[5])) * ((s32)in[6]) +
		((limb)((s32)in[4])) * ((s32)in[7]) +
		((limb)((s32)in[3])) * ((s32)in[8]) +
		((limb)((s32)in[2])) * ((s32)in[9]));
	output[12] = ((limb)((s32)in[6])) * ((s32)in[6]) +
		2 * (((limb)((s32)in[4])) * ((s32)in[8]) +
			2 * (((limb)((s32)in[5])) * ((s32)in[7]) +
				((limb)((s32)in[3])) * ((s32)in[9])));
	output[13] = 2 * (((limb)((s32)in[6])) * ((s32)in[7]) +
		((limb)((s32)in[5])) * ((s32)in[8]) +
		((limb)((s32)in[4])) * ((s32)in[9]));
	output[14] = 2 * (((limb)((s32)in[7])) * ((s32)in[7]) +
		((limb)((s32)in[6])) * ((s32)in[8]) +
		2 * ((limb)((s32)in[5])) * ((s32)in[9]));
	output[15] = 2 * (((limb)((s32)in[7])) * ((s32)in[8]) +
		((limb)((s32)in[6])) * ((s32)in[9]));
	output[16] = ((limb)((s32)in[8])) * ((s32)in[8]) +
		4 * ((limb)((s32)in[7])) * ((s32)in[9]);
	output[17] = 2 * ((limb)((s32)in[8])) * ((s32)in[9]);
	output[18] = 2 * ((limb)((s32)in[9])) * ((s32)in[9]);
}

/* fsquare sets output = in^2.
 *
 * On entry: The |in| argument is in reduced coefficients form and |in[i]| <
 * 2^27.
 *
 * On exit: The |output| argument is in reduced coefficients form (indeed, one
 * need only provide storage for 10 limbs) and |out[i]| < 2^26. */
void Curve25519::fsquare(limb* output, const limb* in) {
	limb t[19];
	fsquare_inner(t, in);
	/* |t[i]| < 14*2^54 because the largest product of two limbs will be <
	 * 2^(27+27) and fsquare_inner adds together, at most, 14 of those
	 * products. */
	freduce_degree(t);
	freduce_coefficients(t);
	/* |t[i]| < 2^26 */
	memcpy(output, t, sizeof(limb) * 10);
}

/* Take a little-endian, 32-byte number and expand it into polynomial form */
void Curve25519::fexpand(limb* output, const u8* input) {
#define F(n,start,shift,mask) \
  output[n] = ((((limb) input[start + 0]) | \
                ((limb) input[start + 1]) << 8 | \
                ((limb) input[start + 2]) << 16 | \
                ((limb) input[start + 3]) << 24) >> shift) & mask;
	F(0, 0, 0, 0x3ffffff);
	F(1, 3, 2, 0x1ffffff);
	F(2, 6, 3, 0x3ffffff);
	F(3, 9, 5, 0x1ffffff);
	F(4, 12, 6, 0x3ffffff);
	F(5, 16, 0, 0x1ffffff);
	F(6, 19, 1, 0x3ffffff);
	F(7, 22, 3, 0x1ffffff);
	F(8, 25, 4, 0x3ffffff);
	F(9, 28, 6, 0x1ffffff);
#undef F
}

#if (-32 >> 1) != -16
#error "This code only works when >> does sign-extension on negative numbers"
#endif

/* s32_eq returns 0xffffffff iff a == b and zero otherwise. */
s32 Curve25519::s32_eq(s32 a, s32 b) {
	a = ~(a ^ b);
	a &= a << 16;
	a &= a << 8;
	a &= a << 4;
	a &= a << 2;
	a &= a << 1;
	return a >> 31;
}

/* s32_gte returns 0xffffffff if a >= b and zero otherwise, where a and b are
 * both non-negative. */
s32 Curve25519::s32_gte(s32 a, s32 b) {
	a -= b;
	/* a >= 0 iff a >= b. */
	return ~(a >> 31);
}

/* Take a fully reduced polynomial form number and contract it into a
 * little-endian, 32-byte array.
 *
 * On entry: |input_limbs[i]| < 2^26 */
void Curve25519::fcontract(u8* output, limb* input_limbs) {
	int i;
	int j;
	s32 input[10];
	s32 mask;

	/* |input_limbs[i]| < 2^26, so it's valid to convert to an s32. */
	for (i = 0; i < 10; i++) {
		input[i] = static_cast<s32>(input_limbs[i]);
	}

	for (j = 0; j < 2; ++j) {
		for (i = 0; i < 9; ++i) {
			if ((i & 1) == 1) {
				/* This calculation is a time-invariant way to make input[i]
				 * non-negative by borrowing from the next-larger limb. */
				const s32 mask = input[i] >> 31;
				const s32 carry = -((input[i] & mask) >> 25);
				input[i] = input[i] + (carry << 25);
				input[i + 1] = input[i + 1] - carry;
			}
			else {
				const s32 mask = input[i] >> 31;
				const s32 carry = -((input[i] & mask) >> 26);
				input[i] = input[i] + (carry << 26);
				input[i + 1] = input[i + 1] - carry;
			}
		}

		/* There's no greater limb for input[9] to borrow from, but we can multiply
		 * by 19 and borrow from input[0], which is valid mod 2^255-19. */
		{
			const s32 mask = input[9] >> 31;
			const s32 carry = -((input[9] & mask) >> 25);
			input[9] = input[9] + (carry << 25);
			input[0] = input[0] - (carry * 19);
		}

		/* After the first iteration, input[1..9] are non-negative and fit within
		 * 25 or 26 bits, depending on position. However, input[0] may be
		 * negative. */
	}

	/* The first borrow-propagation pass above ended with every limb
	   except (possibly) input[0] non-negative.

	   If input[0] was negative after the first pass, then it was because of a
	   carry from input[9]. On entry, input[9] < 2^26 so the carry was, at most,
	   one, since (2**26-1) >> 25 = 1. Thus input[0] >= -19.

	   In the second pass, each limb is decreased by at most one. Thus the second
	   borrow-propagation pass could only have wrapped around to decrease
	   input[0] again if the first pass left input[0] negative *and* input[1]
	   through input[9] were all zero.  In that case, input[1] is now 2^25 - 1,
	   and this last borrow-propagation step will leave input[1] non-negative. */
	{
		const s32 mask = input[0] >> 31;
		const s32 carry = -((input[0] & mask) >> 26);
		input[0] = input[0] + (carry << 26);
		input[1] = input[1] - carry;
	}

	/* All input[i] are now non-negative. However, there might be values between
	 * 2^25 and 2^26 in a limb which is, nominally, 25 bits wide. */
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 9; i++) {
			if ((i & 1) == 1) {
				const s32 carry = input[i] >> 25;
				input[i] &= 0x1ffffff;
				input[i + 1] += carry;
			}
			else {
				const s32 carry = input[i] >> 26;
				input[i] &= 0x3ffffff;
				input[i + 1] += carry;
			}
		}

		{
			const s32 carry = input[9] >> 25;
			input[9] &= 0x1ffffff;
			input[0] += 19 * carry;
		}
	}

	/* If the first carry-chain pass, just above, ended up with a carry from
	 * input[9], and that caused input[0] to be out-of-bounds, then input[0] was
	 * < 2^26 + 2*19, because the carry was, at most, two.
	 *
	 * If the second pass carried from input[9] again then input[0] is < 2*19 and
	 * the input[9] -> input[0] carry didn't push input[0] out of bounds. */

	 /* It still remains the case that input might be between 2^255-19 and 2^255.
	  * In this case, input[1..9] must take their maximum value and input[0] must
	  * be >= (2^255-19) & 0x3ffffff, which is 0x3ffffed. */
	mask = s32_gte(input[0], 0x3ffffed);
	for (i = 1; i < 10; i++) {
		if ((i & 1) == 1) {
			mask &= s32_eq(input[i], 0x1ffffff);
		}
		else {
			mask &= s32_eq(input[i], 0x3ffffff);
		}
	}

	/* mask is either 0xffffffff (if input >= 2^255-19) and zero otherwise. Thus
	 * this conditionally subtracts 2^255-19. */
	input[0] -= mask & 0x3ffffed;

	for (i = 1; i < 10; i++) {
		if ((i & 1) == 1) {
			input[i] -= mask & 0x1ffffff;
		}
		else {
			input[i] -= mask & 0x3ffffff;
		}
	}

	input[1] <<= 2;
	input[2] <<= 3;
	input[3] <<= 5;
	input[4] <<= 6;
	input[6] <<= 1;
	input[7] <<= 3;
	input[8] <<= 4;
	input[9] <<= 6;
#define F(i, s) \
  output[s+0] |=  input[i] & 0xff; \
  output[s+1]  = (input[i] >> 8) & 0xff; \
  output[s+2]  = (input[i] >> 16) & 0xff; \
  output[s+3]  = (input[i] >> 24) & 0xff;
	output[0] = 0;
	output[16] = 0;
	F(0, 0);
	F(1, 3);
	F(2, 6);
	F(3, 9);
	F(4, 12);
	F(5, 16);
	F(6, 19);
	F(7, 22);
	F(8, 25);
	F(9, 28);
#undef F
}

/* Input: Q, Q', Q-Q'
 * Output: 2Q, Q+Q'
 *
 *   x2 z3: long form
 *   x3 z3: long form
 *   x z: short form, destroyed
 *   xprime zprime: short form, destroyed
 *   qmqp: short form, preserved
 *
 * On entry and exit, the absolute value of the limbs of all inputs and outputs
 * are < 2^26. */
void Curve25519::fmonty(limb* x2, limb* z2,  /* output 2Q */
	limb* x3, limb* z3,  /* output Q + Q' */
	limb* x, limb* z,    /* input Q */
	limb* xprime, limb* zprime,  /* input Q' */
	const limb* qmqp /* input Q - Q' */) {
	limb origx[10], origxprime[10], zzz[19], xx[19], zz[19], xxprime[19],
		zzprime[19], zzzprime[19], xxxprime[19];

	memcpy(origx, x, 10 * sizeof(limb));
	fsum(x, z);
	/* |x[i]| < 2^27 */
	fdifference(z, origx);  /* does x - z */
	/* |z[i]| < 2^27 */

	memcpy(origxprime, xprime, sizeof(limb) * 10);
	fsum(xprime, zprime);
	/* |xprime[i]| < 2^27 */
	fdifference(zprime, origxprime);
	/* |zprime[i]| < 2^27 */
	fproduct(xxprime, xprime, z);
	/* |xxprime[i]| < 14*2^54: the largest product of two limbs will be <
	 * 2^(27+27) and fproduct adds together, at most, 14 of those products.
	 * (Approximating that to 2^58 doesn't work out.) */
	fproduct(zzprime, x, zprime);
	/* |zzprime[i]| < 14*2^54 */
	freduce_degree(xxprime);
	freduce_coefficients(xxprime);
	/* |xxprime[i]| < 2^26 */
	freduce_degree(zzprime);
	freduce_coefficients(zzprime);
	/* |zzprime[i]| < 2^26 */
	memcpy(origxprime, xxprime, sizeof(limb) * 10);
	fsum(xxprime, zzprime);
	/* |xxprime[i]| < 2^27 */
	fdifference(zzprime, origxprime);
	/* |zzprime[i]| < 2^27 */
	fsquare(xxxprime, xxprime);
	/* |xxxprime[i]| < 2^26 */
	fsquare(zzzprime, zzprime);
	/* |zzzprime[i]| < 2^26 */
	fproduct(zzprime, zzzprime, qmqp);
	/* |zzprime[i]| < 14*2^52 */
	freduce_degree(zzprime);
	freduce_coefficients(zzprime);
	/* |zzprime[i]| < 2^26 */
	memcpy(x3, xxxprime, sizeof(limb) * 10);
	memcpy(z3, zzprime, sizeof(limb) * 10);

	fsquare(xx, x);
	/* |xx[i]| < 2^26 */
	fsquare(zz, z);
	/* |zz[i]| < 2^26 */
	fproduct(x2, xx, zz);
	/* |x2[i]| < 14*2^52 */
	freduce_degree(x2);
	freduce_coefficients(x2);
	/* |x2[i]| < 2^26 */
	fdifference(zz, xx);  /* does zz = xx - zz */
	/* |zz[i]| < 2^27 */
	memset(zzz + 10, 0, sizeof(limb) * 9);
	fscalar_product(zzz, zz, 121665);
	/* |zzz[i]| < 2^(27+17) */
	/* No need to call freduce_degree here:
	   fscalar_product doesn't increase the degree of its input. */
	freduce_coefficients(zzz);
	/* |zzz[i]| < 2^26 */
	fsum(zzz, xx);
	/* |zzz[i]| < 2^27 */
	fproduct(z2, zz, zzz);
	/* |z2[i]| < 14*2^(26+27) */
	freduce_degree(z2);
	freduce_coefficients(z2);
	/* |z2|i| < 2^26 */
}

/* Conditionally swap two reduced-form limb arrays if 'iswap' is 1, but leave
 * them unchanged if 'iswap' is 0.  Runs in data-invariant time to avoid
 * side-channel attacks.
 *
 * NOTE that this function requires that 'iswap' be 1 or 0; other values give
 * wrong results.  Also, the two limb arrays must be in reduced-coefficient,
 * reduced-degree form: the values in a[10..19] or b[10..19] aren't swapped,
 * and all all values in a[0..9],b[0..9] must have magnitude less than
 * INT32_MAX. */
void Curve25519::swap_conditional(limb a[19], limb b[19], limb iswap) {
	unsigned i;
	const s32 swap = (s32)-iswap;

	for (i = 0; i < 10; ++i) {
		const s32 x = swap & (((s32)a[i]) ^ ((s32)b[i]));
		a[i] = ((s32)a[i]) ^ x;
		b[i] = ((s32)b[i]) ^ x;
	}
}

/* Calculates nQ where Q is the x-coordinate of a point on the curve
 *
 *   resultx/resultz: the x coordinate of the resulting curve point (short form)
 *   n: a little endian, 32-byte number
 *   q: a point of the curve (short form) */
void Curve25519::cmult(limb* resultx, limb* resultz, const u8* n, const limb* q) {
	limb a[19] = { 0 }, b[19] = { 1 }, c[19] = { 1 }, d[19] = { 0 };
	limb* nqpqx = a, * nqpqz = b, * nqx = c, * nqz = d, * t;
	limb e[19] = { 0 }, f[19] = { 1 }, g[19] = { 0 }, h[19] = { 1 };
	limb* nqpqx2 = e, * nqpqz2 = f, * nqx2 = g, * nqz2 = h;

	unsigned i, j;

	memcpy(nqpqx, q, sizeof(limb) * 10);

	for (i = 0; i < 32; ++i) {
		u8 byte = n[31 - i];
		for (j = 0; j < 8; ++j) {
			const limb bit = byte >> 7;

			swap_conditional(nqx, nqpqx, bit);
			swap_conditional(nqz, nqpqz, bit);
			fmonty(nqx2, nqz2,
				nqpqx2, nqpqz2,
				nqx, nqz,
				nqpqx, nqpqz,
				q);
			swap_conditional(nqx2, nqpqx2, bit);
			swap_conditional(nqz2, nqpqz2, bit);

			t = nqx;
			nqx = nqx2;
			nqx2 = t;
			t = nqz;
			nqz = nqz2;
			nqz2 = t;
			t = nqpqx;
			nqpqx = nqpqx2;
			nqpqx2 = t;
			t = nqpqz;
			nqpqz = nqpqz2;
			nqpqz2 = t;

			byte <<= 1;
		}
	}

	memcpy(resultx, nqx, sizeof(limb) * 10);
	memcpy(resultz, nqz, sizeof(limb) * 10);
}

/* -----------------------------------------------------------------------------
 * Shamelessly copied from djb's code
 * ----------------------------------------------------------------------------- */
void Curve25519::crecip(limb* out, const limb* z) {
	limb z2[10];
	limb z9[10];
	limb z11[10];
	limb z2_5_0[10];
	limb z2_10_0[10];
	limb z2_20_0[10];
	limb z2_50_0[10];
	limb z2_100_0[10];
	limb t0[10];
	limb t1[10];
	int i;

	/* 2 */ fsquare(z2, z);
	/* 4 */ fsquare(t1, z2);
	/* 8 */ fsquare(t0, t1);
	/* 9 */ fmul(z9, t0, z);
	/* 11 */ fmul(z11, z9, z2);
	/* 22 */ fsquare(t0, z11);
	/* 2^5 - 2^0 = 31 */ fmul(z2_5_0, t0, z9);

	/* 2^6 - 2^1 */ fsquare(t0, z2_5_0);
	/* 2^7 - 2^2 */ fsquare(t1, t0);
	/* 2^8 - 2^3 */ fsquare(t0, t1);
	/* 2^9 - 2^4 */ fsquare(t1, t0);
	/* 2^10 - 2^5 */ fsquare(t0, t1);
	/* 2^10 - 2^0 */ fmul(z2_10_0, t0, z2_5_0);

	/* 2^11 - 2^1 */ fsquare(t0, z2_10_0);
	/* 2^12 - 2^2 */ fsquare(t1, t0);
	/* 2^20 - 2^10 */ for (i = 2; i < 10; i += 2) { fsquare(t0, t1); fsquare(t1, t0); }
	/* 2^20 - 2^0 */ fmul(z2_20_0, t1, z2_10_0);

	/* 2^21 - 2^1 */ fsquare(t0, z2_20_0);
	/* 2^22 - 2^2 */ fsquare(t1, t0);
	/* 2^40 - 2^20 */ for (i = 2; i < 20; i += 2) { fsquare(t0, t1); fsquare(t1, t0); }
	/* 2^40 - 2^0 */ fmul(t0, t1, z2_20_0);

	/* 2^41 - 2^1 */ fsquare(t1, t0);
	/* 2^42 - 2^2 */ fsquare(t0, t1);
	/* 2^50 - 2^10 */ for (i = 2; i < 10; i += 2) { fsquare(t1, t0); fsquare(t0, t1); }
	/* 2^50 - 2^0 */ fmul(z2_50_0, t0, z2_10_0);

	/* 2^51 - 2^1 */ fsquare(t0, z2_50_0);
	/* 2^52 - 2^2 */ fsquare(t1, t0);
	/* 2^100 - 2^50 */ for (i = 2; i < 50; i += 2) { fsquare(t0, t1); fsquare(t1, t0); }
	/* 2^100 - 2^0 */ fmul(z2_100_0, t1, z2_50_0);

	/* 2^101 - 2^1 */ fsquare(t1, z2_100_0);
	/* 2^102 - 2^2 */ fsquare(t0, t1);
	/* 2^200 - 2^100 */ for (i = 2; i < 100; i += 2) { fsquare(t1, t0); fsquare(t0, t1); }
	/* 2^200 - 2^0 */ fmul(t1, t0, z2_100_0);

	/* 2^201 - 2^1 */ fsquare(t0, t1);
	/* 2^202 - 2^2 */ fsquare(t1, t0);
	/* 2^250 - 2^50 */ for (i = 2; i < 50; i += 2) { fsquare(t0, t1); fsquare(t1, t0); }
	/* 2^250 - 2^0 */ fmul(t0, t1, z2_50_0);

	/* 2^251 - 2^1 */ fsquare(t1, t0);
	/* 2^252 - 2^2 */ fsquare(t0, t1);
	/* 2^253 - 2^3 */ fsquare(t1, t0);
	/* 2^254 - 2^4 */ fsquare(t0, t1);
	/* 2^255 - 2^5 */ fsquare(t1, t0);
	/* 2^255 - 21 */ fmul(out, t1, z11);
}
int Curve25519::curve25519_donna(u8* mypublic, const u8* secret, const u8* basepoint) {
	limb bp[10], x[10], z[11], zmone[10];
	uint8_t e[32];
	int i;

	for (i = 0; i < 32; ++i) e[i] = secret[i];
	e[0] &= 248;
	e[31] &= 127;
	e[31] |= 64;

	fexpand(bp, basepoint);
	cmult(x, z, e, bp);
	crecip(zmone, z);
	fmul(z, x, zmone);
	fcontract(mypublic, z);
	return 0;
}
/* END FILE: /home/josh/tor-connect/torlib/Curve25519.cpp */
/* BEGIN FILE: /home/josh/tor-connect/torlib/HTTPClient.cpp */
/*-
* Copyright (c) 2021, Zano project, https://zano.org/
* Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 4. Neither the name of this program nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/




// Start the asynchronous operation
void HTTPClient::RunClient(const string host, const int port, const string target, const int timeout, int version)
{
    error_operations = false;
    timeout_op = timeout;
    // Set up an HTTP GET request message
    req_.version(version);
    req_.method(http::verb::get);
    req_.target(target);
    req_.set(http::field::host, host);
    req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Look up the domain name
    resolver_.async_resolve(host, std::to_string(port),
        beast::bind_front_handler(&HTTPClient::OnResolve, shared_from_this()));
}

void HTTPClient::OnResolve(beast::error_code ec, tcp::resolver::results_type results)
{
    if (ec) 
      return OnFail(ec, "HTTPClient::OnResolve");
    
    // Set a timeout on the operation
    if (timeout_op > 0) 
      stream_.expires_after(std::chrono::seconds(timeout_op));
    
    // Make the connection on the IP address we get from a lookup
    stream_.async_connect(results, 
        beast::bind_front_handler(&HTTPClient::OnConnect, shared_from_this()));
}

void HTTPClient::OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
{
    if (ec) 
      return OnFail(ec, "HTTPClient::OnConnect");

    // Set a timeout on the operation
    if(timeout_op > 0) 
      stream_.expires_after(std::chrono::seconds(timeout_op));

    http::async_write(stream_, req_,
        beast::bind_front_handler(&HTTPClient::OnWrite, shared_from_this()));
}

void HTTPClient::OnWrite(beast::error_code ec, std::size_t /*bytes_transferred*/)
{
    if (ec) 
      return OnFail(ec, "HTTPClient::OnWrite");

    http::async_read(stream_, buffer_, res_,
        beast::bind_front_handler(&HTTPClient::OnRead, shared_from_this()));
}

void HTTPClient::OnRead(beast::error_code ec, std::size_t /*bytes_transferred*/)
{

    if (ec) 
      return OnFail(ec, "HTTPClient::OnRead");
    
    stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
    
    if (ec && ec != beast::errc::not_connected)
        return OnFail(ec, "HTTPClient shutdown");
}


void HTTPClient::OnFail(beast::error_code ec, char const* what)
{
    BOOST_LOG_TRIVIAL(error) << what << ": " << ec.message(); 
    res_.clear();
    error_operations = true;
}

string HTTPClient::GetData()
{
    string ret_str = "";
    if (!error_operations)
        return std::move(res_.body().data());
    return ret_str;
}
/* END FILE: /home/josh/tor-connect/torlib/HTTPClient.cpp */
/* BEGIN FILE: /home/josh/tor-connect/torlib/NetConnect.cpp */
/*-
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



NetConnect::NetConnect(net::io_service& io_service, ssl::context& context)
	: _resolver(io_service), socket(io_service, context)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::created";
};


void NetConnect::Connect(const string& host, int port, ConnectFunction connectFunc) {
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::Connect host="<< host<< " port="<< port;
	tcp::endpoint ep(net::ip::address::from_string(host.c_str()), port);
	_resolver.async_resolve(ep,
		boost::bind(&NetConnect::ResolvedComplete, this, connectFunc, pl::iterator, pl::error));
}

void NetConnect::ResolvedComplete(ConnectFunction connectFunc, 
	tcp::resolver::iterator endpoint_iter, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ResolvedComplete";		
	TEST_ERR(err, connectFunc);
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ResolvedComplete  Resolve OK";
	socket.set_verify_mode(ssl::verify_none);
	//socket.set_verify_mode(ssl::verify_peer);
	//socket.set_verify_callback(
	//	boost::bind(&NetConnect::VerifyCertificate, this, _1, _2));	
	net::async_connect(socket.lowest_layer(), endpoint_iter,
		boost::bind(&NetConnect::ConnectComplete, this, connectFunc, pl::error));
}

void NetConnect::ConnectComplete(ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ConnectComplete";
	TEST_ERR(err, connectFunc);
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ConnectComplete Connect OK ";
	socket.async_handshake(ssl::stream_base::client,
		boost::bind(&NetConnect::Handshake, this, connectFunc, pl::error));
}

bool NetConnect::VerifyCertificate(bool preverified, ssl::verify_context& ctx)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::VerifyCertificate";
	char subject_name[256];
	X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::VerifyCertificate Verifying " << subject_name;
	//return preverified;
	return true;
}

void NetConnect::Handshake(ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::Handshake";
	TEST_ERR(err, connectFunc);
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::Handshake  Handshake OK ";
	net::post(net::detail::bind_handler(connectFunc, err));
}

void NetConnect::SendVersion(ConnectFunction connectFunc)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::SendVersion";
	unsigned char versionBytes[] = { 0x00, 0x00, 0x07, 0x00, 0x02, 0x00, 0x04 };
	net::async_write(socket, net::buffer(versionBytes, 7),
		boost::bind(&NetConnect::SentVersionComplete, this, connectFunc, pl::error));
}

void NetConnect::SentVersionComplete(ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::SentVersionComplete";
	TEST_ERR(err, connectFunc);	
	net::async_read(socket, response,
		net::transfer_exactly(5),
		boost::bind(&NetConnect::ReadVersionComplete, this, connectFunc, pl::error));
}

tuple<u32, u8, u16> NetConnect::ParsingHeader()
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ParsingHeader";
	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string v_header(std::istreambuf_iterator<char>(response_stream), eos);
	unc* header_data = reinterpret_cast<unc*>(const_cast<char*>(v_header.c_str()));
	Util::HexDump(header_data, v_header.length());

	u32 id = Util::BigEndianArrayToInt(header_data);
	u8 command = static_cast<u8>(header_data[POSITION_COMMAND]);
	BOOST_LOG_TRIVIAL(debug) << "ParsingHeader::cell_command=" << static_cast<unsigned int>(header_data[POSITION_COMMAND]);
	u16 len = Util::BigEndianArrayToShort(header_data + POSITION_PAYLOAD_SIZE);
	BOOST_LOG_TRIVIAL(debug) << "ParsingHeader::len=" << len;
	return make_tuple(id, command, len);
}

void NetConnect::ReadVersionComplete(ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadVersionComplete";
	TEST_ERR(err, connectFunc);	

	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string v_header(std::istreambuf_iterator<char>(response_stream), eos);
	unc* header_data = reinterpret_cast<unc*>(const_cast<char*>(v_header.c_str()));
	Util::HexDump(header_data, v_header.length());

	BOOST_LOG_TRIVIAL(debug) << "ReadVersionComplete::cell_command=" << static_cast<unsigned int>(header_data[2]);
	if (header_data[2] != cell_command::versions) {
		BOOST_LOG_TRIVIAL(error) << "Received strange version response cell.";		
		net::post(net::detail::bind_handler(connectFunc, net::error::bad_descriptor));
		return;
	}	
	u16 len = Util::BigEndianArrayToShort(header_data + 3);
	BOOST_LOG_TRIVIAL(debug) << "ReadVersionComplete::len=" << len;
	if (len > 24) {
		BOOST_LOG_TRIVIAL(error) << "Version response length is strangely long.";
		net::post(net::detail::bind_handler(connectFunc, net::error::bad_descriptor));
		return;
	}
	net::async_read(socket, response,
		net::transfer_exactly(len),
		//net::transfer_at_least(len), 
		boost::bind(&NetConnect::ReadVersionContentComplete, this, len, connectFunc, pl::error));
}

void NetConnect::ReadVersionContentComplete(int len, ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadVersionContentComplete";
	TEST_ERR(err, connectFunc);

	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string v_content(std::istreambuf_iterator<char>(response_stream), eos);
	unc* content_data = reinterpret_cast<unc*>(const_cast<char*>(v_content.c_str()));
	Util::HexDump(content_data, len);

	BOOST_LOG_TRIVIAL(debug) << "Read Certificates 1";
	net::async_read(socket, response,
		net::transfer_exactly(POSITION_PAYLOAD),
		boost::bind(&NetConnect::ReadHederComplete1, this, connectFunc, pl::error));
}

void NetConnect::ReadHederComplete1(ConnectFunction connectFunc, const sys::error_code& err)
{	
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadHederComplete";
	TEST_ERR(err, connectFunc);
	tuple<u32, u8, u16> header = ParsingHeader();
	net::async_read(socket, response,
		net::transfer_exactly(get<2>(header)),
		boost::bind(&NetConnect::ReadCertificatesComplete1,this, get<2>(header), connectFunc, pl::error));
}

void NetConnect::ReadCertificatesComplete1(int len, ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadCertificatesComplete1";
	TEST_ERR(err, connectFunc);
	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string v_header(std::istreambuf_iterator<char>(response_stream), eos);
	unc* header_data = reinterpret_cast<unc*>(const_cast<char*>(v_header.c_str()));
	Util::HexDump(header_data, len);

	BOOST_LOG_TRIVIAL(debug) << "Read Certificates 2";
	net::async_read(socket, response,
		net::transfer_exactly(POSITION_PAYLOAD),
		boost::bind(&NetConnect::ReadHederComplete2, this, connectFunc, pl::error));
}

void NetConnect::ReadHederComplete2(ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadHederComplete2";
	TEST_ERR(err, connectFunc);
	tuple<u32, u8, u16> header = ParsingHeader();
	net::async_read(socket, response,
		net::transfer_exactly(get<2>(header)),
		boost::bind(&NetConnect::ReadCertificatesComplete2, this, get<2>(header), connectFunc, pl::error));
}
void NetConnect::ReadCertificatesComplete2(int len, ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadCertificatesComplete2";
	TEST_ERR(err, connectFunc);
	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string v_header(std::istreambuf_iterator<char>(response_stream), eos);
	unc* header_data = reinterpret_cast<unc*>(const_cast<char*>(v_header.c_str()));
	Util::HexDump(header_data, len);

	BOOST_LOG_TRIVIAL(debug) << "Read Net Info";
	net::async_read(socket, response,
		net::transfer_exactly(POSITION_PAYLOAD),
		boost::bind(&NetConnect::ReadHederNetInfoComplete, this, connectFunc, pl::error));
}
void NetConnect::ReadHederNetInfoComplete(ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadHederNetInfoComplete";
	TEST_ERR(err, connectFunc);
	tuple<u32, u8, u16> header = ParsingHeader();
	net::async_read(socket, response,
		net::transfer_at_least(1),		
		boost::bind(&NetConnect::ReadNetInfoComplete, this, get<2>(header), connectFunc, pl::error));
}
void NetConnect::ReadNetInfoComplete(int len, ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadNetInfoComplet";
	TEST_ERR(err, connectFunc);
	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string v_header(std::istreambuf_iterator<char>(response_stream), eos);
	unc* header_data = reinterpret_cast<unc*>(const_cast<char*>(v_header.c_str()));
	Util::HexDump(header_data, v_header.length());
	net::post(net::detail::bind_handler(connectFunc, err));	
}

void NetConnect::Close() {	
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::Close";	
}

tcp::endpoint NetConnect::GetLocalEndpoint() {	
	return socket.lowest_layer().local_endpoint();
}
long NetConnect::GetEndpointLong() {
	return socket.lowest_layer().remote_endpoint().address().to_v4().to_ulong();
}

NetConnect::~NetConnect() {
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::~NetConnect";
	Close();
}

void NetConnect::DummyFunc(const sys::error_code& error) {
	BOOST_LOG_TRIVIAL(debug) << "DummyFunc";
	if (error)
		BOOST_LOG_TRIVIAL(error) << error.message();
	else
		BOOST_LOG_TRIVIAL(debug) << "No error";
}

void NetConnect::WriteCell(Cell& cell, ConnectFunction connectFunc) {
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::WriteCell";
	unc* buffer = cell.GetBuffer();
	size_t len = cell.GetBufferSize();
	Util::HexDump(buffer, len);
	net::async_write(socket, net::buffer(buffer, len),
		boost::bind(&NetConnect::WriteCellComplete, this, connectFunc, pl::error));
}

void NetConnect::WriteCellComplete(ConnectFunction connectFunc, const sys::error_code& err) {
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::WriteCellComplete";
	TEST_ERR(err, connectFunc);
	net::post(net::detail::bind_handler(connectFunc, err));
}

void NetConnect::ReadCell(shared_ptr<Cell> cell, ConnectFunction connectFunc) {
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadCell";
	// Read heder
	net::async_read(socket, response, 
		net::transfer_exactly(POSITION_PAYLOAD),
		boost::bind(&NetConnect::ReadHeaderComplete, this, connectFunc, cell, pl::error));
}

void NetConnect::ReadHeaderComplete(ConnectFunction connectFunc, shared_ptr<Cell> cell, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadCellComplete";
	TEST_ERR(err, connectFunc);
	tuple<u32, u8, u16> header = ParsingHeader();		
	cell->SetId(get<0>(header));
	cell->SetCommand(get<1>(header));
	cell->SetPayloadSize(get<2>(header));
	net::async_read(socket, response,
		net::transfer_exactly(CELL_SIZE- POSITION_PAYLOAD),
		boost::bind(&NetConnect::ReadCellComplete, this, connectFunc, cell, pl::error));
}
void NetConnect::ReadCellComplete(ConnectFunction connectFunc, shared_ptr<Cell> cell, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadCellComplete";
	TEST_ERR(err, connectFunc);	
	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string data_str(std::istreambuf_iterator<char>(response_stream), eos);	
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadCellComplete len=" << data_str.length();
	cell->Append(data_str);	
	BOOST_LOG_TRIVIAL(debug) << "----------------------CELL Received-------------------";
	Util::HexDump(cell->GetBuffer(), cell->GetBufferSize());
	BOOST_LOG_TRIVIAL(debug) << "------------------------------------------------------";	
	
	net::post(net::detail::bind_handler(connectFunc, err));	
}
void NetConnect::HexDumpResponse()
{
	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string data_str(std::istreambuf_iterator<char>(response_stream), eos);
	unc* res_data = reinterpret_cast<unc*>(const_cast<char*>(data_str.c_str()));
	Util::HexDump(res_data, data_str.length());
}
void NetConnect::ShutDown()
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ShutDown";
	socket.shutdown(); 
}
/* END FILE: /home/josh/tor-connect/torlib/NetConnect.cpp */
/* BEGIN FILE: /home/josh/tor-connect/torlib/OnionRouter.cpp */
/*-
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


OnionRouter::OnionRouter() {    
    memset(forward_iv, 0, sizeof(forward_iv));
    memset(forward_ec, 0, sizeof(forward_ec));
    memset(back_iv, 0, sizeof(back_iv));
    memset(back_ec, 0, sizeof(back_ec));
    forward_num = 0;
    back_num = 0;
    SHA1_Init(&forward_digest);
    SHA1_Init(&backward_digest);
}

string OnionRouter::GetBase16EncodedIdentity(string identity_key) {    
    size_t encoded_len = identity_key.length() * 2 + 1;
    unique_ptr<char[]> encoded = std::unique_ptr<char[]>(new char[encoded_len]);
    Util::Base16Encode(encoded.get(), encoded_len, identity_key.c_str(), identity_key.length());
    string ret(encoded.get());
    return ret;
}

RSA* OnionRouter::GetKey(string str_key) {
    if (str_key.length() == 0) return NULL;
    RSA* key = RSA_new();
    BIO* publicKeyBio = BIO_new_mem_buf(str_key.c_str(), static_cast<int>(str_key.length()));
    if (publicKeyBio == NULL) return NULL;
    key = PEM_read_bio_RSAPublicKey(publicKeyBio, &key, NULL, NULL);
    if (key == NULL) return NULL;
    return key;
}

bool OnionRouter::SetOnionKey(string current_key) {    
    onion_key= Util::Base64Decode(current_key);
    if (onion_key.length()==0) return false;
    return true;
}
bool OnionRouter::SetSignigKey(string current_key) {
    signing_key = Util::Base64Decode(current_key);
    if (signing_key.length() == 0) return false;
    return true;
}
bool OnionRouter::SetNtorOnionKey(string current_key) {
    ntor_onion_key = Util::Base64Decode(current_key);
    if (ntor_onion_key.length() == 0) return false;
    return true;
}
bool OnionRouter::SetServiceKey(string current_key) {
    service_key = Util::Base64Decode(current_key);
    if (service_key.length() == 0) return false;
    return true;
}
void OnionRouter::GeneratPairKeys()
{
    vector<uint8_t> onion_skin_ntor;
    for (int i = 0; i < 32; i++)
    {
        pair_key.second.push_back(Util::GetRandom8());
        pair_key.first.push_back(0);
    }
    pair_key.second[0] &= 248;
    pair_key.second[31] &= 127;
    pair_key.second[31] |= 64;
    Curve25519::curve25519_donna(pair_key.first.data(), pair_key.second.data(), basepoint_9);
    BOOST_LOG_TRIVIAL(debug) << "------------- public key --------------";
    Util::HexDump(pair_key.first.data(), pair_key.first.size());
    BOOST_LOG_TRIVIAL(debug) << "---------------------------------------";
    BOOST_LOG_TRIVIAL(debug) << "------------- private key -------------";
    Util::HexDump(pair_key.second.data(), pair_key.second.size());
    BOOST_LOG_TRIVIAL(debug) << "---------------------------------------";
}

u8* OnionRouter::GetPublicKey()
{    
    return pair_key.first.data();
}

size_t OnionRouter::GetPublicKeySize()
{
    return pair_key.first.size();
}

bool OnionRouter::GeneratKeyMaterialUnc(unc* handshake_data, unc* other_public_key, unc* verify)
{
    BOOST_LOG_TRIVIAL(debug) << "-------------other_public_key-------------";
    Util::HexDump(other_public_key, KEY_SIZE);
    BOOST_LOG_TRIVIAL(debug) << "------------------------------------------";
    BOOST_LOG_TRIVIAL(debug) << "-------------verify-----------------------";
    Util::HexDump(verify, KEY_SIZE);
    BOOST_LOG_TRIVIAL(debug) << "------------------------------------------";
    BOOST_LOG_TRIVIAL(debug) << "-------------ntor_onion_key---------------";
    const unc* ntor = reinterpret_cast<const unc*>(ntor_onion_key.c_str());
    Util::HexDump(const_cast<unc*>(ntor), KEY_SIZE);
    BOOST_LOG_TRIVIAL(debug) << "------------------------------------------";


    unc shared_key1[KEY_SIZE];
    Curve25519::curve25519_donna(&shared_key1[0], pair_key.second.data(), other_public_key);
    BOOST_LOG_TRIVIAL(debug) << "-------------shared_key1-------------";
    Util::HexDump(shared_key1, KEY_SIZE);
    BOOST_LOG_TRIVIAL(debug) << "-------------------------------------";

    unc shared_key2[KEY_SIZE];
    Curve25519::curve25519_donna(&shared_key2[0], pair_key.second.data(),
        reinterpret_cast<const unc*>(ntor_onion_key.c_str()));

    BOOST_LOG_TRIVIAL(debug) << "-------------shared_key2-------------";
    Util::HexDump(shared_key2, KEY_SIZE);
    BOOST_LOG_TRIVIAL(debug) << "-------------------------------------";

    vector<unc> secret_input;
    copy(shared_key1, shared_key1 + KEY_SIZE, back_inserter(secret_input));
    copy(shared_key2, shared_key2 + KEY_SIZE, back_inserter(secret_input));
    copy(identity.begin(), identity.end(), back_inserter(secret_input));
    copy(ntor_onion_key.begin(), ntor_onion_key.end(), back_inserter(secret_input));
    copy(pair_key.first.begin(), pair_key.first.end(), back_inserter(secret_input));
    copy(other_public_key, other_public_key + KEY_SIZE, back_inserter(secret_input));
    copy(const_protoid, const_protoid + sizeof(const_protoid), back_inserter(secret_input));    

    BOOST_LOG_TRIVIAL(debug) << "-------------secret_input-------------";
    Util::HexDump(secret_input.data(), secret_input.size());
    BOOST_LOG_TRIVIAL(debug) << "--------------------------------------";

    unc* result = nullptr;
    uni resultlen = -1;
    unc* verify_comp = HMAC(EVP_sha256(), const_t_verify, sizeof(const_t_verify), secret_input.data(), secret_input.size(), result, &resultlen);

    vector<unc> auth_input;
    copy(verify_comp, verify_comp + KEY_SIZE, back_inserter(auth_input));
    copy(identity.begin(), identity.end(), back_inserter(auth_input));
    copy(ntor_onion_key.begin(), ntor_onion_key.end(), back_inserter(auth_input));
    copy(other_public_key, other_public_key + KEY_SIZE, back_inserter(auth_input));
    copy(pair_key.first.begin(), pair_key.first.end(), back_inserter(auth_input));
    copy(const_protoid, const_protoid + sizeof(const_protoid), back_inserter(auth_input));
    copy(const_server, const_server + sizeof(const_server), back_inserter(auth_input));
    result = nullptr;
    resultlen = -1;
    unc* verify_data = HMAC(EVP_sha256(), const_t_mac, sizeof(const_t_mac), auth_input.data(), auth_input.size(), result, &resultlen);

    if (memcmp(verify, verify_data, resultlen) == 0)
    {
        // create key material   
        result = nullptr;
        resultlen = -1;
        unc* ps_key = HMAC(EVP_sha256(), const_t_key, sizeof(const_t_key), secret_input.data(), secret_input.size(), result, &resultlen);
        vector<u8> pseudo_random_key;
        copy(ps_key, ps_key + resultlen, back_inserter(pseudo_random_key));

        BOOST_LOG_TRIVIAL(debug) << "-------------secret_input-------------";
        Util::HexDump(ps_key, resultlen);
        BOOST_LOG_TRIVIAL(debug) << "---------------------------------------";

        unc* result_block = nullptr;
        size_t bytes_remaining = TOTAL_KEY_MATERIAL;
        size_t bytes_processed=0;

        for (uint8_t i = 1; bytes_remaining > 0; i++)
        {
            vector<u8> message;
            if(result_block!=nullptr)
                copy(result_block, result_block + bytes_processed, back_inserter(message));
            copy(const_m_expand, const_m_expand+ sizeof(const_m_expand), back_inserter(message));
            message.push_back(i);
            result = nullptr;
            resultlen = -1;
            result_block = HMAC(EVP_sha256(), pseudo_random_key.data(), static_cast<int>(pseudo_random_key.size()), message.data(), message.size(), result, &resultlen);
            bytes_processed = min(static_cast<size_t>(resultlen), bytes_remaining);
            copy(result_block, result_block + bytes_processed, back_inserter(key_material));
            bytes_remaining -= bytes_processed;
        }
        BOOST_LOG_TRIVIAL(debug) << "-------------key_material-------------";
        Util::HexDump(key_material.data(), key_material.size());
        BOOST_LOG_TRIVIAL(debug) << "--------------------------------------";
    }
    else
    {
        BOOST_LOG_TRIVIAL(warning) << "Failed to generate key!";
        return false;
    }
    BOOST_LOG_TRIVIAL(debug) << "Material Key Generated!";

    return true;
}

bool OnionRouter::GeneratKeyMaterial(RelayCell &node)
{
    unc* handshake_data = node.GetPayloadPart(HANDSHAKE_DATA , CELL_SIZE - HANDSHAKE_DATA);
    unc* other_public_key = node.GetPayloadPart(HANDSHAKE_DATA, HANDSHAKE_DATA + KEY_SIZE);
    unc* verify = node.GetPayloadPart(HANDSHAKE_DATA + KEY_SIZE, HANDSHAKE_DATA + (KEY_SIZE * 2));

    BOOST_LOG_TRIVIAL(debug) << "-------------handshake_data-------------";
    Util::HexDump(handshake_data, CELL_SIZE - HANDSHAKE_DATA);
    BOOST_LOG_TRIVIAL(debug) << "----------------------------------------";

    bool ret= GeneratKeyMaterialUnc(handshake_data, other_public_key, verify);
    free(handshake_data);
    free(other_public_key);
    free(verify);
    return ret;
}

bool OnionRouter::GeneratKeyMaterial(shared_ptr<Cell> node)
{
    unc* handshake_data = node->GetPayloadPart(POSITION_PAYLOAD, CELL_SIZE - POSITION_PAYLOAD);
    unc* other_public_key = node->GetPayloadPart(POSITION_PAYLOAD, POSITION_PAYLOAD+KEY_SIZE);
    unc* verify = node->GetPayloadPart(POSITION_PAYLOAD+KEY_SIZE, POSITION_PAYLOAD + (KEY_SIZE * 2));
    bool ret = GeneratKeyMaterialUnc(handshake_data, other_public_key, verify);
    free(handshake_data);
    free(other_public_key);
    free(verify);
    return ret;
}
void OnionRouter::SetKeyMaterial(unc* key_material)
{
    BOOST_LOG_TRIVIAL(debug) << "-------------SetKeyMaterial-------------";
    Util::HexDump(key_material, TOTAL_KEY_MATERIAL);
    BOOST_LOG_TRIVIAL(debug) << "----------------------------------------";

    SHA1_Update(&forward_digest, key_material, DIGEST_LEN);
    SHA1_Update(&backward_digest, key_material + DIGEST_LEN, DIGEST_LEN);

    AES_set_encrypt_key(key_material + (DIGEST_LEN * 2), KEY_LEN * 8, &forward_key);
    AES_set_encrypt_key(key_material + (DIGEST_LEN * 2) + KEY_LEN, KEY_LEN * 8, &backward_key);

    BOOST_LOG_TRIVIAL(debug) << "-------------forward_key-------------";
    Util::HexDump(key_material + (DIGEST_LEN * 2), KEY_LEN);
    BOOST_LOG_TRIVIAL(debug) << "-------------------------------------";
}
void OnionRouter::AesEncrypt(Cell& cell, AES_KEY* key, unc* iv, unc* ec, uni* num)
{
    unsigned char buf[CELL_SIZE];
    unc* cell_payload = cell.GetPayload();
    size_t cell_payload_size = cell.GetPayloadSize();

    BOOST_LOG_TRIVIAL(debug) << "-------------Befor AesEncrypt-------------";
    Util::HexDump(cell_payload, cell_payload_size);
    BOOST_LOG_TRIVIAL(debug) << "------------------------------------------";

    CRYPTO_ctr128_encrypt(cell_payload, buf, cell_payload_size, key, iv, ec, num, reinterpret_cast<block128_f>(AES_encrypt));
    memcpy(cell_payload, buf, cell_payload_size);

    BOOST_LOG_TRIVIAL(debug) << "-------------After AesEncrypt-------------";
    Util::HexDump(cell_payload, cell_payload_size);
    BOOST_LOG_TRIVIAL(debug) << "------------------------------------------";
}

void OnionRouter::CalculateDigest(SHA_CTX* digest, RelayCell& cell, unc* result)
{
    unc* cell_payload = cell.GetPayload();
    size_t cell_payload_size = cell.GetPayloadSize();

    SHA1_Update(digest, cell_payload, cell_payload_size);
    SHA_CTX temp;
    memcpy(&temp, digest, sizeof(SHA_CTX));
    SHA1_Final(result, &temp);
}

void OnionRouter::SetDigest(RelayCell& cell) {
    unc buf[DIGEST_LEN];
    CalculateDigest(&forward_digest, cell, buf);
    cell.SetDigest(buf);
}

void OnionRouter::VerifyDigest(RelayCell& cell) {
    unc zero_digest[4];
    unc received_digest[4];
    unc calculated_digest[DIGEST_LEN];
    memset(zero_digest, 0, sizeof(zero_digest));
    cell.GetDigest(received_digest);
    cell.GetDigest(zero_digest);
    CalculateDigest(&backward_digest, cell, calculated_digest);
    if (memcmp(received_digest, calculated_digest, sizeof(received_digest)))
        BOOST_LOG_TRIVIAL(debug) << "Computed digest does not match server's...";
}

void OnionRouter::Encrypt(RelayCell& cell, bool set_digest) {
    if(set_digest) SetDigest(cell);
    AesEncrypt(cell, &forward_key, forward_iv, forward_ec, &forward_num);
}

void OnionRouter::Decrypt(RelayCell& cell, bool verify_digest) {
    AesEncrypt(cell, &backward_key, back_iv, back_ec, &back_num);
    if(verify_digest) VerifyDigest(cell);
}
/* END FILE: /home/josh/tor-connect/torlib/OnionRouter.cpp */
/* BEGIN FILE: /home/josh/tor-connect/torlib/Parser.cpp */
/*-
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * Copyright (c) 2016 Petr Benes https://github.com/wbenny/mini-tor
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



Parser::Parser()
{
	list_control_words[control_words::not_determined] = " ";
	list_control_words[control_words::onion_key] = "onion-key";
	list_control_words[control_words::signing_key] = "signing-key";
	list_control_words[control_words::begin_public_key] = "-----BEGIN RSA PUBLIC KEY-----";
	list_control_words[control_words::end_public_key] = "-----END RSA PUBLIC KEY-----";
	list_control_words[control_words::ntor_onion_key] = "ntor-onion-key";
}

vector<string> Parser::ParsString(const string& in_str, string del)
{
	vector<string> ret_data;
	boost::split(ret_data, in_str, boost::is_any_of(del));
	return ret_data;
}

std::tm Parser::GetValidUntil(const string& in_str)
{
	size_t pos = in_str.find(preamble_control_words);
	string date_time;
	std::tm date;
	if (pos != string::npos)
	{
		date_time = in_str.substr(pos + std::strlen(preamble_control_words.c_str()) + 1, 19);
		std::istringstream str_date(date_time);
		str_date >> std::get_time(&date, "%Y-%m-%d %H:%M:%S");
		if (str_date.fail()) {
			BOOST_LOG_TRIVIAL(warning) << "Parse failed date file consensus";
		}
		else {
			BOOST_LOG_TRIVIAL(debug) << "Date file consensus: " << std::put_time(&date, "%c");
		}
	}
	return date;
}
bool Parser::PortSearch(string in_line, int port_search)
{
	if (port_search == 0) return true;
	bool port_found = false;
	vector<string> ports = ParsString(in_line, ",");
	for (unsigned j = 0; j < ports.size() && !port_found; ++j)
	{
		if (ports[j].find("-") != std::string::npos)
		{
			vector<string> ports_d = ParsString(ports[j], "-");
			if (ports_d.size() == 2)
				port_found = std::stoi(ports_d[0]) <= port_search && port_search <= std::stoi(ports_d[1]);
		}
		else
			port_found = std::stoi(ports[j]) == port_search;
	}
	return port_found;
}

bool Parser::SetOnionRouterKeys(shared_ptr<OnionRouter> onion_node, vector<string>& in_data)
{
	control_words current_location = control_words::not_determined;
	string current_key = "";
	for (string line : in_data)
	{
		if (line == list_control_words[control_words::onion_key])
		{
			current_location = control_words::onion_key;
			continue;
		}
		if (line == list_control_words[control_words::signing_key])
		{
			current_location = control_words::signing_key;
			continue;
		}
		if (line == list_control_words[control_words::begin_public_key]) continue;
		if (line == list_control_words[control_words::end_public_key])
		{
			if (current_location == control_words::onion_key) onion_node->SetOnionKey(current_key);
			if (current_location == control_words::signing_key)	onion_node->SetSignigKey(current_key);
			current_location = control_words::end_public_key;
		}
		if (line.find(list_control_words[control_words::ntor_onion_key]) != string::npos)
		{
			current_location = control_words::ntor_onion_key;
			vector<string> res_pars = ParsString(line, " ");
			if (res_pars.size() > 1 && res_pars[1].length() > 2) onion_node->SetNtorOnionKey(res_pars[1]);
		}
		if (current_location == control_words::onion_key || current_location == control_words::signing_key)	current_key += line;
	}
	return true;
}

vector<string> Parser::SearchOnionRouter(vector<string>& in_data, bool random, int or_port, int dir_port, string sh_ip, vector<string> flags, int search_port)
{
	size_t index = 0;
	if (random) index = Util::GetRandom() % in_data.size();
	else index = 0;
	vector<string> str_data;
	if (in_data.size() < index) return str_data;
	BOOST_LOG_TRIVIAL(debug) << "Search Onion Router ip=" << sh_ip << " or_port=" << or_port << " dir_port=" << dir_port << " search_port =" << search_port;
	do
	{		
		string line = in_data[index];
		if (!line.empty() && line.length() > 2)
		{
			string control_char;// = std::to_string(line[0]);
			control_char.push_back(line[0]);
			control_char.push_back(line[1]);
			boost::trim(control_char);
			if (control_char.length() == 1 && control_char[0] == static_cast<char>(entry_type::entry_r))
			{				
				BOOST_LOG_TRIVIAL(debug) << "Entry line:" << in_data[index];
				unsigned int index_entry[] = { 0, 0 };
				for (int i = 0; i < 7; ++i)
				{
					control_char.clear();
					control_char.push_back(in_data[index + i][0]);
					control_char.push_back(in_data[index + i][1]);
					boost::trim(control_char);
					if (control_char.length() == 1 && control_char[0] == static_cast<char>(entry_type::entry_p))
						index_entry[0] = i;
					if (control_char.length() == 1 && control_char[0] == static_cast<char>(entry_type::entry_s))
						index_entry[1] = i;
				}
				// Search parameters
				bool dop_par = true;
				if (search_port > 0)
				{
					string search_line = in_data[index + index_entry[0]];
					if (search_line.find(reject_ports) != std::string::npos)
					{
						search_line = search_line.substr(reject_ports.length() + 3, search_line.length() - reject_ports.length());
						dop_par = !PortSearch(search_line, search_port);
					}
					else if (search_line.find(accept_ports) != std::string::npos)
					{
						search_line = search_line.substr(accept_ports.length() + 3, search_line.length() - accept_ports.length());
						dop_par = PortSearch(search_line, search_port);
					}
				}
				if (!dop_par)
				{
					BOOST_LOG_TRIVIAL(debug) << "Doesn't fit port:" << in_data[index + index_entry[0]];
					++index;
					continue;
				}
				for (unsigned j = 0; j < flags.size() && dop_par; ++j)
				{
					string s = in_data[index + index_entry[1]];
					size_t pos = in_data[index + index_entry[1]].find(flags[j]);
					dop_par = pos != std::string::npos;
				}
				if (!dop_par)
				{
					BOOST_LOG_TRIVIAL(debug) << "Doesn't fit parametrs:" << in_data[index + index_entry[1]];
					++index;
					continue;
				}
				// Search by conditions 				
				str_data = ParsString(line, " ");
				if (str_data.size() > 1)
				{
					if (or_port > 0) dop_par = std::stoi(str_data[static_cast<int>(entry_r_type::entry_r_or_port)]) == or_port;
					if (dir_port > 0) dop_par = std::stoi(str_data[static_cast<int>(entry_r_type::entry_r_dir_port)]) == dir_port;
					if (dop_par && sh_ip.length() > 0) dop_par = str_data[static_cast<int>(entry_r_type::entry_r_ip)] == sh_ip;

					if (dop_par)
					{
						BOOST_LOG_TRIVIAL(debug) << "---------------- SearchOnionRouter Selected --------------------";
						BOOST_LOG_TRIVIAL(debug) << "nickname=" << str_data[static_cast<int>(entry_r_type::entry_r_nickname)];
						BOOST_LOG_TRIVIAL(debug) << "ip=" << str_data[static_cast<int>(entry_r_type::entry_r_ip)];
						BOOST_LOG_TRIVIAL(debug) << "or_port=" << std::stoi(str_data[static_cast<int>(entry_r_type::entry_r_or_port)]);
						BOOST_LOG_TRIVIAL(debug) << "dir_port=" << std::stoi(str_data[static_cast<int>(entry_r_type::entry_r_dir_port)]);
						BOOST_LOG_TRIVIAL(debug) << "----------------------------------------------------------------";
						return str_data;
					}
					else
					{
						BOOST_LOG_TRIVIAL(debug) << "Doesn't fit conditions:" << line;
					}
				}
			}
		}
		++index;
	} while (in_data.size() > index);
	return str_data;
}

shared_ptr<OnionRouter> Parser::GetOnionRouter(vector<string>& in_data, bool random, int or_port, int dir_port, string sh_ip, vector<string> flags, int search_port)
{
	shared_ptr<OnionRouter> retOn = make_shared<OnionRouter>();
	vector<string> data_node;
	for (int i = 0; i < 10 && data_node.size() == 0; ++i)
		data_node = SearchOnionRouter(in_data, random, or_port, dir_port, sh_ip, flags, search_port);
	if (data_node.size() > 0)
	{
		//vector<string> str_data = ParsString(in_data[index], " ");
		retOn->nickname = data_node[static_cast<int>(entry_r_type::entry_r_nickname)];
		retOn->identity = Util::Base64Decode(data_node[static_cast<int>(entry_r_type::entry_r_identity)]);
		retOn->digest = data_node[static_cast<int>(entry_r_type::entry_r_digest)];
		retOn->publication_date = data_node[static_cast<int>(entry_r_type::entry_r_publication_date)];
		retOn->publication_time = data_node[static_cast<int>(entry_r_type::entry_r_publication_time)];
		retOn->ip = data_node[static_cast<int>(entry_r_type::entry_r_ip)];
		retOn->or_port = std::stoi(data_node[static_cast<int>(entry_r_type::entry_r_or_port)]);
		retOn->dir_port = std::stoi(data_node[static_cast<int>(entry_r_type::entry_r_dir_port)]);
		//retOn->item_count = str_data[router_status_entry_r_item_count];		
	}
	return move(retOn);
}

/* END FILE: /home/josh/tor-connect/torlib/Parser.cpp */
/* BEGIN FILE: /home/josh/tor-connect/torlib/RelayCell.cpp */
/*-
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * Copyright (c) 2009, Moxie Marlinspike
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



RelayCell::RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command)
    : Cell(circuitId, command)
{
    AppendData(streamId, relay_command, 0);
}

RelayCell::RelayCell(u16 streamId, cell_command command, cell_command relay_command)
    : Cell(0, command)
{
    AppendData(streamId, relay_command, 0);
}

RelayCell::RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command, unc* data, int length)
    : Cell(circuitId, command)
{
    AppendData(streamId, relay_command, length);
    Append(data, length);
}

RelayCell::RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command, string& data, bool null_term)
    : Cell(circuitId, command)
{
    AppendData(streamId, relay_command, null_term ? data.length() + 1 : data.length());
    Append(data);
    if (null_term) Append(static_cast<unc>('\0'));
}

RelayCell::RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command, unc payload)
    : Cell(circuitId, command)
{
    AppendData(streamId, relay_command, 1);
    Append(payload);
}

RelayCell::RelayCell(Cell& cell) :Cell()
{
    copy(cell.GetBuffer(), cell.GetBuffer() + GetBufferSize(), GetBuffer());
}

void RelayCell::AppendData(u16 streamId, cell_command relay_command, size_t length) {
    Append(static_cast<unc>(relay_command));                // relay command                    1 byte
    Append(static_cast<u16>(0));                            // recognized                       2 bytes
    Append(streamId);                                       // steram id                        2 bytes
    Append(static_cast<u32>(0));                            // digest placeholder (0 for now)   4 bytes
    Append(static_cast<u16>(0x77));                         // length                           2 bytes
    // Total 11 bytes
}

bool RelayCell::SetLengthRelayPayload(size_t size_data_arg) {
    if(size_data_arg>TOR_MAX_CELL_PAYLOAD_DATA)
    {
        BOOST_LOG_TRIVIAL(error) << "The length of the data is longer than the maximum length of the cell";
        return false;
    }    

    u16 size_data = static_cast<u16>(size_data_arg);
    unc len_payload[RELAY_BYTES_LEN];
    Util::Int16ToArrayBigEndian(len_payload, size_data);
    memcpy(GetBuffer() + RELAY_PAYLOAD_OFFSET, len_payload, RELAY_BYTES_LEN);
    return true;
}

void RelayCell::SetDigest(unc* digest) {
    copy(digest, digest+ DIGEST_LENGTH, GetBuffer() + DIGEST_OFFSET);    
}

void RelayCell::GetDigest(unc* buf) {
    copy(buf, buf + DIGEST_LENGTH, GetBuffer() + DIGEST_OFFSET);
}

unc* RelayCell::GetRelayPayload() {
    return GetBuffer() + RELAY_PAYLOAD_OFFSET + RELAY_BYTES_LEN;
}

int RelayCell::GetRelayPayloadLength() {
    return IsRelayEnd() ? -1 : static_cast<int>(Util::BigEndianArrayToShort(GetBuffer() + RELAY_PAYLOAD_OFFSET));
}

unc RelayCell::GetRelayType() {
    return GetBuffer()[RELAY_TYPE_OFFSET];
}

bool RelayCell::IsRelayEnd() {
    return GetRelayType() == RELAY_END_TYPE;
}

u16 RelayCell::GetStreamId() {
    return Util::BigEndianArrayToShort(GetBuffer() + STREAM_ID_OFFSET);
}


/* END FILE: /home/josh/tor-connect/torlib/RelayCell.cpp */
/* BEGIN FILE: /home/josh/tor-connect/torlib/torlib.cpp */

/*-
* Copyright (c) 2021, Zano project, https://zano.org/
* Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 4. Neither the name of this program nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/




//#include "net/http_client.h"

bool TorLib::Init(log_lv log_level)
{
  // with this filter
  auto filt = boost::log::filter(boost::log::trivial::severity >= log_level);
  boost::log::core::get()->set_filter(filt);

  BOOST_LOG_TRIVIAL(debug) << "TorLib::Init";

  DA.push_back(make_tuple("gabelmoo", "131.188.40.189", 443, 80));
  DA.push_back(make_tuple("moria1", "128.31.0.39", 9101, 9131));
  DA.push_back(make_tuple("tor26", "86.59.21.38", 443, 80));
  DA.push_back(make_tuple("dizum", "45.66.33.45", 443, 80));
  //DA.push_back(make_tuple("Tonga", "82.94.251.203", 443, 80));
  DA.push_back(make_tuple("dannenberg", "193.23.244.244", 443, 80));
  DA.push_back(make_tuple("maatuska", "171.25.193.9", 80, 443));
  DA.push_back(make_tuple("Faravahar", "154.35.175.225", 443, 80));
  DA.push_back(make_tuple("longclaw", "199.58.81.140", 443, 80));
  DA.push_back(make_tuple("bastet", "204.13.164.118", 443, 80));
  DA.push_back(make_tuple("Serge", "66.111.2.131", 9001, 9030));

  circuit_id |= 0x80000000;

  return true;
}

int TorLib::Connect(const string ip, const int port, const int timeout)
{
  pnotifier->notify_state_change(TOR_LIB_STATE_INITIALIZING);
  BOOST_LOG_TRIVIAL(debug) << "TorLib::Connect";
  BOOST_LOG_TRIVIAL(info) << "Connect to " << ip << ":" << port;
  stream_host = ip;
  stream_port = port;
  timeout_global = timeout;
  work = make_shared<net::io_service::work>(io_service);

  if (timeout > 0)
  {
    dtimer = unique_ptr<net::deadline_timer>(new net::deadline_timer(io_service, boost::posix_time::milliseconds(timeout)));
    dtimer->async_wait(boost::bind(&TorLib::OnTimeout, this, pl::error));
  }

  error_last_operation = false;
  int state_op = 1;
  try
  {
    pnotifier->notify_state_change(TOR_LIB_STATE_DOWNLOADING_CONSENSUS);
    BOOST_LOG_TRIVIAL(info) << "Retrieving the file consensus...";
    // Get Consensus
    if (!GetConsensus()) 
      return state_op;


    
    pnotifier->notify_state_change(TOR_LIB_STATE_MAKING_TUNNEL_A);
    // Node 1
    BOOST_LOG_TRIVIAL(info) << "Connect to Node 1 ...";
    state_op = 2;
    // Connect To Node
    if (!ConnectToNode(1)) return state_op;
    if (!SendNodeInfo(boost::bind(&TorLib::LogErr, this, pl::error))) return 3;
    state_op = 4;
    // Get keys	
    if (!GetKeysNode(1)) return state_op;

    if (!CreateNtor(1, boost::bind(&TorLib::LogErr, this, pl::error))) return 5;

    BOOST_LOG_TRIVIAL(debug) << "Connect To Node 1 complete";

    pnotifier->notify_state_change(TOR_LIB_STATE_MAKING_TUNNEL_B);
    // Node 2
    BOOST_LOG_TRIVIAL(info) << "Connect to Node 2 ...";
    state_op = 6;
    // Connect To Node
    if (!ConnectToNode(2, port)) return state_op;
    state_op = 7;
    // Get keys	
    if (!GetKeysNode(2)) return state_op;
    state_op = 8;
    if (!CreateExtendNtor(2, boost::bind(&TorLib::LogErr, this, pl::error))) return state_op;

    BOOST_LOG_TRIVIAL(debug) << "Connect To Node 2 complete";

    pnotifier->notify_state_change(TOR_LIB_STATE_CREATING_STREAM);
    BOOST_LOG_TRIVIAL(info) << "Create stream ...";
    n_stream = 1;
    state_op = 9;
    if (!CreateStream(3, n_stream, ip, port, timeout, boost::bind(&TorLib::LogErr, this, pl::error))) return state_op;

    BOOST_LOG_TRIVIAL(info) << "Connect completed.";
    state_op = 0;
    pnotifier->notify_state_change(TOR_LIB_STATE_SUCCESS);
  }
  catch (...)
  {
    BOOST_LOG_TRIVIAL(error) << "Unsuccessful connection to the Tor network.";
    pnotifier->notify_state_change(TOR_LIB_STATE_FAILED);
  }
  return state_op;
}

void TorLib::OnTimeout(const sys::error_code& err)
{
  if (!err && err != net::error::operation_aborted)
  {
    BOOST_LOG_TRIVIAL(error) << "Time out.";
    error_last_operation = true;
    io_service.stop();
    net_connect->ShutDown();
  }
}
void TorLib::SetNotifier(tools::tor::t_transport_state_notifier* pn)
{
  pnotifier = pn;
}
void TorLib::TransferExternalConsensus(std::vector<std::string>& local_consensus_data)
{
  local_consensus_data.swap(data_consensus);
  last_consensus_receive_time = time(nullptr);
}

void TorLib::WithdrawExternalConsensus(std::vector<std::string>& local_consensus_data)
{
  local_consensus_data.swap(data_consensus);
  last_consensus_receive_time = time(nullptr);
}

bool TorLib::Receive(string& buff, const int timeout)
{
  operation_completed = false;
  BOOST_LOG_TRIVIAL(debug) << "TorLib::Receive";
  if (timeout > 0)
  {
    dtimer = unique_ptr<net::deadline_timer>(new net::deadline_timer(io_service, boost::posix_time::milliseconds(timeout)));
    dtimer->async_wait(boost::bind(&TorLib::OnTimeout, this, pl::error));
  }
  data_result = "";
  try
  {
    ReadStreamOne(3, boost::bind(&TorLib::LogErr, this, pl::error));
    while (!operation_completed) io_service.poll_one();
  }
  catch (...)
  {
    BOOST_LOG_TRIVIAL(error) << "Unsuccessful read from Tor network.";
  }
  buff = data_result;
  return !error_last_operation;
}
bool TorLib::Send(const string& req_)
{
  //BOOST_LOG_TRIVIAL(debug) << "TorLib::Send : " << req;
  //string req = (boost::format("GET %1% HTTP/1.0\r\nHost: %2%\r\n\r\n") % path % stream_host).str();	
  std::string req = req_;
  while (req.size())
  {
    std::string buff_to_send;
    if (req.size() > TOR_MAX_CELL_PAYLOAD_DATA)
    {
      buff_to_send = req.substr(0, TOR_MAX_CELL_PAYLOAD_DATA);
      req.erase(0, TOR_MAX_CELL_PAYLOAD_DATA);
    }
    else
    {
      buff_to_send.swap(req);
    }
    bool ret_val = SendData(buff_to_send/*, boost::bind(&TorLib::LogErr, this, pl::error)*/);
    if (!ret_val)
    {
      BOOST_LOG_TRIVIAL(info) << "Send failed.";
      return false;
    }
  }
  BOOST_LOG_TRIVIAL(info) << "Request sent.(" << req_.size() << " bytes)";
  return true;
}
bool TorLib::SendData(string reqest /* , ConnectFunction connectFunc*/)
{
  operation_completed = false;
  BOOST_LOG_TRIVIAL(debug) << "TorLib::SendData reqest = " << reqest;
  RelayCell circuit_node(circuit_id, n_stream, cell_command::relay, cell_command::relay_data);
  circuit_node.Append(reqest);
  if (!circuit_node.SetLengthRelayPayload(reqest.size())) 
    return false;

  onion_routers[3]->Encrypt(circuit_node);
  onion_routers[2]->Encrypt(circuit_node, false);
  //net_connect->WriteCell(circuit_node,
  //	boost::bind(&TorLib::ReadStreamData, this, 3, connectFunc, pl::error));

  net_connect->WriteCell(circuit_node, boost::bind(&TorLib::LogErr, this, pl::error));

  while (!operation_completed) io_service.poll_one();
  return !error_last_operation;
}
void TorLib::ReadStreamOne(int n_node, ConnectFunction connectFunc)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::ReadStreamOne";
  shared_ptr<Cell> node(new Cell());
  net_connect->ReadCell(node, boost::bind(&TorLib::ReadStreamComplete,
    this, n_node, connectFunc, node, pl::error));
}
void TorLib::ReadStreamData(int n_node, ConnectFunction connectFunc, const sys::error_code& err)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::ReadStreamData";
  TEST_ERR(err, connectFunc);
  shared_ptr<Cell> node(new Cell());
  net_connect->ReadCell(node, boost::bind(&TorLib::ReadStreamComplete,
    this, n_node, connectFunc, node, pl::error));
}
void TorLib::ReadStreamComplete(int n_node, ConnectFunction connectFunc,
  shared_ptr<Cell> node, const sys::error_code& err)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::ReadStreamCompleteP1";
  TEST_ERR(err, connectFunc);
  shared_ptr<RelayCell> relay_node(new RelayCell(*node));
  onion_routers[n_node]->Decrypt(*relay_node, false);
  onion_routers[n_node - 1]->Decrypt(*relay_node, false);
  BOOST_LOG_TRIVIAL(debug) << "TorLib::ReadStreamComplete Command=" << static_cast<unsigned int>(relay_node->GetCommand());
  BOOST_LOG_TRIVIAL(debug) << "TorLib::ReadStreamComplete Command Relay=" << static_cast<unsigned int>(relay_node->GetRelayType());

  BOOST_LOG_TRIVIAL(debug) << "-------------RelayCell-------------";
  Util::HexDump(relay_node->GetBuffer(), relay_node->GetBufferSize());
  BOOST_LOG_TRIVIAL(debug) << "-----------------------------------";

  data_result.clear();
  data_result.append(reinterpret_cast<char const*>(relay_node->GetRelayPayload()), relay_node->GetRelayPayloadLength());
  net::post(net::detail::bind_handler(connectFunc, err));
}
bool TorLib::CreateStream(int n_node, u16 id_stream, string host, int port, int timeout, ConnectFunction connectFunc)
{
  operation_completed = false;
  BOOST_LOG_TRIVIAL(debug) << "TorLib::CreateStream to " << host << ":" << port;
  string host_port;
  host_port.append(host);
  host_port.append(":");
  host_port.append(std::to_string(port));

  RelayCell circuit_node(circuit_id, id_stream, cell_command::relay, cell_command::relay_begin);
  circuit_node.Append(host_port);
  onion_routers[n_node] = make_shared<OnionRouter>();
  onion_routers[n_node]->SetKeyMaterial(onion_routers[n_node - 1]->key_material.data());
  onion_routers[n_node]->Encrypt(circuit_node);
  onion_routers[n_node - 1]->Encrypt(circuit_node, false);

  net_connect->WriteCell(circuit_node,
    boost::bind(&TorLib::ReadStreamNode, this, n_node, connectFunc, pl::error));

  while (!operation_completed) io_service.poll_one();
  return !error_last_operation;
}
void TorLib::ReadStreamNode(int n_node, ConnectFunction connectFunc, const sys::error_code& err)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::ReadStreamNode";
  TEST_ERR(err, connectFunc);
  shared_ptr<Cell> node(new Cell());
  net_connect->ReadCell(node, boost::bind(&TorLib::CreateStreamComplete,
    this, n_node, connectFunc, node, pl::error));
}

void TorLib::CreateStreamComplete(int n_node, ConnectFunction connectFunc,
  shared_ptr<Cell> node, const sys::error_code& err)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::CreateStreamComplete";
  TEST_ERR(err, connectFunc);
  shared_ptr<RelayCell> relay_node(new RelayCell(*node));
  onion_routers[n_node]->Decrypt(*relay_node, false);
  onion_routers[n_node - 1]->Decrypt(*relay_node);
  BOOST_LOG_TRIVIAL(debug) << "TorLib::CreateStreamComplete Command=" << static_cast<unsigned int>(relay_node->GetCommand());
  BOOST_LOG_TRIVIAL(debug) << "TorLib::CreateStreamComplete Command Relay=" << static_cast<unsigned int>(relay_node->GetRelayType());

  BOOST_LOG_TRIVIAL(debug) << "-------------RelayCell-------------";
  Util::HexDump(relay_node->GetBuffer(), relay_node->GetBufferSize());
  BOOST_LOG_TRIVIAL(debug) << "-----------------------------------";

  net::post(net::detail::bind_handler(connectFunc, err));
}

bool TorLib::ConnectToNode(int n_node, int search_port)
{
  if (n_node == 1)
  {
    BOOST_LOG_TRIVIAL(debug) << "TorLib::ConnectToNode 1";

    ssl::context ctx(ssl::context::sslv23);
    //ctx.load_verify_file("torlib/cacert.pem");

    operation_completed = false;
    net_connect = std::unique_ptr<NetConnect>(new NetConnect(io_service, ctx));
    onion_routers[n_node] = parser.GetOnionRouter(data_consensus, true, 443, 0, "", { "Fast","Running", "Valid" });

    if (onion_routers[n_node]->nickname.length() == 0)
    {
      BOOST_LOG_TRIVIAL(debug) << "Onion Router not found";
      return false;
    }
    BOOST_LOG_TRIVIAL(info) << "Connecting to node " << n_node << ": '"
      << onion_routers[n_node]->nickname << "' (" << onion_routers[n_node]->ip << ":" << onion_routers[n_node]->or_port << ")";
    net_connect->Connect(onion_routers[n_node]->ip, onion_routers[n_node]->or_port,
      boost::bind(&TorLib::LogErr, this, pl::error));

    while (!operation_completed) io_service.poll_one();
    return !error_last_operation;
  }
  if (n_node == 2)
  {
    BOOST_LOG_TRIVIAL(debug) << "TorLib::ConnectToNode 2";
    onion_routers[n_node] = parser.GetOnionRouter(data_consensus, true, 0, 0, "", { "Exit", "Fast","Running", "Valid" }, search_port);
    if (onion_routers[n_node]->nickname.length() == 0)
    {
      BOOST_LOG_TRIVIAL(debug) << "Onion Router not found";
      return false;
    }

    BOOST_LOG_TRIVIAL(info) << "Node 2: " << n_node << ": '"
      << onion_routers[n_node]->nickname << "' (" << onion_routers[n_node]->ip << ":" << onion_routers[n_node]->or_port << ")";
    return true;
  }
  BOOST_LOG_TRIVIAL(debug) << "TorLib::ConnectToNode Incorrect call parameters";
  return false;
}

bool TorLib::GetKeysNode(int n_node)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::GetKeysNode";
  operation_completed = false;
  if (data_consensus.size() == 0)
  {
    BOOST_LOG_TRIVIAL(error) << "TorLib::GetKeysNode data_consensus empty";
    return false;
  }
  vector<string> data_node = parser.SearchOnionRouter(data_consensus, true, 0, 80, "", {});

  if (data_node.size() == 0)
  {
    BOOST_LOG_TRIVIAL(debug) << "Onion Router not found";
    return false;
  }
  string ip = data_node[static_cast<int>(entry_r_type::entry_r_ip)];
  int port = std::stoi(data_node[static_cast<int>(entry_r_type::entry_r_dir_port)]);
  BOOST_LOG_TRIVIAL(debug) << "Get Keys from Node " << n_node << ": '"
    << data_node[static_cast<int>(entry_r_type::entry_r_nickname)] << "' (" << ip << ":" << port << ")";
  string target = "/tor/server/fp/" + onion_routers[n_node]->GetBase16EncodedIdentity(onion_routers[n_node]->identity);
  data_result = GetDataFromUrlAsync(ip, port, target);
  if (!data_result.empty())
  {
    BOOST_LOG_TRIVIAL(debug) << "TorLib::SetOnionRouterKeys";
    vector<string> data_node = parser.ParsString(data_result, "\n");
    if (data_node.size() == 0) return false;
    parser.SetOnionRouterKeys(onion_routers[n_node], data_node);
  }
  else
  {
    BOOST_LOG_TRIVIAL(debug) << "TorLib::ConnectToNodeComplete [data_result.empty()]";
    return false;
  }
  return true;
}

bool TorLib::GetConsensus()
{
  if (data_consensus.size() != 0 && time(nullptr) - last_consensus_receive_time < 60)
  {
    BOOST_LOG_TRIVIAL(info) << "Reusing downloaded consensus";
    return true;
  }

  BOOST_LOG_TRIVIAL(info) << "Downloading consensus...";
  tuple<string, string, int, int> sv_one;
  data_consensus.clear();
  int rnd = Util::GetRandom() % DA.size();
  //int rnd = 0;
  sv_one = DA[rnd];
  BOOST_LOG_TRIVIAL(debug) << "Connect to " << get<0>(sv_one) << ":" << get<3>(sv_one);
  data_result = GetDataFromUrlAsync(get<1>(sv_one), get<3>(sv_one), "/tor/status-vote/current/consensus");
  if (data_result.empty())
  {
    BOOST_LOG_TRIVIAL(error) << "The file Consensus was not received!("<< get<1>(sv_one) << ":" << get<3>(sv_one) << ")";
    return false;
  }
  data_consensus = parser.ParsString(data_result, "\n");
  last_consensus_receive_time = time(nullptr);
  return data_consensus.size() != 0;
}

void TorLib::LogErr(const sys::error_code& err)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::LogErr";
  if (err) {
    BOOST_LOG_TRIVIAL(error) << err.message();
    error_last_operation = true;
  }
  operation_completed = true;
}

TorLib::TorLib(): pnotifier(&notifier_dummy)
{
}

TorLib::~TorLib()
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::~TorLib";
}
bool TorLib::Close()
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::Close";
  net_connect.release();
  onion_routers.clear();
  stream_host.clear();
  stream_port = 0;
  n_stream = 0;
  data_consensus.clear();
  data_result.clear();
  return true;
}

bool TorLib::CreateNtor(int n_node, ConnectFunction connectFunc)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::CreateNtor ";
  operation_completed = false;

  Cell circuit_node(circuit_id, cell_command::create2);
  circuit_node.Append(static_cast<u16>(2)); // ntor type
  circuit_node.Append(static_cast<u16>(84));  // ntor onion skin length
                                              // Generating keys
  onion_routers[n_node]->GeneratPairKeys();
  // Add ntor onion skin
  circuit_node.Append(onion_routers[n_node]->identity);
  circuit_node.Append(onion_routers[n_node]->ntor_onion_key);
  circuit_node.Append(onion_routers[n_node]->GetPublicKey(), onion_routers[n_node]->GetPublicKeySize());
  net_connect->WriteCell(circuit_node, boost::bind(&TorLib::ReadCNtor, this,
    n_node, connectFunc, pl::error));
  while (!operation_completed) io_service.poll_one();
  return !error_last_operation;
}
bool TorLib::CreateExtendNtor(int n_node, ConnectFunction connectFunc)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::CreateExtendNtor";
  operation_completed = false;
  RelayCell circuit_node(circuit_id, 0, cell_command::relay_early, cell_command::relay_extend2);
  circuit_node.Append(static_cast<u8>(2)); // 2x NSPEC
                                           // NSPEC IPv4 (4 bytes) + port (2 bytes)
  circuit_node.Append(static_cast<u8>(link_specifier_type::ipv4));
  circuit_node.Append(static_cast<u8>(6));
  circuit_node.Append(net::ip::address_v4::from_string(onion_routers[n_node]->ip.c_str()).to_uint());
  circuit_node.Append(static_cast<u16>(onion_routers[n_node]->or_port));
  // NSPEC identity_fingerprint (20 bytes)
  circuit_node.Append(static_cast<u8>(link_specifier_type::legacy_id));
  circuit_node.Append(static_cast<u8>(20));
  circuit_node.Append(onion_routers[n_node]->identity);

  circuit_node.Append(static_cast<u16>(2));// HTYPE
  circuit_node.Append(static_cast<u16>(84));// HLEN

  onion_routers[n_node]->GeneratPairKeys();// Generating keys
                                           // HDATA
  circuit_node.Append(onion_routers[n_node]->identity);
  circuit_node.Append(onion_routers[n_node]->ntor_onion_key);
  circuit_node.Append(onion_routers[n_node]->GetPublicKey(), onion_routers[n_node]->GetPublicKeySize());

  onion_routers[n_node]->SetKeyMaterial(onion_routers[n_node - 1]->key_material.data());
  onion_routers[n_node]->Encrypt(circuit_node);

  net_connect->WriteCell(circuit_node,
    boost::bind(&TorLib::ReadExtendNtor, this, n_node, connectFunc, pl::error));

  while (!operation_completed) io_service.poll_one();
  return !error_last_operation;
}

void TorLib::ReadExtendNtor(int n_node, ConnectFunction connectFunc, const sys::error_code& err)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::ReadExtendNtor";
  TEST_ERR(err, connectFunc);
  shared_ptr<Cell> node(new Cell());
  net_connect->ReadCell(node, boost::bind(&TorLib::CreateExtendNtorComplete,
    this, n_node, connectFunc, node, pl::error));
}

void TorLib::CreateExtendNtorComplete(int n_node, ConnectFunction connectFunc,
  shared_ptr<Cell> node, const sys::error_code& err)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::CreateExtendNtorComplete";
  TEST_ERR(err, connectFunc);
  shared_ptr<RelayCell> relay_node(new RelayCell(*node));
  onion_routers[n_node]->Decrypt(*relay_node);
  BOOST_LOG_TRIVIAL(debug) << "TorLib::CreateExtendNtorComplete Command=" << static_cast<unsigned int>(relay_node->GetCommand());
  sys::error_code loc_err;
  if (onion_routers[n_node]->GeneratKeyMaterial(*relay_node)) loc_err = err;
  else loc_err = net::error::bad_descriptor;
  net::post(net::detail::bind_handler(connectFunc, loc_err));
}

void TorLib::ReadCNtor(int n_node, ConnectFunction connectFunc,
  const sys::error_code& err)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::ReadCNtor";
  TEST_ERR(err, connectFunc);
  shared_ptr<Cell> node(new Cell());
  net_connect->ReadCell(node, boost::bind(&TorLib::CreateNtorComplete,
    this, n_node, connectFunc, node, pl::error));
}

void TorLib::CreateNtorComplete(int n_node, ConnectFunction connectFunc,
  shared_ptr<Cell> node, const sys::error_code& err)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::CreateNtorComplete";
  TEST_ERR(err, connectFunc);
  BOOST_LOG_TRIVIAL(debug) << "TorLib::CreateNtorComplete Command=" << static_cast<unsigned int>(node->GetCommand());
  sys::error_code loc_err;
  if (onion_routers[n_node]->GeneratKeyMaterial(node)) loc_err = err;
  else loc_err = net::error::bad_descriptor;
  net::post(net::detail::bind_handler(connectFunc, loc_err));
}


bool TorLib::SendNodeInfo(ConnectFunction connectFunc)
{
  BOOST_LOG_TRIVIAL(debug) << "TorLib::SendNodeInfo";
  operation_completed = false;

  net_connect->SendVersion(boost::bind(&TorLib::LogErr, this, pl::error));

  while (!operation_completed) io_service.poll_one();
  if (error_last_operation) return false;

  operation_completed = false;
  //unc loc_host[] = { 0xc0, 0xa8, 0x01, 0x01 }; // Nobody seems to care.
  long rem_host = net_connect->GetEndpointLong();

  Cell cell_Info(0, cell_command::netinfo);
  cell_Info.Append(static_cast<uint32_t>(time(0)));   // Timestamp
  cell_Info.Append(static_cast<unc>(0x04)); // Type (host)
  cell_Info.Append(static_cast<unc>(0x04)); // Address Length  
  cell_Info.Append(reinterpret_cast<unc*>(&rem_host), 4); // Address
  cell_Info.Append(static_cast<unc>(0x01)); // Address Count
  cell_Info.Append(static_cast<unc>(0x04)); // Type (ipv4)
  cell_Info.Append(static_cast<unc>(0x04));

  net_connect->WriteCell(cell_Info, boost::bind(&TorLib::LogErr, this, pl::error));
  while (!operation_completed) io_service.poll_one();
  return !error_last_operation;
}
/*
string TorLib::GetDataFromUrl(const string host, const int port, const string target)
{
//   BOOST_LOG_TRIVIAL(debug) << "TorLib::GetDataFromUrl " << host << ":" << port << " target=" << target;
//   // The io_context is required for all I/O
//   net::io_context ioc;
//   // These objects perform our I/O
//   tcp::resolver resolver(ioc);
//   beast::tcp_stream stream(ioc);
//   // Look up the domain name
//   auto const results = resolver.resolve(host, std::to_string(port));
//   // Make the connection on the IP address we get from a lookup
//   stream.connect(results);
//   // Set up an HTTP GET request message
//   http::request<http::string_body> req{ http::verb::get, target, 11 };
//   req.set(http::field::host, host);
//   req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
//   // Send the HTTP request to the remote host
//   //http::async_write();
//   http::write(stream, req);
//   // This buffer is used for reading and must be persisted
//   beast::flat_buffer buffer;
//   // Declare a container to hold the response
//   http::response<http::dynamic_body> res;
//   // Receive the HTTP response
//   http::read(stream, buffer, res);
//   // Gracefully close the socket
//   beast::error_code ec;
//   stream.socket().shutdown(tcp::socket::shutdown_both, ec);
//   // not_connected happens sometimes
//   // so don't bother reporting it.
//   //
//   if (ec && ec != beast::errc::not_connected)
//   {
//     BOOST_LOG_TRIVIAL(error) << ec.value();
//     return "";
//   }
//   std::string ret_str = boost::beast::buffers_to_string(res.body().data());
// 
//   return ret_str;

  epee::net_utils::http::http_simple_client cli;
  if (!cli.connect(host, port, timeout_global))
  {
    BOOST_LOG_TRIVIAL(error) << "Failed to connect to " << host << ":" << port;
    return "";
  }
  const epee::net_utils::http::http_response_info* resp_ptr = nullptr;
  if (!cli.invoke_get(target, std::string(), &resp_ptr))
  {
    BOOST_LOG_TRIVIAL(error) << "Failed to invoke to " << host << ":" << port << "  " << target;
    return "";
  }

  if (!resp_ptr)
  {
    BOOST_LOG_TRIVIAL(error) << "Failed to invoke to " << host << ":" << port << "  " << target;
    return "";
  }

  return resp_ptr->m_body;
}

*/
string TorLib::GetDataFromUrlAsync(const string host, const int port, const string target)
{
  /*epee::net_utils::http::http_simple_client cli;
  if (!cli.connect(host, port, timeout_global))
  {
    BOOST_LOG_TRIVIAL(error) << "Failed to connect to " << host << ":" << port;
    return "";
  }
  const epee::net_utils::http::http_response_info* resp_ptr = nullptr;
  if (!cli.invoke_get(target, std::string(), &resp_ptr))
  {
    BOOST_LOG_TRIVIAL(error) << "Failed to invoke to " << host << ":" << port << "  " << target;
    return "";
  }

  if (!resp_ptr)
  {
    BOOST_LOG_TRIVIAL(error) << "Failed to invoke to " << host << ":" << port << "  " << target;
    return "";
  }

  return resp_ptr->m_body;
  */

  BOOST_LOG_TRIVIAL(debug) << "TorLib::GetDataFromUrlAsync " << host << ":" << port << " target=" << target;
  net::io_context ioc;
  shared_ptr<HTTPClient> client = make_shared<HTTPClient>(ioc);
  client->RunClient(host.c_str(), port, target, timeout_global);
  ioc.run();
  return client->GetData();
}
/* END FILE: /home/josh/tor-connect/torlib/torlib.cpp */
/* BEGIN FILE: /home/josh/tor-connect/torlib/tor_wrapper.cpp */

/*-
* Copyright (c) 2021, Zano project, https://zano.org/
* Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
* Copyright (c) 2009, Moxie Marlinspike
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 4. Neither the name of this program nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/


namespace tools
{
  namespace tor
  {

    tor_transport::tor_transport() :m_is_connected(false), m_recev_timeout(0)
    {
      
    }

    bool tor_transport::connect(const std::string& ip, int port, int timeout, int recev_timeout, const std::string& /*bind_ip*/)
    {
      if (m_ptransport.get())
      {
        std::vector<std::string> consensus;
        m_ptransport->WithdrawExternalConsensus(consensus);
        m_ptransport.reset(new TorLib());
        m_ptransport->TransferExternalConsensus(consensus);
      }
      else
      {
        m_ptransport.reset(new TorLib());
      }
      if (!m_ptransport->Init(boost::log::trivial::warning))
      {
        return false;
      }
      m_ptransport->SetNotifier(m_pn);

      m_recev_timeout = recev_timeout;

      int res = m_ptransport->Connect(ip, port, timeout);
      if (res == 0)
      {
        m_is_connected = true;
        return true;
      }
      return false;
    }
    bool tor_transport::disconnect()
    {
      return m_ptransport->Close();
    }
    bool tor_transport::is_connected()
    {
      return m_is_connected;
    }
    bool tor_transport::send(const void* data, size_t sz)
    {
      //todo: subject for future refactoring, extra copy operation
      std::string str((const char*)data, sz);
      return this->send(str);
    }
    bool tor_transport::send(const std::string& buff)
    {
      return m_ptransport->Send(buff);
    }

    bool tor_transport::recv_n(std::string& buff, size_t sz)
    {
      while (m_intermediate_buff.size() < sz)
      {
        std::string new_buff;
        if (!m_ptransport->Receive(new_buff, m_recev_timeout))
        {
          return false;
        }
        m_intermediate_buff.append(new_buff);
      }

      buff = m_intermediate_buff.substr(0, sz);
      m_intermediate_buff.erase(0, sz);
      return true;
    }

    void tor_transport::set_notifier(tools::tor::t_transport_state_notifier* pn)
    {
      m_pn = pn;
    }

  }
}
/* END FILE: /home/josh/tor-connect/torlib/tor_wrapper.cpp */
/* BEGIN FILE: /home/josh/tor-connect/torlib/Util.cpp */
/*-
 * Copyright (c) 2021, Zano project, https://zano.org/
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * Copyright (c) 2009, Moxie Marlinspike
 * Copyright (c) 2008, John Millikin,  https://stackoverflow.com/a/180949/5566653
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


static const string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

uint16_t Util::BigEndianArrayToShort(unc* buf) {
	return (uint16_t)((buf[0] & 0xff) << 8 | (buf[1] & 0xff));
}

uint32_t Util::BigEndianArrayToInt(unc* buf) {
	return (uint32_t)((buf[0] & 0xff) << 24 | (buf[1] & 0xff) << 16 |
		(buf[2] & 0xff) << 8 | (buf[3] & 0xff));
}

void Util::Int64ToArrayBigEndian(unc* a, u64 i) {
	a[0] = (i >> 56) & 0xFF;
	a[1] = (i >> 48) & 0xFF;
	a[2] = (i >> 40) & 0xFF;
	a[3] = (i >> 32) & 0xFF;
	a[4] = (i >> 24) & 0xFF;
	a[5] = (i >> 16) & 0xFF;
	a[6] = (i >> 8) & 0xFF;
	a[7] = i & 0xFF;
}


void Util::Int32ToArrayBigEndian(unc* a, u32 i) {
	a[0] = (i >> 24) & 0xFF;
	a[1] = (i >> 16) & 0xFF;
	a[2] = (i >> 8) & 0xFF;
	a[3] = i & 0xFF;
}

void Util::Int16ToArrayBigEndian(unc* a, u16 i) {
	a[0] = (i >> 8) & 0xff;
	a[1] = i & 0xff;
}
void Util::HexDump(unc* buffer, size_t length) {
	stringstream str;
	str.setf(std::ios_base::hex, std::ios::basefield);
	str.fill('0');

	for (size_t i = 0; i < length; ++i) {
		str << std::setw(2) << static_cast<unsigned short>(buffer[i]) << " ";
	}
	BOOST_LOG_TRIVIAL(debug) << str.str();
}

void Util::Base16Encode(char* dest, size_t destlen, const char* src, size_t srclen)
{
	const char* end;
	char* cp;

	assert(destlen >= srclen * 2 + 1);

	cp = dest;
	end = src + srclen;
	while (src < end) {
		*cp++ = "0123456789ABCDEF"[(*(const uint8_t*)src) >> 4];
		*cp++ = "0123456789ABCDEF"[(*(const uint8_t*)src) & 0xf];
		++src;
	}
	*cp = '\0';
}
uint16_t Util::GetRandomId() {
	unc id[2];
	RAND_bytes(id, sizeof(id));

	return BigEndianArrayToShort(id);
}

uint32_t Util::GetRandom() {
	unc bytes[4];
	RAND_bytes(bytes, sizeof(bytes));

	return BigEndianArrayToInt(bytes);
}
uint8_t Util::GetRandom8() {
	unc buf[1];
	RAND_bytes(buf, sizeof(buf));
	return buf[0] & 0xff;
}

bool Util::IsBase64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

string Util::Base64Decode(string const& encoded_string) {
	size_t in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && IsBase64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}
/* END FILE: /home/josh/tor-connect/torlib/Util.cpp */
/* END FILE: /home/josh/tor-connect/torlib/inc.c */
/* AMALGAMATED OUTPUT END (entry: /home/josh/tor-connect/torlib/inc.c) */
