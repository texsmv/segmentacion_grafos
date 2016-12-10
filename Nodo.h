#include <map>

using namespace std;

template <class D,class K,class P>
class Edge;

template <class D,class K,class P>
class Nodo
{
public:
	

	Nodo(K llave,D dato){
		m_dato = dato;
		m_llave = llave;
	}

	Nodo(){
		
	}
	
	K m_llave;
	D m_dato;
	map< K, Edge<D,K,P> * >  m_aristas;
};
