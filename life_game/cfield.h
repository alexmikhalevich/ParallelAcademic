#include <vector>
#include <fstream>
#include <string>

class CField {
	private:
		std::vector<std::vector<bool>> m_state;
	public:
		CField(const std::string input);
		void step();
		void write_state(const std::string output) const;
};
