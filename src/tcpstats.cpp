#include "tcpstats.h"
#include <Xrd/XrdTcpMonPin.hh>
#include <picojson.h>
#include <XrdVersion.hh>
#include <XrdOuc/XrdOucEnv.hh>

XrdVERSIONINFO(TcpMonPin,XRootDTCP);



XrdTcpMonPin* TcpMonPin::getInstance(const char   *parms,
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

void TCPStats::Monitor(int fd, XrdNetAddrInfo &netInfo, const char *tident)
{

    tcp_info tcp_info;
    socklen_t tcp_info_length = sizeof(tcp_info);
    int sockopt_rc = getsockopt( fd, SOL_TCP, TCP_INFO, (void *)&tcp_info, &tcp_info_length );
    if ( sockopt_rc  == 0 ) {
        auto returnedJSON = TCPStats::GenerateJSON(tcp_info, netInfo);
    }
        // Create the json object
        /*
        int sprintf_rc = sprintf(statistics,
            "connect=%s bytes_in=%llu bytes_out=%llu rtt=%u rttvar=%u unacked=%u sacked=%u "
            "lost=%u retrans=%u reordering=%u",
            addr_str,
            LinkBytesIn,
            LinkBytesOut,
            tcp_info.tcpi_rtt,
            tcp_info.tcpi_rttvar,
            tcp_info.tcpi_unacked,
            tcp_info.tcpi_sacked,
            tcp_info.tcpi_lost,
            tcp_info.tcpi_retrans,
            tcp_info.tcpi_reordering
        );
        */

}

std::string TCPStats::GenerateJSON(tcp_info& tcp_info, XrdNetAddrInfo& netInfo)
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

    return picojson::value(wrapper).serialize();
}



