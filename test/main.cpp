#include "../src/tcpstats.h"

#include <gtest/gtest.h>
#include <XrdOuc/XrdOucEnv.hh>
#include <XrdSys/XrdSysLogger.hh>
#include <XrdNet/XrdNetAddr.hh>
#include <netinet/tcp.h>
#include <netdb.h>
#include <string>
#include <picojson.h>


namespace {


class TCPTest : public ::testing::Test {
    protected:
        void SetUp() override {
            m_env = std::unique_ptr<XrdOucEnv>(new XrdOucEnv());
            m_env->PutPtr("TcpMon.gStream*", nullptr);

            m_logger = std::unique_ptr<XrdSysLogger>(new XrdSysLogger());

            auto new_instance =  std::unique_ptr<TcpMonPin>(new TcpMonPin());
            ASSERT_TRUE(new_instance != nullptr);

            XrdTcpMonPin *prevP = nullptr;

            m_instance = std::unique_ptr<XrdTcpMonPin>(new_instance->getInstance("",
                                    *m_env,
                                    *m_logger,
                                    prevP));
            
            ASSERT_TRUE(m_instance != nullptr);
        }

    std::unique_ptr<XrdOucEnv> m_env;
    std::unique_ptr<XrdSysLogger> m_logger;
    std::unique_ptr<XrdTcpMonPin> m_instance;

};

TEST_F(TCPTest, GenerateJSON) {
    
    // Setup the tcp_info struct
    tcp_info tcp_info;
    tcp_info.tcpi_rtt = 1;
    tcp_info.tcpi_rttvar = 2;
    tcp_info.tcpi_unacked = 3;
    tcp_info.tcpi_sacked = 4;
    tcp_info.tcpi_lost = 5;
    tcp_info.tcpi_retrans = 6;
    tcp_info.tcpi_reordering = 7;
    
    // Setup the XrdNetAddrInfo
    struct hostent *hostinfo;
    struct sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_port = htons (1094);
    hostinfo = gethostbyname ("localhost");
    ASSERT_TRUE(hostinfo != NULL);
    name.sin_addr = *(struct in_addr *) hostinfo->h_addr;
    XrdNetAddr xrdnetaddr(&name);

    std::string returned_json = TCPStats::GenerateJSON(tcp_info, xrdnetaddr);

    // Parse the returned JSON
    picojson::value v;
    std::string err = picojson::parse(v, returned_json);
    ASSERT_TRUE(err.empty());

    ASSERT_TRUE(v.is<picojson::object>());

    ASSERT_EQ(v.get<picojson::object>()["rtt"].get<double>(), 1);
    ASSERT_EQ(v.get<picojson::object>()["unacked"].get<double>(), 3);
    ASSERT_EQ(v.get<picojson::object>()["sacked"].get<double>(), 4);
    ASSERT_EQ(v.get<picojson::object>()["lost"].get<double>(), 5);
    ASSERT_EQ(v.get<picojson::object>()["retrans"].get<double>(), 6);
    ASSERT_EQ(v.get<picojson::object>()["reordering"].get<double>(), 7);

    //std::cout << returned_json;
    
}

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}