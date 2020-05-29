#include <XrdNet/XrdNetAddrInfo.hh>
#include <Xrd/XrdTcpMonPin.hh>
#include <XrdSys/XrdSysLogger.hh>
#include <XrdOuc/XrdOucPinObject.hh>
#include <XrdXrootd/XrdXrootdGStream.hh>

#include <string>
#include <netinet/tcp.h>



class TCPStats: public XrdTcpMonPin {

    public:
        void Monitor(XrdNetAddrInfo &netInfo, XrdTcpMonPin::LinkInfo &lnkInfo, int liLen);
        static std::string GenerateJSON(tcp_info& tcp_info, XrdNetAddrInfo& netInfo, long long bytesIn, long long bytesOut);

        TCPStats(XrdXrootdGStream* gs);
        virtual ~TCPStats() {};
    
    private:
        XrdXrootdGStream* stream;
        
};


class myPinObject : public XrdOucPinObject<XrdTcpMonPin>
{
    public:
        myPinObject() {};
        virtual ~myPinObject() {};
        XrdTcpMonPin *getInstance(const char   *parms,
                            XrdOucEnv    &envR,
                            XrdSysLogger &logR,
                            XrdTcpMonPin *prevP);
} TcpMonPin;