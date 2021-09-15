#pragma once

template<int N>
struct Contract {
	std::array<uint8_t, N> address;
	
	Contract(char* ptr, uint32_t len) : Contract() {
		std::fill(address.begin(), address.end(), 0);
		std::copy(ptr, ptr + std::min<std::size_t>(len, address.size()), address.begin());
	}

	Contract() { std::fill(address.begin(), address.end(), 0); }
	Contract(std::string str) : Contract((char*)str.c_str(), str.size()) {}
	Contract(std::array<uint8_t, N> other) { address = other; }

	template<int N1>
	std::array<uint8_t, N1> To() {
		std::array<uint8_t, N1> other;
		std::fill(other.begin(), other.end(), 0);
		std::copy(address.begin(), address.begin() + std::min<std::size_t>(other.size(), address.size()), other.begin());
		return other;
	}

	bool operator == (const Contract& other) const {
		return std::memcmp(&address[0], &other.address[0], other.address.size()) == 0;
	}

	bool operator < (const Contract& other) const {
		for (std::size_t i = 0; i < address.size(); i++) {
			if (address[i] == other.address[i])
				continue;
			return address[i] < other.address[i];
		}
		return false;
	}

	void Swap(Contract& other) {
		std::swap(*this, other);
	}

	bool IsEmpty() {
		return std::all_of(address.begin(), address.end(), [](uint8_t c) { return c == 0; });
	}

	void Print() {
		std::cout << ToJasonString();
	}

	std::string ToJasonString() {
		boost::property_tree::ptree root;
		auto contractStr = Util::ToString<uint8_t, N>(address);
		root.put("Contract", contractStr);
		
		std::ostringstream outStream;
		boost::property_tree::write_json(outStream, root);
		return outStream.str();
	}

private:
	friend class boost::serialization::access;
	template<class T>
	void serialize(T & ar, const unsigned int version) {
		ar & address;
	}
};
