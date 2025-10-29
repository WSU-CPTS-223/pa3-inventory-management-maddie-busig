#include "CSV/CSVWriter.hpp"

#include <string>
#include <fstream>

namespace CSV {

class CSVFileWriter : public CSVWriter {
public:
	CSVFileWriter(std::string filename, bool write_header) :
		filename_(filename),
		write_header_(write_header),
		file_(filename) { }

private:

	void writeline(std::string line) override;

	std::string filename_;
	bool write_header_;
	std::ofstream file_;
};

} // namespace CSV

