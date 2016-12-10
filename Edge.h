template <class D,class K,class P>
class Edge
{
public:
	Edge(Nodo<D,K,P> * i,Nodo<D,K,P> * d, P peso){
		m_peso = peso;
		m_pdestino = d;
		m_pinicio = i;
	}

	P m_peso;

	Nodo<D,K,P> * m_pdestino;
	Nodo<D,K,P> * m_pinicio;

};

template <class D,class K, class P>
inline bool operator >(const Edge<D,K,P> & a1,const Edge<D,K,P> & a2)  
{  
  return a1.m_peso > a2.m_peso;
}