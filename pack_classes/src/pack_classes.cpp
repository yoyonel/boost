#include <type_traits>
#include <string>
#include <typeinfo>
#include <iostream>
using namespace std;


// ----
// url: http://stackoverflow.com/questions/11813940/possible-to-use-type-traits-sfinae-to-find-if-a-class-defines-a-member-type
template<typename ...>
struct void_type
{
    using type = void;
};

template<typename ...T>
using void_t = typename void_type<T...>::type;

#define HAS_TYPE(NAME) \
    template<typename, typename = void> \
    struct has_type_##NAME: std::false_type \
{}; \
    template<typename T> \
    struct has_type_##NAME<T, void_t<typename T::NAME>>: std::true_type \
{} \

//HAS_TYPE(type);

// ----
//template <typename T>
//struct has_type {
//private:
//    template <typename T1>
//    static typename T1::type test(int);
//    template <typename>
//    static void test(...);
//public:
//    enum { value = !std::is_void<decltype(test<T>(0))>::value };
//};
// ----

// with BOOST
// url: http://stackoverflow.com/a/24520544
#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/assert.hpp>

BOOST_MPL_HAS_XXX_TRAIT_DEF(ros_class)
BOOST_MPL_HAS_XXX_TRAIT_DEF(ros_type)
BOOST_MPL_HAS_XXX_TRAIT_DEF(ros_trait_pack_types)

#define ROS_CLASS   typedef void ros_class;

struct RosMessage;
struct RosService;
struct RosServiceResponse;

// TP: Template Parameter
// (RosMessage,RosService,RosServiceResponse) -> (RosMessage)(RosService)(RosServiceResponse)
// (RosMessage)(RosService)(RosServiceResponse) ->template< typename TP_RosMessage, typename TP_RosService, typename TP_RosServiceResponse >
// _class_name (RosMessage)(RosService)(RosServiceResponse) _inst_suffix -> typedef CUseRosClasses<RosMessage, RosService, RosServiceResponse> CUseRosClasses##_inst_suffix;

template< typename RosMessage, typename RosService, typename RosServiceMessage >
class CUseRosClasses
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


struct Trait_RosMessage         { typedef RosMessage            ros_type; };
struct Trait_RosService         { typedef RosService            ros_type; };
struct Trait_RosServiceResponse { typedef RosServiceResponse    ros_type; };

#define ROS_TRAIT_PACK_TYPES typedef void ros_trait_pack_types;

template< typename TRM, typename TRS, typename TRSR >
struct TraitsMSSR {
    ROS_TRAIT_PACK_TYPES
    //    static_assert(has_type<TRM>::value, "Probleme avec le parametre template TRM! Pas de type 'type' accessible!");
    //    static_assert(has_type<TRS>::value, "Probleme avec le parametre template TRS! Pas de type 'type' accessible!");
    //    static_assert(has_type<TRSR>::value, "Probleme avec le parametre template TRSR! Pas de type 'type' accessible!");
    //    BOOST_MPL_ASSERT(( has_type< TRM > ));
    BOOST_MPL_ASSERT_MSG(
            has_ros_type< TRM >::value
            , NON_ROS_TYPE_FOUND
            , (TRM)
            );
    BOOST_MPL_ASSERT_MSG(
            has_ros_type< TRS >::value
            , NON_ROS_TYPE_FOUND
            , (TRS)
            );
    BOOST_MPL_ASSERT_MSG(
            has_ros_type< TRSR >::value
            , NON_ROS_TYPE_FOUND
            , (TRSR)
            );

    typedef typename TRM::ros_type    type_RosMessage;
    typedef typename TRS::ros_type    type_RosService;
    typedef typename TRSR::ros_type   type_RosServiceResponse;
};

typedef TraitsMSSR< Trait_RosMessage, Trait_RosService, Trait_RosServiceResponse > PackTypesMSSR;
//typedef TraitsMSSR< Trait_RosMessage, RosService, Trait_RosServiceResponse > PackTypesMSSR; // error: RosMessage n'est pas officiellement un type/class ROS

//typedef TraitsMSSR< Trait_RosMessage, Trait_RosService, Trait_RosServiceResponse > PackTypesMSSR2;  // error
//PackTypesMSSR2 test;

template< typename PackTypes >
class CUseRosClassesWithPackTypes
{
    // Boost Asserts
    BOOST_MPL_ASSERT_MSG(
            has_ros_trait_pack_types<PackTypes>::value
            , NON_TRAIT_PACK_TYPES
            , (PackTypes)
            );

    // Unpack
    typedef typename PackTypes::type_RosMessage         RosMessage;
    typedef typename PackTypes::type_RosService         RosService ;
    typedef typename PackTypes::type_RosServiceResponse RosServiceResponse;

    CUseRosClassesWithPackTypes(
            const RosMessage * _cst_ptr_rosmessage,
            const RosService * _cst_ptr_rosservice,
            const RosServiceResponse * _cst_ptr_rosserviceresponse
            )
    { }

    virtual void run();
};

//
template<>
void CUseRosClassesWithPackTypes<PackTypesMSSR>::run() { }  // ok

//
//template<>
//void CUseRosClassesWithPackTypes<RosMessage>::run() { } // Boost Assert: 'assertion_failed(mpl_::failed************ (CUseRosClassesWithPackTypes<RosMessage>::NON_TRAIT_PACK_TYPES::************)(RosMessage))'
//CUseRosClassesWithPackTypes<RosMessage> dummyInstantiation; // meme type d'erreur


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


//#include <boost/tti/has_member_function.hpp>
#include <boost/tti/member_type.hpp>
#include <boost/tti/has_member_function.hpp>


int main(int argc, char const *argv[])
{
    //    VLP16_WebServer_Template<0> toto;     // error: invalid conversion from 'int' to 'WebServerConnectionType'
    //    VLP16_WebServer_Template<TOTO> toto;  // error: could not convert template argument 'TOTO' to 'WebServerConnectionType'
    VLP16_WebServer_Template<BOOST_ASIO_ASYNCHRONOUS> toto; // ok
    //    VLP16_WebServer_Template<WebServerConnectionType::BOOST_ASIO_ASYNCHRONOUS> toto;    // ok

    //    static_assert(
    //            has_type<RosService>::value && is_integral<has_type<RosService>::type>::value,
    //            "Error!"
    //            );
    /* code */

    //    std::cout << has_type_type<Trait_RosService>::value << std::endl;
    std::cout << has_ros_class<RosMessage>::value << std::endl;
    std::cout << has_ros_class<RosService>::value << std::endl;

//    BOOST_TTI_HAS_MEMBER_FUNCTION(aMemberFunction);
//    has_member_function_aMemberFunction<void (T::*)(U::Ntype)>::value;

    return 0;
}
