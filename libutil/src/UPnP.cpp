// $Id: UPnP.cpp 6465 2010-06-03 08:31:48Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Siedler II.5 RTTR.
//
// Siedler II.5 RTTR is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Siedler II.5 RTTR is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Siedler II.5 RTTR. If not, see <http://www.gnu.org/licenses/>.

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "UPnP.h"

#ifdef _WIN32
	#undef DATADIR

	#include <iphlpapi.h>
	#include <ole2.h>
	#include <natupnp.h>
	#include <atlconv.h>

	#pragma comment(lib, "iphlpapi.lib")
#endif // _WIN32

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Standardkonstruktor von @p UPnP.
 *
 *  @author FloSoft
 */
UPnP::UPnP() : remote_port_(0)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p UPnP.
 *
 *  @author FloSoft
 */
UPnP::~UPnP()
{
	ClosePort();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Erstellt per UPnP ein Portforwarding.
 *
 *  @author FloSoft
 */
bool UPnP::OpenPort(const unsigned short& port)
{
	if(remote_port_ != 0)
		ClosePort();

	remote_port_ = port;

#ifdef _WIN32
	HRESULT hr;
	
	IUPnPNAT* upnpnat;
	hr = CoCreateInstance (__uuidof(UPnPNAT), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUPnPNAT), (void**)&upnpnat);
	if(FAILED(hr) || !upnpnat)
		return false;

	IStaticPortMappingCollection* upnpspmc = NULL;
	hr = upnpnat->get_StaticPortMappingCollection(&upnpspmc);
	if(FAILED(hr) || !upnpspmc)
		return false;


	std::string local_address;
	std::vector<std::string> addresses = GetAllv4Addresses();

	// if we have multiple addresses, search the private one
	if(addresses.size() > 1)
	{
		for(std::vector<std::string>::iterator addr = addresses.begin(); addr != addresses.end(); ++addr)
		{
			std::string ss = *addr;
			std::stringstream s, sc;
			s << ss;
			std::getline(s, ss, '.');
			sc << ss << " ";
			std::getline(s, ss, '.');
			sc << ss << " ";

			int a, b;
			sc >> a;
			sc >> b;

			int ab = (a << 24) | (b << 16);

			if( (ab & 0xff000000) == 0x0a000000 || // 10.0.0.0/8
				(ab & 0xff000000) == 0x7f000000 || // 127.0.0.0/8
				(ab & 0xfff00000) == 0xac100000 || // 172.16.0.0/12
				(ab & 0xffff0000) == 0xc0a80000 )  // 192.168.0.0/16
				local_address = *addr;
		}
	}

	// otherwise use the first one
	if(local_address == "" && addresses.size())
		local_address = addresses.front();

	// I hope we found one ...
	if(local_address == "")
		return false;

	BSTR bstrProtocol = A2BSTR("TCP");
	BSTR bstrLocalAddress = A2BSTR(local_address.c_str());
	BSTR bstrDescription = A2BSTR("Return To The Roots");

	IStaticPortMapping* upnpspm = NULL;
	hr = upnpspmc->Add(port, bstrProtocol, port, bstrLocalAddress, VARIANT_TRUE, bstrDescription, &upnpspm);

	SysFreeString(bstrProtocol);
	SysFreeString(bstrLocalAddress);
	SysFreeString(bstrDescription);

	SetLastError(hr);
	
	if(SUCCEEDED(hr) && upnpspm)
		return true;
#else
	#warning "OpenPort: not implemented"
#endif
	return false;
}

void UPnP::ClosePort()
{
	if(remote_port_ == 0)
		return;

#ifdef _WIN32
	HRESULT hr;
	
	IUPnPNAT* upnpnat;
	hr = CoCreateInstance (__uuidof(UPnPNAT), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUPnPNAT), (void**)&upnpnat);
	if(FAILED(hr) || !upnpnat)
		return;

	IStaticPortMappingCollection* upnpspmc = NULL;
	hr = upnpnat->get_StaticPortMappingCollection(&upnpspmc);
	if(FAILED(hr) || !upnpspmc)
		return;

	BSTR bstrProtocol = A2BSTR("TCP");

	hr = upnpspmc->Remove(remote_port_, bstrProtocol);

	if(FAILED(hr))
		LOG.getlasterror("Automatisches Entfernen des Portforwardings mit UPnP fehlgeschlagen\nFehler");

	SysFreeString(bstrProtocol);

	if(FAILED(hr))
		return;
#else
	#warning "ClosePort: not implemented"
#endif

	remote_port_ = 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Liest alle Adapteradressen aus.
 *
 *  @author FloSoft
 */
std::vector<std::string> UPnP::GetAllv4Addresses()
{
	std::vector<std::string> addresses;

#ifdef _WIN32
	// read addresses from adapters
	ULONG OutBufLen = 0;
	GetAdaptersInfo(NULL, &OutBufLen);

	PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO)HeapAlloc(GetProcessHeap(), 0, OutBufLen);

	if(GetAdaptersInfo(pAdapterInfo, &OutBufLen) == NO_ERROR)
	{
		PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
        while (pAdapter) 
		{
			std::string address = pAdapter->IpAddressList.IpAddress.String;
			if(address != "0.0.0.0")
				addresses.push_back(address);

			pAdapter = pAdapter->Next;
		}
	}

	HeapFree(GetProcessHeap(), 0, pAdapterInfo);

	// and failback solution: read address from hostname
	char host[512];
	gethostname(host, 512);

	struct hostent *hosts = gethostbyname(host);
	if(hosts)
	{
		for (int i = 0; hosts->h_addr_list[i] != 0; ++i) 
		{
			struct in_addr addr;
			memcpy(&addr, hosts->h_addr_list[i], sizeof(struct in_addr));
			std::string address = inet_ntoa(addr);
			if(address != "0.0.0.0")
				addresses.push_back(address);
		}
	}
#else
	#warning "GetAllAddresses: not implemented"
#endif

	// remove duplicates
	std::sort(addresses.begin(), addresses.end());
	std::vector<std::string>::iterator begin = std::unique( addresses.begin(), addresses.end() );
	addresses.erase(begin, addresses.end());

	return addresses;
}