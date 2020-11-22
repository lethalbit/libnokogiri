// SPDX-License-Identifier: LGPL-3.0-or-later
/* internal/iterator.hh - A bi-directional templated iterator  */
#pragma once
#if !defined(LIBNOKOGIRI_INTERNAL_ITERATOR_HH)
#define LIBNOKOGIRI_INTERNAL_ITERATOR_HH

#include <libnokogiri/internal/defs.hh>

#include <functional>


namespace libnokogiri::internal {
	template<typename T, typename U, typename I>
	struct bi_iterator final {
	public:
		using acc_func_t = std::function<T&(U&)>;
	private:
		acc_func_t _accessor;
		const I _begin;
		const I _end;
		I _current;
	public:
		constexpr bi_iterator(acc_func_t acc, const I begin, const I end, const bool forward = true) noexcept :
			_accessor{acc}, _begin{begin}, _end{end}, _current{(forward) ? begin : end} { /* NOP */ }

		[[nodiscard]]
		bi_iterator& operator++() noexcept {
			if (_current != _end) {
				_current++;
			}

			return *this;
		}

		[[nodiscard]]
		bi_iterator& operator--() noexcept {
			if (_current != _begin) {
				_current--;
			}

			return *this;
		}

		[[nodiscard]]
		T& operator*() noexcept { return acc_func_t(*_current); }
		[[nodiscard]]
		T& operator->() noexcept { return operator*(); }

		[[nodiscard]]
		bool operator==(bi_iterator& a) const noexcept { return _current == a._current; }
		[[nodiscard]]
		bool operator!=(bi_iterator& a) const noexcept { return !operator==(a); }
	};
}

#endif /* LIBNOKOGIRI_INTERNAL_ITERATOR_HH */
