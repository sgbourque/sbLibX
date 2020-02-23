#pragma once
#include "common/include/sb_common.h"

//#include "debug.h"
#include <functional>

namespace SB { namespace LibX
{

	template< typename container_t >
struct container_traits
{
	using container_type = container_t;
	using value = typename container_type::value_type;
	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using reference = typename container_type::reference;
	using const_reference = typename container_type::const_reference;
};

	template< typename type_t >
struct container_traits< type_t* >
{
	//using container_type = delete;
	using iterator = type_t*;
	using const_iterator = const type_t*;
	using reference = type_t&;
	using const_reference = const type_t&;
};
// C-array container :
	template< typename type_t >
struct container_traits< type_t*[] > : container_traits< type_t** > {};

// old WIP
//	template< typename type_t, typename allocator_t = void >
//struct array_view
//{
//	using container_type = array_view;
//	using iterator = typename container_traits<type_t>::iterator;
//	using const_iterator = typename container_traits<type_t>::const_iterator;
//	using reference = typename container_traits<type_t>::reference;
//	using const_reference = typename container_traits<type_t>::const_reference;
//
//	array_view() = default;
//	array_view( const array_view& ) = default;
//	array_view( const_iterator begin, const_iterator end ) : _begin( begin ), _end( end ) {}
//	const_iterator begin() const { return _begin; }
//	const_iterator end() const { return _end; }
//private:
//	const_iterator _begin, _end;
//};
//	template< typename type_t, typename allocator_t, template<typename type_t, typename allocator_t> class container_t >
//struct array_view< container_t<type_t, allocator_t>, void >
//{
//	using container_type = container_t<type_t, allocator_t>;
//	using iterator = typename container_traits<container_type>::iterator;
//	using const_iterator = typename container_traits<container_type>::const_iterator;
//	using reference = typename container_type::reference;
//	using const_reference = typename container_type::const_reference;
//
//	explicit array_view( const container_type& container ) : _begin( std::begin( container ) ), _end( std::end( container ) ) {}
//	array_view( const_iterator begin, const_iterator end ) : _begin( begin ), _end( end ) {}
//	const_iterator begin() const { return _begin; }
//	const_iterator end() const { return _end; }
//private:
//	const_iterator _begin, _end;
//};


namespace RangeAccessors
{
using std::get;
// yuck! This is an accessor for basic_string_view such that it behaves as a begin/end range pair
// I can't use begin()/end() as this is the low-level accessor that begin()/end() are actually using!
	template< size_t index_type, typename char_type, typename char_traits_t, typename = std::enable_if_t< ( index_type < 2 ) > >
constexpr const char_type* get( const std::basic_string_view<char_type, char_traits_t>& view ) { return view.data() + index_type * view.size(); }

using std::distance;
	template< typename value_type1, typename value_type2, typename = std::enable_if_t< std::is_arithmetic_v<decltype( value_type1() - value_type2() )> > >
constexpr auto distance( value_type1 begin, value_type2 end ) { return ( end - begin ); }
}

	template< typename range_traits, typename data_type >
struct basic_range_t
{
	using data_t = data_type;

	basic_range_t() = default;
	basic_range_t( data_t&& bounds ) : data_bounds( bounds ) {}

	auto begin() const { return range_traits::begin( *this ); }
	auto& begin() { return range_traits::begin( *this ); }
	auto end() const { return range_traits::end( *this ); }
	auto& end() { return range_traits::end( *this ); }
	auto size() const { return range_traits::size( *this ); }

	const data_t& bounds() const { return data_bounds; }
	data_t& bounds() { return data_bounds; }

	operator bool() const { return size(); }

protected:
	data_type data_bounds;
};
	template< typename range_traits >
struct basic_range_t< range_traits, void >
{
	using data_t = void;
	basic_range_t() = default;
	auto begin() const { return range_traits::begin( *this ); }
	auto& begin() { return range_traits::begin( *this ); }
	auto end() const { return range_traits::end( *this ); }
	auto& end() { return range_traits::end( *this ); }
	auto size() const { return range_traits::size( *this ); }
};

namespace RangeAccessors
{

	template< typename get_type, typename range_traits, typename data_type >
std::enable_if_t< std::is_same_v<data_type, get_type>, get_type >&
get( basic_range_t<range_traits, data_type>& range ) { return range.bounds(); }
	template< typename get_type, typename data_type, typename range_traits >
const std::enable_if_t< std::is_same_v<data_type, get_type>, get_type >&
get( const basic_range_t<range_traits, data_type>& range ) { return range.bounds(); }

	template< typename get_type, typename range_traits, typename data_type >
std::enable_if_t< std::is_convertible_v<basic_range_t<range_traits, data_type>, get_type>, get_type >&
get( basic_range_t<range_traits, data_type>& range ) { return static_cast<get_type&>( range ); }
	template< typename get_type, typename range_traits, typename data_type>
const std::enable_if_t< std::is_convertible_v<basic_range_t<range_traits, data_type>, get_type>, get_type >&
get( const basic_range_t<range_traits, data_type>& range ) { return static_cast<const get_type&>( range ); }

}

template< typename index_type, typename data_type, typename parent_type =
	std::enable_if_t<
	std::is_integral_v<index_type> || std::is_pointer_v<index_type>
	>
>
struct basic_index_range_t : basic_range_t< typename std::conditional_t<std::is_void_v<parent_type>, basic_index_range_t<index_type, data_type, parent_type>, parent_type>, data_type >
{
	using const_iterator = index_type;
	using iterator = index_type;
	using data_t = data_type;
	using range_t = basic_range_t< basic_index_range_t<index_type, data_type>, data_t >;

	basic_index_range_t() = default;
	basic_index_range_t( range_t&& range ) : range_t( std::move( range ) ) {}
	basic_index_range_t( data_t&& bounds ) : range_t( std::move( bounds ) ) {}

	using range_t::begin;
	using range_t::end;
	using range_t::size;

	// yuck...
	static constexpr const_iterator begin( const range_t& range ) { return RangeAccessors::get<0>(RangeAccessors::get<data_t>( range ) ); }
	static constexpr iterator& begin( range_t& range ) { return RangeAccessors::get<0>(RangeAccessors::get<data_t>( range ) ); }
	static constexpr const_iterator end( const range_t& range ) { return RangeAccessors::get<1>(RangeAccessors::get<data_t>( range ) ); }
	static constexpr iterator& end( range_t& range ) { return RangeAccessors::get<1>(RangeAccessors::get<data_t>( range ) ); }

	// as a pointer, the index range is directly a view over a container's data
	template< typename is_pointer = std::void_t< std::is_pointer<basic_index_range_t> > >
	iterator data() const { return begin(); }

	static constexpr size_t size( const range_t& range )
	{
		const basic_index_range_t& index_range = RangeAccessors::get<basic_index_range_t>( range );
		return static_cast<size_t>(RangeAccessors::distance( std::begin( index_range ), std::end( index_range ) ) );
	}
};
template< typename index_type, typename data_type >
constexpr bool operator ==( const basic_index_range_t<index_type, data_type>& range1, const basic_index_range_t<index_type, data_type>& range2 )
{
	return RangeAccessors::get<data_type>( range1 ) == RangeAccessors::get<data_type>( range2 );
}
template< typename ostream_type, typename index_type, typename data_type >
auto& operator <<( ostream_type& out, const basic_index_range_t<index_type, data_type>& range )
{
	return out << sbLibX::get<data_type>( range );
}

template< typename index_type, typename private_type = void >
using index_range_t = basic_index_range_t< index_type, std::tuple<index_type, index_type> >;

template< typename char_type, typename char_traits = std::char_traits<char_type> >
struct string_view : basic_index_range_t< const char_type*, std::basic_string_view<char_type, char_traits> >
{
	using value_type = char_type;
	using data_t = std::basic_string_view<char_type, char_traits>;
	using iterator = typename data_t::iterator;
	using const_iterator = typename data_t::const_iterator;
	using range_t = basic_index_range_t< const char_type*, data_t >;

	string_view() = default;
	string_view( range_t&& range ) : range_t( std::move( range ) ) {}
	string_view( data_t&& bounds ) : range_t( std::move( bounds ) ) {}
	string_view( const char_type* begin, size_t length ) : range_t( std::move( data_t( begin, length ) ) ) {}

	using range_t::begin;
	using range_t::end;
	using range_t::size;
	using range_t::data;

	operator data_t&( ) { return bounds(); }
	operator const data_t&( ) const { return bounds(); }

	value_type operator[]( size_t index ) const { assert( index < size() ); return *( begin() + index ); }
};

template< typename value_type >
constexpr auto get_pointer( value_type& value )
{
	return &value;
}
template< typename value_type >
constexpr auto get_pointer( value_type* const value )
{
	return value;
}

template< typename view_type, typename container_type, typename range_type >
struct range_view_helper;

template< typename view_type, typename container_type, typename index_type, typename data_type >
struct range_view_helper<view_type, container_type, basic_index_range_t<index_type, data_type>>
{
	static_assert(
		std::is_convertible_v<
		typename std::iterator_traits<typename std::remove_pointer_t<container_type>::iterator>::iterator_category,
		std::random_access_iterator_tag>
		, "range_view needs a random access iterator over a container" );
	static constexpr auto apply( const container_type& container, const basic_index_range_t<index_type, data_type>& range )
	{
		auto container_ptr = get_pointer( container );
		assert( range.begin() <= range.end() );
		assert( 0 <= range.begin() && range.end() <= container_ptr->size() );
		static_assert( sizeof( *container_ptr->data() ) == sizeof( view_type::value_type ) );
		return view_type{ reinterpret_cast<const view_type::value_type* const>( container_ptr->data() ) + range.begin(), range.end() - range.begin() };
	}
};
template< typename view_type, typename container_type, typename range_type >
constexpr view_type to_view( const container_type& container, const range_type& range )
{
	return range_view_helper<view_type, container_type, range_type>::apply( container, range );
}
template< typename char_type, typename container_type, typename range_type >
constexpr string_view<char_type> to_string_view( const container_type& container, const range_type& range )
{
	return range_view_helper<string_view<char_type>, container_type, range_type>::apply( container, range );
}


namespace
{
template< typename view_t >
struct hash : std::hash<view_t> {};

template< typename char_type >
struct hash< string_view<char_type> > : std::hash< typename string_view<char_type>::data_t >
{
	using view_t = string_view<char_type>;
	using data_t = typename view_t::data_t;
	using std_hash_t = std::hash<data_t>;
	constexpr size_t operator()( const view_t& view ) const
	{
		return std_hash_t::operator()( view );
	}
};
}

template< typename container_type, typename view_traits_t, typename hash_base_t = hash<typename view_traits_t::view_t> >
struct range_hash_t : hash_base_t
{
	using container_t = container_type;
	using view_t = typename view_traits_t::view_t;
	using hash = hash_base_t;

	range_hash_t( const container_t* container ) : container( container ) {}

	template< typename range_type >
	constexpr auto operator()( const range_type& range ) const
	{
		return hash::operator()( to_view<view_t>( container, range ) );
	}
private:
	const container_t* container;
};
template< typename container_type, typename view_traits_t >
struct range_equal_t
{
	using container_t = container_type;
	using view_t = typename view_traits_t::view_t;

	range_equal_t( const container_t* container ) : container( container ) {}

	template< typename range_type >
	constexpr bool operator ()( const range_type& a, const range_type& b ) const
	{
		return to_view<view_t>( container, a ) == to_view<view_t>( container, b );
	}
private:
	const container_t* container;
};

}} // namespace sbLibX
