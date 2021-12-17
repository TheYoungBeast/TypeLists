#include <iostream>
#include <type_traits>

//! the end of the list, null type
struct null_t
{};

//! type list
template <typename...>
struct type_list;

//! Partial specialization
template <typename _Head, typename... _Tail>
struct type_list<_Head, _Tail...>
{
    using head =        _Head;
    using tail =        type_list<_Tail...>;
    using type =        type_list<_Head, _Tail...>;

    using value_type =  head;
    using next_type =   tail;
    
    value_type value;
    next_type next;
};

//! explicit(full) specialization
template <>
struct type_list<>
{
    using head = null_t;
    using tail = null_t;
    using type = type_list<>;
};

//! standard alias 
template <typename List>
using head_t = typename List::head;

//! tail type helper
template <typename List>
using tail_t = typename List::tail;

//! determines the end of the list
template <typename T>
struct is_nil : std::false_type {};

//! explicit specialization
template <>
struct is_nil< type_list<> > : std::true_type {};

//! standard alias
template <typename T>
constexpr bool is_nil_v = is_nil<T>::value;

//! list length
template <typename T>
struct length
{
    static constexpr int value = is_nil_v<T> ? 0 : (1 + length< tail_t<T> >::value);
};

//! explicit specialization, the end of the list
template <>
struct length <null_t>
{
    static constexpr int value = 0;
};

//! standard alias
template <typename T>
constexpr int length_v = length<T>::value;

//! standard alias
template <typename T>
constexpr int size_v = length<T>::value;

//! nth child
template <typename T, int N>
struct nth_child
{
    static_assert(!is_nil_v<T>, "Can't perform 'child' operation on empty lists"); // Solution 1 [Empty Lists]
    static_assert(N > 0, "Index below 0");

    using tail = tail_t<T>;
    using type = typename nth_child<tail, N-1>::type;
};

//! part specialization, the end of the list
template <int N>
struct nth_child <null_t, N> // Solution 2 [Empty Lists] | Edge Case
{
    using type = null_t;
};

//! part specialization, nth child accquired
template <typename T>
struct nth_child <T, 0>
{
    using type = head_t<T>;
};

//! nth child helper
template<typename T, int N>
using nth_child_t = typename nth_child<T, N>::type;

//! Does the list contain a certain type?
template <typename, typename>
struct contains;

template <typename List, typename T>
struct contains
{
    static constexpr bool value = std::is_same_v<head_t<List>, T> || contains<tail_t<List>, T>::value;
};

//! Partial specialization, the end of the list
template <typename T>
struct contains<null_t, T>
{
    static constexpr bool value = false;
};

//! standard alias
template <typename List, typename T>
constexpr bool contains_v = contains<List, T>::value;

//! append list
template <typename VariadicList, typename T>
struct append;

//! (non-empty list, type) => return list with appended type
template <typename... Types, typename T>
struct append <type_list<Types...>, T>
{
    using type = type_list<Types..., T>;
};

//! (empty list, type) => return new list<type>
//! (non-empty list, non-empty list) => return merged list
//! (non-empty list, empty-list) => return non-empty list
template <typename... ListItems1, typename... ListItems2>
struct append <type_list<ListItems1...>,  type_list<ListItems2...>>
{
    using type = type_list<ListItems1..., ListItems2...>;
};

//! (empty list, non-empty list) => return non-empty list
template <typename T, typename... ListItems>
struct append <T, type_list<ListItems...>>
{
    using type = type_list<T, ListItems...>;
};

//! standard alias
template <typename VariadicList, typename T>
using append_t = typename append<VariadicList, T>::type;

//! standard alias
template <typename VariadicList, typename T>
using prepend_t = typename append<T, VariadicList>::type;

int main()
{
    using List = type_list<float, float, double, int>;
    using List2 = type_list<std::string, char, double**>;

    using app = append_t<List, List2>;
    using prep = prepend_t<List2, int**>;

    std::cout << size_v<type_list<>> << std::endl;
    std::cout << "Append char : " << typeid(nth_child_t<app, size_v<app>-1>).name() << " " << size_v<app> << std::endl;
    std::cout << "Prepend int : " << typeid(nth_child_t<prep, 0>).name() << " " << size_v<prep> << std::endl;

    std::cout << "Does the list contain int**?: " << contains_v<List, int**> << std::endl;
    std::cout << "Does the list contain int**?: " << contains_v<prep, int**> << std::endl;

    using test = type_list<int, double, char, std::string, int*>;
    test my_list;
    my_list.value = 5;
    my_list.next.value = 20.22;
    my_list.next.next.value = 'c';
    my_list.next.next.next.value = std::string("Uuuuuuuuu aaaaaaaaaaa");
    my_list.next.next.next.next.value = &(my_list.value);

    std::cout << my_list.value << std::endl;
    std::cout << my_list.next.value << std::endl;
    std::cout << my_list.next.next.value << std::endl;
    std::cout << my_list.next.next.next.value << std::endl;
    std::cout << *my_list.next.next.next.next.value << std::endl;

    return 0;
}