# c-_boost
Dépot sur des tests C++ + Boost

## Result (2016-07-08)
-> TEST: Output arguments of variadic macro
float, int, string, vector<int>
-> TEST: BOOST_PP_VARIADIC_TO_SEQ on arguments
(float)(int)(string)(vector<int>)
-> TEST: BOOST_PP_SEQ_REST_N on arguments and index 2
(string)(vector<int>)
-> TEST: BOOST_PP_REPEAT_FROM_TO_1 from 5 to 10 with text=int x
int x5 = 5; int x6 = 6; int x7 = 7; int x8 = 8; int x9 = 9;

