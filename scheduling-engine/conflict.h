#pragma once
struct Conflict {
	Contract<ID_LEN> from;
	Contract<ID_LEN> to;

	Conflict() {}
	Conflict(std::string fromStr, std::string toStr) :from(fromStr), to(toStr) { Arrange(); }
	Conflict(char* lftPtr, char* rgtPtr) :from(lftPtr), to(rgtPtr) { Arrange(); }

	bool operator < (Conflict& other) {
		if (from == other.from)
			return to < other.to;
		return from < other.from;
	}

	bool operator == (Conflict& other) {
		return from == other.from && to == other.to;
	}

	void Arrange() {
		if (!(from < to))
			from.Swap(to);
	}

	std::string ToJasonString() {
		boost::property_tree::ptree root;	
		root.add_child("from", Util::ToJasonNode(from.ToJasonString()));
		root.add_child("to", Util::ToJasonNode(to.ToJasonString()));		

		std::ostringstream outStream;
		boost::property_tree::write_json(outStream, root);
		return outStream.str();
	}

	static std::string ToJasonString(std::vector<Conflict> conflicts) {
		boost::property_tree::ptree root;
		for (std::size_t i = 0; i < conflicts.size(); i++) {
			root.add_child(std::to_string(i), Util::ToJasonNode(conflicts[i].ToJasonString()));
		}

		std::ostringstream outStream;
		boost::property_tree::write_json(outStream, root);
		return outStream.str();
	}

	static std::vector<Contract<ID_LEN>> ToAddresses(std::vector<Conflict>& conflicts) {
		std::vector<Contract<ID_LEN>> newAddresses(conflicts.size() * 2);
		for (std::size_t i = 0; i < conflicts.size(); i++) {
			newAddresses[i * 2] = conflicts[i].from;
			newAddresses[i * 2 + 1] = conflicts[i].to;
		}

		std::sort(newAddresses.begin(), newAddresses.end());
		newAddresses.erase(std::unique(newAddresses.begin(), newAddresses.end()), newAddresses.end());
		return newAddresses;
	}

	void Print() {
		std::cout << "from:";
		from.Print();
		std::cout << std::endl;

		std::cout << "to:";
		to.Print();
		std::cout << std::endl;
	}
};