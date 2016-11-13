#include <vector>
#include <fstream>
#include <string>

#ifdef MULTITHREAD
#include <omp.h>
#endif

typedef std::vector<std::vector<bool>> State;

class CField {
	private:
		State m_state;
		void _init_state(State& state, size_t sizex, size_t sizey);
		int _neighbours(size_t x, size_t y) const;
	public:
		CField(const std::string& input);
		void step();
		void write_state(const std::string& output) const;
};
