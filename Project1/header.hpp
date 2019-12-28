#include <any>
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>

class Json {
public:

	explicit Json(const std::string&);
	explicit Json(std::map<std::string, std::any>);
	explicit Json(std::vector<std::any>);
	bool is_array() const;
	bool is_object() const;
	std::any& operator[](const std::string&);
	std::any& operator[](int);
	static Json parse(const std::string&);
	static Json parseFile(const std::string&);

private:

	std::map<std::string, std::any> data_;
	std::vector<std::any> array_;

	std::map<std::string, std::any> parse_object(const std::string&,
		size_t);

	std::vector<std::any> parse_array(const std::string&, size_t);

	static std::string parse_string(const std::string&, size_t);

	static double parse_number(const std::string&, size_t);

	bool IsTrue(const size_t);
};
// 2
class Programm
{

	std::string _firstname;
	std::string _language;
	std::string _profession;
	bool _Vacation;
	int _age;

public:
	Programm(const std::string firstname, const std::string language,
		const int age, const std::string profession,
		const bool vacation) :
		_firstname(firstname), _language(language), _age(age),
		_profession(profession), _Vacation(vacation) {}
};
