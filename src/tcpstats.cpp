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

}

std::string GenerateJSON(tcp_info& tcp_info, XrdNetAddrInfo& netInfo)
{
    picojson::object wrapper;
    wrapper["type"] = picojson::value("tcpstats");

    return picojson::value(wrapper).serialize();
}



