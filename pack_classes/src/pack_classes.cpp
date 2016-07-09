class RosMessage;
class RosService;
class RosServiceResponse;

// TP: Template Parameter
// (RosMessage,RosService,RosServiceResponse) -> (RosMessage)(RosService)(RosServiceResponse)
// (RosMessage)(RosService)(RosServiceResponse) ->template< typename TP_RosMessage, typename TP_RosService, typename TP_RosServiceResponse >
// _class_name (RosMessage)(RosService)(RosServiceResponse) _inst_suffix -> typedef CUseRosClasses<RosMessage, RosService, RosServiceResponse> CUseRosClasses##_inst_suffix;

template< typename RosMessage, typename RosService, typename RosServiceMessage > class CUseRosClasses
{
    CUseRosClasses(
            const RosMessage * _cst_ptr_rosmessage,
            const RosService * _cst_ptr_rosservice,
            const RosServiceMessage * _cst_ptr_rosservicemessage
            )
    { }

    virtual void run();
};

template<typename RosMessage, typename RosService, typename RosServiceResponse>
void CUseRosClasses<RosMessage, RosService, RosServiceResponse>::run() { }

typedef CUseRosClasses<RosMessage, RosService, RosServiceResponse> CUseRosClasses_Inst0;
//
template<>
void CUseRosClasses_Inst0::run() { }

enum WebServerConnectionType {
    BOOST_ASIO_SYNCHRONOUS=0,
    BOOST_ASIO_ASYNCHRONOUS,
    CURL
};

enum DummyEnum {
    TOTO=0
};

struct WebServer_BOOST_ASIO_SYNCHRONOUS
{

};

struct WebServer_BOOST_ASIO_ASYNCHRONOUS
{

};

struct WebServer_CURL
{

};

template< WebServerConnectionType >
struct VLP16_WebServer_Template {
};


int main(int argc, char const *argv[])
{
//    VLP16_WebServer_Template<0> toto;     // error: invalid conversion from 'int' to 'WebServerConnectionType'
//    VLP16_WebServer_Template<TOTO> toto;  // error: could not convert template argument 'TOTO' to 'WebServerConnectionType'
    VLP16_WebServer_Template<BOOST_ASIO_ASYNCHRONOUS> toto; // ok
//    VLP16_WebServer_Template<WebServerConnectionType::BOOST_ASIO_ASYNCHRONOUS> toto;    // ok

    /* code */
    return 0;
}
