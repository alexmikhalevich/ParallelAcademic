#include "cfield.h"

CField::CField(const std::string& input) {
	m_cur_state = 0;
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

void CField::_init_state(State& state, size_t xsize, size_t ysize) {
	for(size_t i = 0; i < sizex; ++i) {
		state[i].resize(sizey);
		for(size_t j = 0; j < sizey; j++) state[i][j] = false;
	}
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
	_init_state(next_state);
	for(size_t i = 0; i < m_state.size(); ++i) {
		for(size_t j = 0; j < m_state[0].size(); ++j) {

		}
	}
}
