#include "cfield.h"

CField::CField(const std::string& input) {
	std::ifstream in(input);
	size_t sizex, sizey;
	in >> sizex >> sizey;
	m_state.resize(sizex);
	for(size_t i = 0; i < sizex; ++i) {
		m_state[i].resize(sizey);
		for(size_t j = 0; j < sizey; j++) m_state[i][j] = false;
	}
	while(!in.eof()) {
		size_t x, y;
		in >> x >> y;
		m_state[x][y] = true;
	}
}

void write_state(const std::string& output) const {
	std::ofstream out(output);
	out << m_state.size() << " " << m_state[0].size() << std::endl;
	for(size_t i = 0; i < m_state.size(); ++i) 
		for(size_t j = 0; j < m_state[0].size(); ++j) 
			if(m_state[i][j]) out << i << " " << j << std::endl;
}
