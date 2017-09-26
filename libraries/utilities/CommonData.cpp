#include <graphene/utilities/CommonData.hpp>
#include <boost/algorithm/string.hpp>
#include <graphene/utilities/SHA3.hpp>

using namespace std;
using namespace graphene::utilities;

int graphene::utilities::fromHex(char _i, WhenError _throw)
{
	if (_i >= '0' && _i <= '9')
		return _i - '0';
	if (_i >= 'a' && _i <= 'f')
		return _i - 'a' + 10;
	if (_i >= 'A' && _i <= 'F')
		return _i - 'A' + 10;
	if (_throw == WhenError::Throw)
		return -1;// BOOST_THROW_EXCEPTION(BadHexCharacter() << errinfo_invalidSymbol(_i));
	else
		return -1;
}

bytes graphene::utilities::fromHex(std::string const& _s, WhenError _throw)
{
	unsigned s = (_s.size() >= 2 && _s[0] == '0' && _s[1] == 'x') ? 2 : 0;
	std::vector<uint8_t> ret;
	ret.reserve((_s.size() - s + 1) / 2);

	if (_s.size() % 2)
	{
		int h = fromHex(_s[s++], WhenError::DontThrow);
		if (h != -1)
			ret.push_back(h);
		else if (_throw == WhenError::Throw)
			return bytes();//BOOST_THROW_EXCEPTION(BadHexCharacter());
		else
			return bytes();
	}
	for (unsigned i = s; i < _s.size(); i += 2)
	{
		int h = fromHex(_s[i], WhenError::DontThrow);
		int l = fromHex(_s[i + 1], WhenError::DontThrow);
		if (h != -1 && l != -1)
			ret.push_back((byte)(h * 16 + l));
		else if (_throw == WhenError::Throw)
			return bytes();//BOOST_THROW_EXCEPTION(BadHexCharacter());
		else
			return bytes();
	}
	return ret;
}

bool graphene::utilities::passesAddressChecksum(string const& _str, bool _strict)
{
    if (_str.substr(0, 2) != "0x")
        return false;

    // string s = _str.substr(0, 2) == "0x" ? _str.substr(2) : _str;
    string s = _str.substr(2);

    if (s.length() != 40)
        return false;

        /*
    if (!_strict && (
        _str.find_first_of("abcdef") == string::npos ||
        _str.find_first_of("ABCDEF") == string::npos
    ))
        return true;
        */
    if (!_strict)
    {
        if (s.find_first_of("abcdef") == string::npos && s.find_first_not_of("0123456789ABCDEF") == string::npos)
        {
            return true;
        } else if (s.find_first_of("ABCDEF") == string::npos && s.find_first_not_of("0123456789abcdef") == string::npos)
        {
            return true;
        }
    }

    h256 hash = keccak256(boost::algorithm::to_lower_copy(s, std::locale::classic()));
    for (size_t i = 0; i < 40; ++i)
    {
        char addressCharacter = s[i];
        bool lowerCase;
        if ('a' <= addressCharacter && addressCharacter <= 'f')
            lowerCase = true;
        else if ('A' <= addressCharacter && addressCharacter <= 'F')
            lowerCase = false;
        else
            continue;
        unsigned nibble = (unsigned(hash[i / 2]) >> (4 * (1 - (i % 2)))) & 0xf;
        if ((nibble >= 8) == lowerCase)
            return false;
    }
    return true;
}
