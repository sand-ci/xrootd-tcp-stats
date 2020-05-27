#include <XrdNet/XrdNetAddrInfo.hh>
#include <Xrd/XrdTcpMonPin.hh>
#include <XrdSys/XrdSysLogger.hh>
#include <XrdOuc/XrdOucPinObject.hh>
#include <XrdXrootd/XrdXrootdGStream.hh>

#include <string>
#include <netinet/tcp.h>



class TCPStats: public XrdTcpMonPin {

    public:
        void Monitor(int fd, XrdNetAddrInfo &netInfo, const char *tident);
        static std::string GenerateJSON(tcp_info& tcp_info, XrdNetAddrInfo& netInfo);

        TCPStats(XrdXrootdGStream* gs);
        virtual ~TCPStats() {};
    
    private:
        XrdXrootdGStream* stream;
        
};


class TcpMonPin : public XrdOucPinObject<XrdTcpMonPin>
{
    public:
        XrdTcpMonPin *getInstance(const char   *parms,
                            XrdOucEnv    &envR,
                            XrdSysLogger &logR,
                            XrdTcpMonPin *prevP);
};