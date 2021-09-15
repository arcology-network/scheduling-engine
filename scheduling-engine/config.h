#pragma once
enum {
	NON_CONFLICT = 0,
	CONFLICT = 255,
	ID_LEN = 48,
	BUFFER_SIZE = 4096,
	INVALID = UINT32_MAX,
};

struct Config {
	uint8_t minParaBatch;
	float presentBetweenThresh = 0.7;
	float historyBetweenThresh = 0.25;
	float historselfThresh = 0.25;
	std::string historyFile;
	bool debug;

	Config(std::string contents) {
		boost::property_tree::ptree root;
		std::istringstream instream(contents);
		try {boost::property_tree::read_json(instream, root);}
		catch (...) {}

		presentBetweenThresh = root.get<float>("presentBetweenThresh", 0.7);
		historyBetweenThresh = root.get<float>("historyBetweenThresh", 0.25);
		historselfThresh = root.get<float>("historselfThresh", 0.25);

		minParaBatch = root.get<uint8_t>("minParaBatch", 2);
		historyFile = root.get<std::string>("historyFile", "");
		debug = root.get<bool>("debug", false);
	}

	std::string ToJasonString() {
		boost::property_tree::ptree root;	

		root.put<float>("presentBetweenThresh", presentBetweenThresh);
		root.put<uint8_t>("minParaBatch", minParaBatch);
		root.put<std::string>("historyFile", historyFile);

		std::ostringstream outStream;
		boost::property_tree::write_json(outStream, root);
		return outStream.str();
	}
};
