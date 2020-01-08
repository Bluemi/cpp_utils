#include <tuple>
#include <iterator>
#include <functional>

template<typename ...Ts>
class zip_iterator {
	public:
		using iterator_types = std::tuple<typename Ts::iterator...>;

		zip_iterator(const std::tuple<typename Ts::iterator...>& iterators)
			: _iterators(iterators)
		{}

		template<std::size_t ...I, typename ...TsIterator>
		bool comp_tuple_impl(
			[[maybe_unused]] std::index_sequence<I...>,
			const iterator_types& t1,
			const iterator_types& t2
		) const {
			return ((std::get<I>(t1) == std::get<I>(t2)) || ...);
		}

		bool operator==(const zip_iterator& other) const {
			return comp_tuple_impl(std::make_index_sequence<sizeof...(Ts)>(), this->_iterators, other._iterators);
		}

		bool operator!=(const zip_iterator& other) const {
			return !(*this == other);
		}

		template<std::size_t ...I, typename ...TsIterator>
		void inc_tuple_impl(
			[[maybe_unused]] std::index_sequence<I...>,
			std::tuple<TsIterator...>& t
		) {
			((std::get<I>(t) = std::next(std::get<I>(t))), ...);
		}

		template<typename ...TsIterator>
		void inc_tuple(std::tuple<TsIterator...>& t) {
			inc_tuple_impl(std::make_index_sequence<sizeof...(TsIterator)>(), t);
		}

		void operator++() {
			inc_tuple(_iterators);
		}

		template<typename T>
		typename T::value_type& get_member(T& t) {
			return *t;
		}

		template<std::size_t ...I>
		std::tuple<typename Ts::value_type&...> get_tuple_impl(
			[[maybe_unused]] std::index_sequence<I...>,
			iterator_types& t
		) {
			return std::tuple<typename Ts::value_type&...>(get_member(std::get<I>(t)) ...);
		}

		std::tuple<typename Ts::value_type&...> operator*() {
			return get_tuple_impl(std::make_index_sequence<sizeof...(Ts)>(), _iterators);
		}
	private:
		iterator_types _iterators;
};

template<typename ...Ts>
class zip {
	public:
		zip(Ts&... containers) : _containers(&containers...) {}

		template<typename ContainerType>
		typename ContainerType::iterator get_begin_member(ContainerType* t) {
			return std::begin(*t);
		}

		template<std::size_t ...I>
		std::tuple<typename Ts::iterator...> get_begin_impl(
			[[maybe_unused]] std::index_sequence<I...>,
			const std::tuple<Ts*...>& t
		) {
			return std::make_tuple(get_begin_member(std::get<I>(t)) ...);
		}

		zip_iterator<Ts...> begin() {
			return zip_iterator<Ts...>(get_begin_impl(std::make_index_sequence<sizeof...(Ts)>(), _containers));
		}

		template<typename ContainerType>
		typename ContainerType::iterator get_end_member(ContainerType* t) {
			return std::end(*t);
		}

		template<std::size_t ...I>
		std::tuple<typename Ts::iterator...> get_end_impl(
			[[maybe_unused]] std::index_sequence<I...>,
			const std::tuple<Ts*...>& t
		) {
			return std::make_tuple(get_end_member(std::get<I>(t)) ...);
		}

		zip_iterator<Ts...> end() {
			return zip_iterator<Ts...>(get_end_impl(std::make_index_sequence<sizeof...(Ts)>(), _containers));
		}

	private:
		std::tuple<Ts*...> _containers;
};
