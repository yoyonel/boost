#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

#define CALL_MACRO(_,__,SEQ) Macro(Foo<BOOST_PP_SEQ_ENUM(SEQ)>)

#define GENERATE_MACRO_INVOCATIONS(SEQS) BOOST_PP_SEQ_FOR_EACH(CALL_MACRO,_,SEQS)

#define GENERATE_DESCENDING_SEQUENCES(_,INDEX,DATA) (BOOST_PP_SEQ_REST_N(INDEX,DATA))

#define BAR_IMPL(SEQ) GENERATE_MACRO_INVOCATIONS(BOOST_PP_REPEAT_FROM_TO(0,BOOST_PP_SEQ_SIZE(SEQ),GENERATE_DESCENDING_SEQUENCES, SEQ))


#define Bar(...) BAR_IMPL(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

// Bar(float, int, string, vector<int>)

#define View_VA_ARGS(...) 						__VA_ARGS__
#define View_BOOST_PP_VARIADIC_TO_SEQ(...) 		BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)
#define View_BOOST_PP_REPEAT_FROM_TO(SEQ) 		BOOST_PP_REPEAT_FROM_TO(0,BOOST_PP_SEQ_SIZE(SEQ),GENERATE_DESCENDING_SEQUENCES, SEQ)
#define View_BOOST_PP_SEQ_REST_N(...)			BOOST_PP_SEQ_REST_N(2, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
//
#define ARGS float, int, string, vector<int>

-> TEST: Output arguments of variadic macro
View_VA_ARGS(ARGS)
//
// url: http://www.boost.org/doc/libs/1_59_0/libs/preprocessor/doc/ref/variadic_to_seq.html
-> TEST: BOOST_PP_VARIADIC_TO_SEQ on arguments
View_BOOST_PP_VARIADIC_TO_SEQ(ARGS)
// => produce: (float)(int)(string)(vector<int>)
//
// url: http://www.boost.org/doc/libs/1_52_0/libs/preprocessor/doc/ref/seq_rest_n.html
-> TEST: BOOST_PP_SEQ_REST_N on arguments and index 2
View_BOOST_PP_SEQ_REST_N(ARGS)
// => produce: (string)(vector<int>)
// => c'est (float)(int)(string)(vector<int>) moins les 2 1ers éléments => (string)(vector<int>)
//
// url: http://www.boost.org/doc/libs/1_46_1/libs/preprocessor/doc/ref/repeat_from_to.html
-> TEST: BOOST_PP_REPEAT_FROM_TO from 5 to 10 with text=int x
#define DECL(z, n, text) BOOST_PP_CAT(text, n) = n;
BOOST_PP_REPEAT_FROM_TO(5, 10, DECL, int x)
// => produce: int x5 = 5; int x6 = 6; int x7 = 7; int x8 = 8; int x9 = 9;
//