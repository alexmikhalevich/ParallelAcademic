#include "cfield.h"

CField::CField(const std::string& input) {
	std::ifstream in(input);
	size_t sizex, sizey;
	in >> sizex >> sizey;
	_init_state(m_state, sizex, sizey);
	while(!in.eof()) {
		size_t x, y;
		in >> x >> y;
		m_state[x][y] = true;
	}
}

void CField::_init_state(State& state, size_t sizex, size_t sizey) {
	state.resize(sizex);
#ifdef MULTITHREAD
#pragma omp parallel 
{
#pragma omp for 
#endif
	for(size_t i = 0; i < sizex; ++i) {
		state[i].resize(sizey);
		for(size_t j = 0; j < sizey; j++) state[i][j] = false;
	}
#ifdef MULTITHREAD
}
#endif
}

int CField::_neighbours(size_t x, size_t y) const {
	size_t north_x = (x == 0) ? m_state.size() - 1 : x - 1;
	size_t south_x = (x == m_state.size() - 1) ? 0 : x + 1;
	size_t west_y = (y == 0) ? m_state[0].size() - 1 : y - 1;
	size_t east_y = (y == m_state[0].size() - 1) ? 0 : y + 1;
	return (int)m_state[north_x][y] + (int)m_state[south_x][y] + (int)m_state[x][west_y] + (int)m_state[x][east_y]
		+ (int)m_state[north_x][west_y] + (int)m_state[north_x][east_y] + (int)m_state[south_x][west_y]
		+ (int)m_state[south_x][east_y];
}

void CField::write_state(const std::string& output) const {
	std::ofstream out(output);
	out << m_state.size() << " " << m_state[0].size() << std::endl;
	for(size_t i = 0; i < m_state.size(); ++i) 
		for(size_t j = 0; j < m_state[0].size(); ++j) 
			if(m_state[i][j]) out << i << " " << j << std::endl;
}

void CField::step() {
	State next_state;
	_init_state(next_state, m_state.size(), m_state[0].size());
#ifdef MULTITHREAD
#pragma omp parallel
{
#pragma omp for 
#endif 
	for(size_t i = 0; i < m_state.size(); ++i) {
		for(size_t j = 0; j < m_state[0].size(); ++j) {
			size_t n = _neighbours(i, j);
			if(n == 3 && !m_state[i][j]) next_state[i][j] = true;
			else if((n == 2 || n == 3) && m_state[i][j]) next_state[i][j] = true; 
			else next_state[i][j] = false;
		}
	}
#ifdef MULTITHREAD
}
#endif
	m_state = next_state;
}
