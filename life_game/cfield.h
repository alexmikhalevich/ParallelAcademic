#include <vector>
#include <fstream>
#include <string>

typedef std::vector<std::vector<bool>> State;

class CField {
	private:
		State m_state;
		void _init_state(State& state, size_t xsize, size_t ysize);
	public:
		CField(const std::string input);
		void step();
		void write_state(const std::string output) const;
};
