#include <utility>
#include "header.hpp"
Json::Json(const std::string &s) {
	size_t begin_object = s.find_first_of("{");
	size_t begin_array = s.find_first_of("[");
	if (begin_object == std::string::npos) begin_object =
		std::numeric_limits<size_t>::max();
	if (begin_array == std::string::npos) begin_array =
		std::numeric_limits<size_t>::max();

	if (begin_object < begin_array) {
		data_ = parse_object(s, begin_object);
	}
	if (begin_array < begin_object) {
		array_ = parse_array(s, begin_array);
	}
}
Json::Json(std::map<std::string, std::any> object) : data_(std::move(object)) {}
Json::Json(std::vector<std::any> array) : array_(std::move(array)) {}
bool Json::is_array() const {
	return !array_.empty();
}
bool Json::is_object() const {
	return !data_.empty();
}
std::any& Json::operator[](const std::string &key) {
	if (is_object()) {
		return data_[key];
	}
	throw std::logic_error("JSON is array!");
}
std::any& Json::operator[](int index) {
	if (is_array())
		return array_.at(index);
	throw std::logic_error("JSON is object!");
}
Json Json::parse(const std::string &s) {
	return Json(s);
}
Json Json::parseFile(const std::string &path_to_file) {
	std::string text;
	std::ifstream in(path_to_file);
	if (in.is_open())
	{
		while (getline(in, text))
		{
			std::cout << text << std::endl;
		}
	}
	in.close();
	return Json(text);
}
std::map<std::string, std::any> Json::parse_object(const std::string &s,
	size_t current) {
	std::map<std::string, std::any> json;
	std::string key;
	bool flag_end_of_object = false;
	bool flag_key = false;
	bool flag_colon = false;
	bool flag_value = false;
	bool flag_comma = false;
	for (; current < s.length(); ++current) {
		if (s[current] == ' ' || s[current] == '\n' || s[current] == '\t' ||
			s[current] == '\v' || s[current] == '\0') {
			continue;
		}
		//открытие объекта
		if (s[current] == '{' && !flag_end_of_object) {
			flag_end_of_object = true;
			//далее ожидаем ключ =>
			flag_key = true;
			continue;
		}

		if (s[current] == '"' && flag_key) {
			key = parse_string(s, ++current);
			flag_key = false;
			//двоеточие
			flag_colon = true;
			continue;
		}

		if (s[current] == ':' && flag_colon) {
			flag_colon = false;
			flag_value = true;

			for (++current; flag_value; ++current) {
				switch (s[current]) {
					//if string
				case '"': {
					json[key] = parse_string(s, ++current);
					flag_value = false;
					break;
				}
						  //if array
				case '[': {
					json[key] = Json(parse_array(s, current));
					flag_value = false;
					break;
				}
						  //if object
				case '{': {
					json[key] = Json(parse_object(s, current));
					flag_value = false;
					break;
				}
						  //if true
				case 't': {
					const char* k_True = "true";
					for (size_t i = current; i < 3; ++i) {
						if (k_True[i] != s[current]) {
							throw std::invalid_argument("Bad argument");
						}
						++current;
					}
					json[key] = true;
					flag_value = false;
					break;
				}
						  //if false
				case 'f': {
					const char* k_False = "false";
					for (size_t i = current; i < 4; ++i) {
						if (k_False[i] != s[current]) {
							throw std::invalid_argument("Bad argument");
						}
						++current;
					}
					json[key] = false;
					flag_value = false;
					break;
				}
						  //if number
				case '-':
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				{
					double number = parse_number(s, current);
					if (number - static_cast<int>(number) == 0) {
						json[key] = static_cast<int>(number);
					}
					else {
						json[key] = number;
					}
					flag_value = false;
					break;
				}
				default: {
					if (s[current] == ' ' || s[current] == '\n' ||
						s[current] == '\v' || s[current] == '\0') {
						break;
					}
					else {
						throw std::logic_error("wrong value");
					}
				}
				}
			}
			--current;
			flag_comma = true;
			key.clear();
			continue;
		}
		if (s[current] == ',' && flag_comma) {
			flag_key = true;
			continue;
		}
		if (s[current] == '}' && flag_end_of_object) {
			break;
		}
	}
	return json;
}
std::vector<std::any> Json::parse_array(const std::string &s, size_t current) {
	std::vector<std::any> arr;
	if (s[current] == '[')
	{
		bool expected_end_of_array = true;
		for (current += 1; expected_end_of_array; ++current)
		{
			switch (s[current])
			{
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			{
				double number = parse_number(s, current);
				if (number - static_cast<int>(number) == 0) {
					arr.emplace_back(static_cast<int>(number));
				}
				else {
					arr.emplace_back(number);
				}
				break;
			}
			case ']': {
				expected_end_of_array = false;
				break;
			}
			}
		}
	}
	return arr;
}

std::string Json::parse_string(const std::string &s, size_t begin) {
	auto end = s.find_first_of('"', begin);
	std::string value = s.substr(begin, end - begin);
	return value;
}

double Json::parse_number(const std::string &s, size_t begin) {
	size_t comma = s.find_first_of(',', begin);
	size_t end_object = s.find_first_of('}', begin);
	size_t end_arr = s.find_first_of(']', begin);
	if (comma == std::string::npos) comma = std::numeric_limits<size_t>::max();
	if (end_arr == std::string::npos) end_arr =
		std::numeric_limits<size_t>::max();
	if (end_object == std::string::npos) end_object =
		std::numeric_limits<size_t>::max();

	size_t end = 0;
	if (comma < end_arr && comma < end_object) {
		end = comma;
	}
	if (end_arr < comma && end_arr < end_object) {
		end = end_arr;
	}
	if (end_object < comma && end_object < end_arr) {
		end = end_object;
	}

	std::string value = s.substr(begin, end - begin);
	return std::atof(value.c_str());
}
