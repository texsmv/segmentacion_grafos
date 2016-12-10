 #include "Nodo.h"
#include "Edge.h"
#include <vector>
#include <iostream>
#include <tuple>
#include <map>
#include <queue>
#include <functional>
#include <list>

using namespace std;


template <class D,class K,class P>
class Grafo
{
	typedef    Edge<D,K,P> T_Arista;
	typedef   Nodo<D,K,P> T_Nodo;
	typedef  list<T_Arista*> lista_p_aristas;
	typedef  tuple<bool,P,lista_p_aristas > t_tuple_bool_valor_aristas;
	typedef  map<K, t_tuple_bool_valor_aristas >  t_map_caminos;


	#define  T_Grafo Grafo<D,K,P> 
	#define f first
	#define s second
public:
	Grafo(){};
	map< K,T_Nodo > m_nodos;
	void addNodo(K,D);
	Nodo<D,K,P>* addNodo2(K,D);
	void prints();
	bool addEdge(K,K,P,bool direccionado = false);
	Grafo<D,K,P> dijkstra(K);
	K get_min(t_map_caminos mapa);

	T_Grafo kruskall();
	bool buscar(K llave);
	bool hayCiclo(T_Grafo&,T_Arista&);
	bool perteneceGrafo(T_Nodo*);
};



template <class D,class K,class P>
void Grafo<D,K,P>::addNodo(K llave, D dato){
	T_Nodo nuevo(llave, dato);
	m_nodos[llave]= nuevo;
}

template <class D,class K,class P>
Nodo<D,K,P>* Grafo<D,K,P>::addNodo2(K llave, D dato){
	T_Nodo nuevo(llave, dato);
	m_nodos[llave]= nuevo;
	T_Nodo * p_nuevo=&nuevo;
	return p_nuevo;
}

template <class D,class K,class P>
bool T_Grafo::addEdge(K a,K b,P peso,bool direccionado){

  	auto ita=m_nodos.find(a);
	auto itb=m_nodos.find(b);

	T_Arista * nuevo1 = new T_Arista(&(ita->second),&(itb->second),peso);
	T_Arista * nuevo2 = new T_Arista(&(itb->second),&(ita->second),peso);

	if(ita!=m_nodos.end() && itb!=m_nodos.end()){

		ita->second.m_aristas[itb->first]=nuevo1;
		if(direccionado) itb->second.m_aristas[ita->first]=nuevo2;
		return true;
	}
	else{
		cout << "Algo salio mal" << endl;
		//cout << a << "\n" << b << endl;
		return false;
	}
}

template <class D,class K,class P>
void T_Grafo::prints(){
	auto it= m_nodos.begin();
	//typename std::list<T_Nodo >::iterator it;

	while(it!=m_nodos.end()){
		cout<<(it->first)<<"   ( ";
		auto ite=(it->second).m_aristas.begin();
		while(ite!=(it->second).m_aristas.end()){
			cout<< ite->first << " , ";
			ite++;
		}
		cout << " )"<<endl;
		it++;
	}
}

template <class D,class K,class P>
bool T_Grafo::perteneceGrafo(T_Nodo * nodo){

	auto it = this->m_nodos.find(nodo->m_llave);

	if(it!=m_nodos.end()) return false;
	return true;
}

template <class D,class K,class P>
bool T_Grafo::hayCiclo(T_Grafo & grafo,T_Arista & arista){

	if(perteneceGrafo(arista.m_pinicio) && perteneceGrafo(arista.m_pdestino))return true;
	return false;
}

template <class D,class K,class P>
T_Grafo T_Grafo::kruskall(){

	T_Grafo solucion;
	priority_queue<T_Arista ,vector<T_Arista  >,greater<T_Arista  > > heap;

	for(auto nodo: m_nodos){
		for(auto arista : nodo.second.m_aristas){
			heap.push(*(arista.second));
		}
	}

	while(solucion.m_nodos.size() < this->m_nodos.size()){
		T_Arista  min = heap.top();
		heap.pop();

		if(!hayCiclo(solucion,min)){
			solucion.addNodo(min.m_pinicio->m_llave, min.m_pinicio->m_dato);
			solucion.addNodo(min.m_pdestino->m_llave, min.m_pdestino->m_dato);
			solucion.addEdge(min.m_pinicio->m_llave , min.m_pdestino->m_llave , min.m_peso);
			
		}

		
	}


	return solucion;
}

template <class D, class K,  class P>
Grafo<D,K,P> Grafo<D,K,P>::dijkstra(K llave){


	auto origen = m_nodos.find(llave);
	t_map_caminos caminos;
	Grafo<D,K,P> grafoFinal;


	for (auto it : origen->second.m_aristas){
		lista_p_aristas lista ={it.s};
		caminos[it.f]=t_tuple_bool_valor_aristas(0,it.s->m_peso,lista);
		grafoFinal.addNodo(it.f,it.s->m_pdestino->m_dato);
	}


	grafoFinal.addNodo(origen->f,origen->s.m_dato);
	caminos[origen->first]=t_tuple_bool_valor_aristas(1,0,lista_p_aristas());
	
	
	K minimo;
	int cont=1;	

	while(cont<caminos.size()){
		
		minimo=get_min(caminos);
		auto c_minimo=caminos.find(minimo);
		get<0>(c_minimo->second)=1;
	
		for(auto& it : m_nodos){
			
			auto it_camino_directo= caminos.find(it.first);
			auto it_camino_doble= ( (get<2>(c_minimo->second)).back() )->m_pdestino->m_aristas.find(it.first);
			if(minimo!=it.first){
	

				if(it_camino_directo!=caminos.end()){
					if(it_camino_doble!=( (get<2>(c_minimo->second)).back() )->m_pdestino->m_aristas.end()){
						if( ( (get<2>(c_minimo->s)).back() )->m_peso + it_camino_doble->s->m_peso < get<1>(it_camino_directo->s) ){
							get<2>(it_camino_directo->s).clear();
							for(auto arista : get<2>(c_minimo->s) ){
								get<2>(it_camino_directo->s).push_back(arista);
							}
	
							T_Nodo* nodo_p =&it.second;
	
							T_Arista*nuevo=new T_Arista(  (get<2>(c_minimo->second)).back()->m_pdestino,nodo_p , it_camino_doble->second->m_peso );
							get<2>(it_camino_directo->second).push_back( nuevo );
							get<1>(it_camino_directo->second)= ( (get<2>(c_minimo->second)).back() )->m_peso + it_camino_doble->second->m_peso;

						}
					}
				}
				else{

					if(it_camino_doble!=( (get<2>(c_minimo->second)).back() )->m_pdestino->m_aristas.end()){
	
						T_Nodo * n=&(it.s);
						T_Arista*nuevo=new T_Arista(  (get<2>(c_minimo->s)).back()->m_pdestino , n , it_camino_doble->s->m_peso  );
	
						lista_p_aristas lista;
						caminos[it.f]=t_tuple_bool_valor_aristas(0,(it_camino_doble->s)->m_peso   + get<1>(c_minimo->second) ,lista);		
	

						auto it_c=caminos.find(it.first);
						for(auto arista : get<2>(c_minimo->second) ){
							get<2>(it_c->second).push_back(arista);
						}
						get<2>(it_c->second).push_back(nuevo);

						grafoFinal.addNodo(it.first,it.second.m_dato);
					}


				}
			}
		}
		cont++;
	}


	for(auto it: caminos){
		//cout<<"( dato: "<<it.first<<" bool: "<<get<0>(it.second)<<" peso: "<<get<1>(it.second)<<" Path: ";
		for(auto ite: get<2>(it.second)){
			//cout<<" "<<ite->m_pinicio->m_dato<<"->"<<ite->m_pdestino->m_dato;

			grafoFinal.addEdge(ite->m_pinicio->m_llave,ite->m_pdestino->m_llave,ite->m_peso,0);
		}
		//cout<<") "<<"----";

	}
	return grafoFinal;  
}




template<class D,class K, class P>
K Grafo<D,K,P>::get_min(t_map_caminos mapa){
	bool buscar=1;
	pair<K,P> minimo;
	for (auto it: mapa){
		if(buscar==1 && get<0>(it.second)==0){
			minimo.first= it.first;
			minimo.second= get<1>(it.second);
			buscar=0;

		}
		else if(minimo.second>get<1>(it.second) && get<0>(it.second)==0){
			minimo.first= it.first;
			minimo.second=get<1>(it.second);

		}
	}
	
	return minimo.first;
}



template<class D,class K, class P>
bool Grafo<D,K,P>::buscar(K llave){
	auto it = m_nodos.find(llave);
	if(it==m_nodos.end()) return false;
	return true;
}