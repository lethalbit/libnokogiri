// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcapng/option.hh - Base type for pcapng options */
#if !defined(LIBNOKOGIRI_PCAPNG_OPTIONS_HH)
#define LIBNOKOGIRI_PCAPNG_OPTIONS_HH

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <limits>

namespace libnokogiri::pcapng {
	/*! \enum libnokogiri::pcapng::option_type_t
		\brief Predefined option types

		The following are predefined option types as defined by the pcapng standard, as such
		they are explicitly named.


		For documentation on how to register new option types with libnokogiri see
		the `Adding Options` section in `Extending libnokogiri`.
	*/
	enum struct option_type_t : std::uint16_t {
		End                = 0x0000U, /*!< Indicates the end of options */
		Comment            = 0x0001U, /*!< UTF-8 encoded string, not zero terminated */

		SHBHardware        = 0x0002U, /*!< UTF-8 non zero terminated string - Hardware used to create this section */
		SHBOperatingSystem = 0x0003U, /*!< UTF-8 non zero terminated string - Operating system used to create this section */
		SHBUserApplication = 0x0004U, /*!< UTF-8 non zero terminated string - Application used to create this section */

		Custom1            = 0x0BACU, /*!< Contains a UTF-8 string, can be safely copied, see section 6.2 */
		Custom2            = 0x0BADU, /*!< Contains binary data, can be safely copied, see section 6.2 */
		/* 0x0BAEU - 0x4BABU : Unallocated, presumed reserved */
		Custom3            = 0x4BACU, /*!< Contains a UTF-8 string, can not be copied, see section 6.2 */
		Custom4            = 0x4BADU, /*!< Contains binary data, can not be copied, see section 6.2 */
		/* 0x4BAEU - 0xFFFFU : Unallocated, presumed reserved */
	};

	/*! \struct libnokogiri::pcapng::option_t
		\brief Base for all pcapng options

		This structure represents the atomic base for all options defined by libnokogiri
		as well as by the developer if the add custom options.

		For documentation on how to add new options to libnokogiri see
		the `Adding Options` section in `Extending libnokogiri`

		Options have the following overarching structure:

		```
		 0               1               2               3
		 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|           Option Type         |          Option Length        |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		/                          Option Data                          /
		/                Variable Length padded to 32 bits              /
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		```

		- `Option Type` is a 16-bit unsigned value describing how to interpret the data
		- `Option Length` is a 16-bit unsigned value describing how long the option data is without padding
		- `Option Data` is optional, and depends on `Option Type`

		Options are collected into a contiguous block which is then suffixed with a special option value to
		indicate the end of an option block.

		The termination option is a single option with the type of `0x0000` and the length of `0x0000`. However, the assumption
		that this termination option is present can not be made (for some reason, ask the RFC committee, I don't fucking know).

		For safety, when you construct a default empty base option_t, it will act the same as an options::end_of_options_t .
	*/
	struct option_t {
	private:
		option_type_t _type;
		std::uint16_t _length;
		bool _multiple_allowed;
	protected:
		constexpr option_t(option_type_t type, std::uint16_t length, bool multiple) noexcept :
			_type{type}, _length{length}, _multiple_allowed{multiple}
			{ /* NOP */ }
	public:
		constexpr option_t() noexcept :
			_type{option_type_t::End}, _length{0U}, _multiple_allowed{false}
			{ /* NOP */ }

		/*! Gets the type of the option */
		[[nodiscard]]
		option_type_t type() const noexcept { return _type; }

		/*! Gets the length of the option */
		[[nodiscard]]
		std::uint16_t length() const noexcept { return _length; }

		/*! Gets the total size of this structure as it would be in the pcap file */
		[[nodiscard]]
		std::size_t size() const noexcept { return _length + 4; }

		/*! Gets if there can be more than one of this type of option per block */
		[[nodiscard]]
		bool multiple_allowed() const noexcept { return _multiple_allowed; }
	};

	namespace options {
		/*! \struct libnokogiri::pcapng::options::end_of_options_t
			\brief Sentinel type for the end of a collection of options

			This special struct is used to signify the end of a collection
			of options.

			Therefore it has no members or methods other than those relating to the
			base option_t class.

			Within the list of options this is represented as 4 bytes of consecutive `0x00`'s

			The option is represented in file as follows

			```
			 0               1               2               3
			 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
			+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			|            0x0000U            |            0x0000U            |
			+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			```
		*/
		struct end_of_options_t final : public option_t {
			constexpr end_of_options_t() noexcept : option_t(option_type_t::End, 0x0000U, false)
				{ /* NOP */ }
		};
	}
}

#endif /* LIBNOKOGIRI_PCAPNG_OPTIONS_HH */
