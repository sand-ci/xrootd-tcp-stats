#include "tcpstats.h"
#include <Xrd/XrdTcpMonPin.hh>
#include <picojson.h>
#include <XrdVersion.hh>
#include <XrdOuc/XrdOucEnv.hh>

XrdVERSIONINFO(TcpMonPin,XRootDTCP);



XrdTcpMonPin* myPinObject::getInstance(const char   *parms,
                            XrdOucEnv    &envR,
                            XrdSysLogger &logR,
                            XrdTcpMonPin *prevP)
{

    // Create the tcp stats object
    XrdXrootdGStream *gS = (XrdXrootdGStream *)envR.GetPtr("TcpMon.gStream*");
    TCPStats* tcpstats = new TCPStats(gS);
    return tcpstats;

}

TCPStats::TCPStats(XrdXrootdGStream* gs)
{
    this->stream = gs;
}

void TCPStats::Monitor(XrdNetAddrInfo &netInfo, XrdTcpMonPin::LinkInfo &lnkInfo, int liLen)
{


    tcp_info tcp_info;
    socklen_t tcp_info_length = sizeof(tcp_info);
    int sockopt_rc = getsockopt( lnkInfo.fd, SOL_TCP, TCP_INFO, (void *)&tcp_info, &tcp_info_length );
    if ( sockopt_rc  == 0 ) {
        std::string returnedJSON = TCPStats::GenerateJSON(tcp_info, netInfo, lnkInfo.bytesIn, lnkInfo.bytesOut);

        // Insert the JSON into the gstream
        // Include the null terminated character in the count
        this->stream->Insert(returnedJSON.c_str(), returnedJSON.length()+1);
        
    }

}

std::string TCPStats::GenerateJSON(tcp_info& tcp_info, XrdNetAddrInfo& netInfo, long long bytesIn, long long bytesOut)
{
    picojson::object wrapper;
    wrapper["type"] = picojson::value("tcpstats");

    // Remote Address String
    char addr_str[512];
    netInfo.Format(addr_str, 512);
    wrapper["remote"] = picojson::value(addr_str);

    // TCP Statistics
    wrapper["rtt"] = picojson::value(static_cast<double>(tcp_info.tcpi_rtt));
    wrapper["unacked"] = picojson::value(static_cast<double>(tcp_info.tcpi_unacked));
    wrapper["sacked"] = picojson::value(static_cast<double>(tcp_info.tcpi_sacked));
    wrapper["lost"] = picojson::value(static_cast<double>(tcp_info.tcpi_lost));
    wrapper["retrans"] = picojson::value(static_cast<double>(tcp_info.tcpi_retrans));
    wrapper["reordering"] = picojson::value(static_cast<double>(tcp_info.tcpi_reordering));
    wrapper["bytes_in"] = picojson::value(static_cast<double>(bytesIn));
    wrapper["bytes_out"] = picojson::value(static_cast<double>(bytesOut));

    return picojson::value(wrapper).serialize();
}



