//Author - Dheerajabc@gmail.com

#ifndef VARIADIC_UTILITIES_H_INCLUDED
#define VARIADIC_UTILITIES_H_INCLUDED
#include<type_traits>
#include<tuple>
#include<utility>
#include<typeinfo>

namespace vadic
{

//general value pack holder, also communicating interface
template<typename T, T...pack> struct valuePack {};
//general type pack holder, also communicating interface
template<typename ...T> struct typePack {};


template<unsigned int val, int ... npack> struct genNumSeqPack : genNumSeqPack<val-1, val-1, npack...> { };
template<int ...npack> struct genNumSeqPack<0, npack...>
{
    typedef valuePack<unsigned int , npack...> seqpack;
};


//general value pack indexer
template<typename T, unsigned int index, T head, T...rempack> struct valuePackIndexer
{
    static_assert(index<=sizeof...(rempack)+1 , "Watch it dude!!, Index error!! ");
    static const T value = valuePackIndexer<T, index-1, rempack...>::value;
};
template<typename T, T head, T...rempack> struct  valuePackIndexer<T, 0, head, rempack...>
{
    static const T value = head;
};


//find value type's index in value pack
template<typename T,  T val, T head, T... rempack> struct isValuePresent
{
    static const bool value = val == head || val ==isValuePresent<T, val, rempack...>::value;
    static const int index ;
    typedef isValuePresent<T, val, rempack...> nestedType;
    static constexpr int Index();
    static constexpr int index_helper(int );
    static constexpr int index_helper_helper(size_t  ) ;
};
template<typename T,  T val, T head, T... rempack>  constexpr int isValuePresent<T, val, head,rempack...>::Index()
{
    return index_helper (0);
}
template<typename T,  T val, T head, T... rempack> constexpr int isValuePresent<T, val, head,rempack...>::index_helper(int n)
{
    return index_helper_helper(n);
}
template<typename T,  T val, T head, T... rempack>  constexpr int isValuePresent<T, val, head,rempack...>::index_helper_helper(size_t var )
{
    return (val == head) ? var : nestedType::index_helper(var+1) ;
}
template<typename T,  T val, T head, T... rempack> const int isValuePresent<T,val, head, rempack...>::index = isValuePresent<T,val, head, rempack...>::Index();

template<typename T, T val , T head> struct isValuePresent<T, val, head>
{
    static const bool value = val==head;
    static constexpr int index_helper(int n)
    {
        return val == head ? n : -1;
    }
};


//compare two value packs
template<typename T, typename Y> struct isEqualPacks;
template<typename T, T head1, T... rempack1, T head2, T...rempack2> struct isEqualPacks<valuePack<T, head1, rempack1...>, valuePack<T, head2, rempack2...>>
{
    static const bool value = head1 == head2 && isEqualPacks<valuePack<T, rempack1...>, valuePack<T,  rempack2...>> ::value;
};
template<typename T, T val1, T val2> struct isEqualPacks<valuePack<T, val1>, valuePack<T, val2>>
{
    static const bool value = val1 = val2;
};


////Usage generateSeqRange<true, 2,5, valuePack<int>>::seqRangePack == valuePack<int, 2,3,4,5>
//template<bool,size_t begin, size_t end,  typename T> struct generateSeqRange;
//template<size_t begin, size_t end,  typename T, T...valpack> struct generateSeqRange<true, begin, end, valuePack<T, valpack...>> : generateSeqRange<begin!=end, begin, end-1, valuePack<T, end, valpack...>>
//{ };
//template<size_t begin, size_t end,  typename T, T...valpack> struct generateSeqRange<false, begin, end, valuePack<T, valpack...>>
//{
//    typedef valuePack<T, valpack...> seqRangePack;
//};

//Usage generateSeqRange<true, 2,5, valuePack<int>>::seqRangePack == valuePack<int, 2,3,4>
template<bool,size_t begin, size_t end,  typename T> struct generateSeqRange;
template<size_t begin, size_t end,  typename T, T...valpack> struct generateSeqRange<true, begin, end, valuePack<T, valpack...>> : generateSeqRange<begin!=end, begin, end-1, valuePack<T, end-1, valpack...>>
{ };
template<size_t begin, size_t end,  typename T, T head, T...valpack> struct generateSeqRange<false, begin, end, valuePack<T, head,valpack...>>
{
    typedef valuePack<T, valpack...> seqRangePack;
};

//makeValueRangePack's helper meta function
template<typename T, typename X> struct valForwardFn;
template<typename T, T... t , typename X, X...x> struct valForwardFn<valuePack<T, t...>, valuePack<X, x...>>
{
    typedef  valuePack<X, valuePackIndexer<X, t, x...>::value...> rangepack;
};

//builds val range pack from existing val range pack, e.g existing pack : 12,23,34,45,56 , usage makeValueRangePack<2,4, valuePack<int, 12,23,34,45,56>>::rangepack ==  valuePack<int, 34,45,56>
template<unsigned int begin,unsigned int end, typename T> struct makeValueRangePack;
template<unsigned int begin, unsigned int end, typename valtype , valtype...valpack> struct makeValueRangePack<begin, end, valuePack<valtype, valpack...>>
{
    static_assert(end<=sizeof...(valpack) , "Watch it dude!!, Range error!!"); //if using commented generateSeqRange, make end<sizeof...(valpack)
    typedef typename valForwardFn< typename generateSeqRange<true, begin, end,valuePack<unsigned int>>::seqRangePack,  valuePack<valtype, valpack...>> ::rangepack valrangepack;
};

template<unsigned int begin,unsigned int end, typename T> struct makeValueRangePack_size;
template<unsigned int begin, unsigned int end, typename valtype , valtype...valpack> struct makeValueRangePack_size<begin, end, valuePack<valtype, valpack...>>
{
    static_assert(end<=sizeof...(valpack) , "Watch it dude!!, Range error!!"); //if using commented generateSeqRange, make end<sizeof...(valpack)
    typedef typename valForwardFn< typename generateSeqRange<true, begin, begin+end,valuePack<unsigned int>>::seqRangePack,  valuePack<valtype, valpack...>> ::rangepack valrangepack;
};

//print val pack
template<typename T, T head> void printvalpack(const valuePack<T,head>&)
{
    std::cout<< "value is : "<<head<<std::endl;
}
template<typename T, T head, T...pack> void printvalpack(const valuePack<T, head,pack...>&)
{
    std::cout<< "value is : "<<head<<std::endl;
    printvalpack( valuePack<T, pack...>());
}


//general type pack indexer
template<unsigned int index, typename head, typename...remtypepack> struct typePackIndexer
{
    typedef typename typePackIndexer<index-1, remtypepack...>::type type;
};
template<typename  head, typename...remtypepack> struct  typePackIndexer<0, head, remtypepack...>
{
    typedef head type ;
};

//seeks type exists in packs
template<typename findtype, typename head, typename...rempack> struct isTypePresent
{
    static const bool value = std::is_same<findtype, head>::value || isTypePresent<findtype, rempack...>::value;
};
template<typename findtype, typename head> struct isTypePresent<findtype, head>
{
    static bool const value = std::is_same<findtype, head>::value;
};

//makeTypeRangePack's helper meta function
template<typename T, typename X> struct typeForwardFn;
template<typename T, T...t, typename...X> struct typeForwardFn<valuePack<T, t...>, typePack<X...>>
{
    typedef  typePack<typename typePackIndexer<t, X...>::type...> rangepack;
};

//Generates ranged type pack from existing pack // Usage : makeTypeRangePack<2,4, typePack<int, float, int, char, double>>::rangepack == typePack<int,char>
template<unsigned int begin,unsigned int end, typename T> struct makeTypeRangePack;
template<unsigned int begin, unsigned int end, typename...typepack> struct makeTypeRangePack<begin, end, typePack<typepack...>>
{
    static_assert(end<=sizeof...(typepack) , "Watch it dude!!, Range error!!"); //if using commented generateSeqRange, make end<sizeof...(typepack)
    typedef typename typeForwardFn<typename generateSeqRange<true, begin, end,valuePack<unsigned int>>::seqRangePack, typePack<typepack...>>::rangepack typerangepack;
};

template<unsigned int begin,unsigned int end, typename T> struct makeTypeRangePack_size;
template<unsigned int begin, unsigned int end, typename...typepack> struct makeTypeRangePack_size<begin, end, typePack<typepack...>>
{
    static_assert(end<=sizeof...(typepack) , "Watch it dude!!, Range error!!"); //if using commented generateSeqRange, make end<sizeof...(typepack)
    typedef typename typeForwardFn<typename generateSeqRange<true, begin, begin+end,valuePack<unsigned int>>::seqRangePack, typePack<typepack...>>::rangepack typerangepack;
};

//print type pack
template<typename head> void printTypePack(const typePack<head>&)
{
    std::cout<< "Type is : "<< typeid(head).name() <<std::endl;
}
template<typename head, typename...pack> void printTypePack(const typePack<head, pack...>&)
{
    std::cout<< "Type is : "<< typeid(head).name() <<std::endl;
    printTypePack(typePack<pack...>());
}

}

#endif // VARIADIC_UTILITIES_H_INCLUDED
