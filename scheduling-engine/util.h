#pragma once
namespace Util {
	static bool WriteFile(std::string contents, std::string name) {
		std::ofstream file(name);
		if (file.is_open()) {
			file << contents;
			file.close();
			return true;
		}
		return false;
	}

	template<typename T0, typename T1>
	static std::vector<T0> Rearrange(std::vector<T0>& source, std::vector<T1>& indVec) {
		std::vector<T0> target(source.size());
		for (std::size_t i = 0; i < source.size(); i++)
			target[i] = source(indVec[i]);
		return target;
	}

	template<typename T>
	static void Resize(boost::numeric::ublas::matrix<T>& mat, std::size_t rows, std::size_t cols, T value) {
		if (mat.size1() == rows && mat.size2() == cols)
			return;

		std::size_t r = mat.size1();
		std::size_t c = mat.size2();
		mat.resize(std::max<std::size_t>(mat.size1(), rows), std::max<std::size_t>(mat.size2(), cols));

		for (size_t i = r; i < mat.size1(); i++) {
			std::fill((mat.begin1() + r).begin(), (mat.begin1() + r).end(), value);
		}

		for (size_t j = r; j < mat.size2(); j++) {
			std::fill((mat.begin2() + j).begin(), (mat.begin2() + j).end(), value);
		}
	}

	template<typename T>
	static uint32_t Max(boost::numeric::ublas::matrix<T>& mat, std::size_t rc, T ID) {
		return std::max<uint32_t>(
			std::count((mat.begin1() + rc).begin(), (mat.begin1() + rc).end(), ID),
			std::count((mat.begin2() + rc).begin(), (mat.begin2() + rc).end(), ID));
	}

	template<typename T>
	static void Print(boost::numeric::ublas::matrix<T>& mat) {
		for (size_t i = 0; i < mat.size1(); i++) {
			for (size_t j = 0; j < mat.size2(); j++)
				std::cout << std::to_string(mat(i, j)) + ",";
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	template<typename T>
	static std::string ToString(boost::numeric::ublas::matrix<T>& mat) {
		std::string matrixStr;
		for (size_t i = 0; i < mat.size1(); i++) {
			for (size_t j = 0; j < mat.size2(); j++)
				matrixStr += std::to_string(mat(i, j)) + ",";
			matrixStr += '/n';
		}
		return matrixStr;
	}

	template<typename T>
	static bool ToFile(boost::numeric::ublas::matrix<T>& mat, std::string filename)  {
		std::string msg;
		for (size_t i = 0; i < mat.size1(); i++) {
			for (size_t j = 0; j < mat.size2(); j++)
				msg += std::to_string(mat(i, j)) + ",";
			msg.push_back('\n');
		}
		return Util::WriteFile(msg, filename);
	}

	template<typename T, int N>
	static std::string ToString(std::array<T, N>& array) {
		std::string msg = "[";		
		for (size_t i = 0; i < array.size(); i++) {
			if (uint8_t(array[i]) != 0)
				msg.push_back(char(uint8_t(array[i])));
		}
		msg += "]";
		return msg;
	}
	
	template<typename T>
	static std::string ToJasonString(boost::numeric::ublas::matrix<T>& mat) {
		boost::property_tree::ptree root;
		for (size_t i = 0; i < mat.size1(); i++) {		
			std::string msg;
			for (size_t j = 0; j < mat.size2(); j++)
				msg += std::to_string(mat(i, j)) + ",";
			root.push_back(std::make_pair("", boost::property_tree::ptree(msg)));
		}

		std::ostringstream outStream;
		boost::property_tree::write_json(outStream, boost::property_tree::ptree(root));
		return outStream.str();
	}

	template<typename T>
	static void Concate(std::vector<T>& lft, std::vector<T> rgt) {
		lft.insert(lft.end(), rgt.begin(), rgt.end());
	}

	template<typename T>
	static std::vector<T> Concate(std::vector<std::vector<T>> nested) {
		std::vector<T> concatenated;
		for (std::size_t i = 0; i < nested.size(); i++)
			Concate(concatenated, nested[i]);
		return concatenated;
	}
	
	template<typename T>
	static std::vector<T> Range(std::size_t count, T initV) {
		std::vector<T> rangeVec;
		rangeVec.resize(count);
		std::generate(rangeVec.begin(), rangeVec.end(), [&]() mutable { return initV ++; });
		return rangeVec;
	}

	template<typename T>
	static T Last(std::vector<T>& indVec) {
		if (indVec.empty())
			return 0;
		else
			return indVec.back() + 1;
	}
	
	static boost::property_tree::ptree ToJasonNode(std::string jsonString) {
		boost::property_tree::ptree subnode;
		std::istringstream instream(jsonString);
		try { boost::property_tree::read_json(instream, subnode); }
		catch (...) {}
		return subnode;
	}

	static std::string Now() {
		auto time = std::chrono::system_clock::now();
		std::time_t now = std::chrono::system_clock::to_time_t(time);
		std::string timeStr(std::ctime(&now));
		return timeStr;
	}

	static std::vector<std::string> ReadFile(std::string name) {
		std::ifstream file(name);
		std::string str;
		std::vector<std::string> contents;
		while (std::getline(file, str))
			if (!str.empty()) 
				contents.push_back(str);
			
		file.close();
		return contents;
	}
	
	template<class T>
	std::string Save(T* obj, std::string fileName) {
		std::string msg;
		std::ofstream ofs(fileName);
		if (ofs.is_open()) {
			boost::archive::text_oarchive oa(ofs);
			oa << *obj;
			ofs.close();
		}
		else
			msg = "Failed to open file:" + fileName;
		return msg;
	}

	template<class T>
	std::string Load(T & obj, std::string fileName) {
		std::string msg;
		std::ifstream ifs(fileName);
		if (ifs.is_open()) {
			boost::archive::text_iarchive ia(ifs);
			ia >> obj; // restore the history from the archive
			ifs.close();
		}
		else
			msg += "Failed to open file:" + fileName;
		return msg;
	}
}



