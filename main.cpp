#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <list>
#include "Grafo.h"

#include <opencv2/highgui/highgui_c.h>
#include <math.h>

using namespace cv;
using namespace std;


#define f first
#define s second
#define vec Vec3b
#define atV at<Vec3b>
#define par pair<int,int>
#define t_grafoPixels Grafo<Vec3b*,par,float>
#define t_nodoP Nodo<Vec3b*,par,float>
#define t_aristaP Edge<Vec3b*,par,float>
#define t_marcas vector<vector<bool> >

#define XYZ tuple<double,double,double> 
#define XYZ tuple<double,double,double> 



float distancia(vec& a,vec& b){
    vec d = a-b;
    float distance = cv::norm(d);
    return distance;
}

float deltae(vec&a, vec& b){
    float xc1 = sqrt(pow(a[1],2) + pow(a[2],2));
    float xc2 = sqrt(pow(b[1],2) + pow(b[2],2));
    float xdl = b[0] - a[0];
    float xdc = xc2 - xc1;
    float xde = sqrt(((a[0] - b[0]) * (a[0] - b[0])) + ((a[1] - b[1]) * (a[1] - b[1])) + ((a[2] - b[2]) * (a[2] - b[2])));
    float xdh, xsc, xsh;
    if (sqrt(xde) > (sqrt(abs(xdl)) + sqrt(abs(xdc)))){
        xdh = sqrt((xde * xde) - (xdl * xdl) - (xdc * xdc));
    }
    else{
        xdh = 0;
    }
    xsc = 1 + (0.045 * xc1);
    xsh = 1 + (0.015 * xc1);
    xdl = xdl / 1;
    xdc = xdc / (1 * xsc);
    xdh = xdh / (1 * xsh);
    float resultado = sqrt(pow(xdl, 2) + pow(xdc, 2) + pow(xdh, 2));
    
    return resultado;
}

bool cumple(vec& a, vec&b, int umbral){
    float xD = deltae(a,b);
    if(xD<umbral) return true;
    return false;
}

vector<par> get_vecindario(int i, int j){
    vector<par> vecindario= { par(i-1,j-1) , par(i-1,j) , par(i-1,j+1) , pair<int,int>(i,j+1) 
        , par(i+1,j+1) , par(i+1,j) , par(i+1,j-1) , par(i,j-1) } ;

    return vecindario;
}

/*
    Funcion  que procesara un pixel y devolvera la llave del nodo representante
    el cual tendra conexion con todos los pixeles de su area procesada

    input: coordenada (x,y) del pixel , matriz de la imagen, el grafo al cual se añadiran los nodos, 
    y una llave para el representante

    output: llave del representante en el grafo
*/
par procesarPixel(int x, int y, Mat& img, t_marcas& matMarcas, t_grafoPixels& graph, int num_repr){
    //cout << "Par .. " <<x << " " << y << endl;
    Vec3b* pixel_representante=new Vec3b(img.atV(x,y)[0],img.atV(x,y)[1],img.atV(x,y)[3]);
    par par_representante(num_repr,num_repr);
    graph.addNodo( par_representante , pixel_representante );
    long long num_pixeles=1;
                            
    graph.addNodo( par(x,y), &(img.atV(x,y)) );
    graph.addEdge(par_representante,par(x,y) , distancia( *(pixel_representante), img.atV(x,y) ) );

    //(matMarcas.at(y)).at(x)=1;
    matMarcas[x][y]=1;
    list<par> colaPixels = {par(x,y)};

    while(colaPixels.size()!=0){
        par p_actual = colaPixels.front();
        vec& pixel_actual = img.atV(p_actual.f,p_actual.s);
        

        colaPixels.pop_front();
        auto vecindario= get_vecindario(p_actual.f , p_actual.s);
        for( auto& vecino : vecindario){            
            //cout<<"img tam " <<img.rows<<"-"<<img.cols<<endl;
            //cout<<vecino.f<<" "<vecino.s<<endl;
            if(vecino.f>0 && vecino.s>0 && vecino.f< (img.rows) && vecino.s<(img.cols)){

                if(!matMarcas[vecino.f][vecino.s]){
                    vec& pixel_vecino= img.atV(vecino.f,vecino.s) ;
                    
                    if(cumple(pixel_actual,pixel_vecino,50) && cumple(*pixel_representante,pixel_vecino,40) ){
                    

                        
                        matMarcas[vecino.f][vecino.s]=1;
                        graph.addNodo(vecino, &(pixel_vecino) );
                    
                        graph.addEdge(par_representante,par(vecino.f,vecino.s),distancia(*pixel_representante,pixel_vecino));
                        colaPixels.push_back(vecino);
                    
                        
                    }
                   
                }
            }
        }
    }

    return par_representante;

}

/*
    funcion para segmentar una imagen dado un conjuno de pixeles
    
*/
 vector<par> procesarPixeles(map<pair<int, int>,pair<int, int > > par_pixeles, Mat& img ,t_grafoPixels& grafo){    
    vector<vector<bool> > matMarcas(img.rows,vector<bool> (img.cols)) ;
    vector<par> llaves_representantes;
    int cont = -1;
    for(auto par_pixel : par_pixeles){
        if(!matMarcas[par_pixel.f.f][par_pixel.f.s]){
            //cout << "procesar " << par_pixel.f.f << " * " <<par_pixel.f.s<< endl;
            llaves_representantes.push_back( procesarPixel(par_pixel.f.f, par_pixel.f.s , img, matMarcas, grafo, cont ) );
        }
        cont--;
    }
    return llaves_representantes;
}
Mat imgp;
map<pair < int, int >, pair< int, int > > datos;

bool presionado = false;

void CallBackFunc(int event, int x, int y, int flags, void* userdata){
    if ( event == EVENT_LBUTTONDOWN ){
            presionado = true;
//            cout << "click izquierdo - position (" << x << ", " << y << ")" << endl;
        }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
              //cout << "click derecho - position (" << x << ", " << y << ")" << endl;
         }
    else if  ( event == EVENT_MBUTTONDOWN ){
        //cout << "boton del centro - position (" << x << ", " << y << ")" << endl;
         }
    else if ( event == EVENT_MOUSEMOVE ){
        if(presionado ==true ){
            cout << "  => (" << x << ", " << y << ")" << endl;
            datos[make_pair(y,x)] = make_pair(y,x); 
            vec& pintar = imgp.atV(y,x) ;
            pintar[0] = (255);
            pintar[1] = (0); 
            pintar[2] = (0);
            imshow("My Window",imgp);
        }
    }
    else if(event == EVENT_LBUTTONUP){
        presionado = false;
       cout<<"soltando izquierdo "<<endl;
    }

}



int main ( int argc, char **argv )
{
    
     // Read image from file 
    //convert_to_lab();
    imgp = imread("frutas.jpg");

    Mat img=imgp.clone();
    Mat img_tmp=img.clone();
    

      //if fail to read the image
     if ( img.empty() ) 
     { 
          cout << "Error loading the image" << endl;
          return -1; 
     }

      //Create a window
     namedWindow("My Window", 1);

      //set the callback function for any mouse event
     setMouseCallback("My Window", CallBackFunc, NULL);

      //show the image
     imshow("My Window", img);

      // Wait until user press some key
     waitKey(0);

    cvtColor(img, img, CV_RGB2Lab);
    //------------------------rules---------------
    
    
    //segmentarColor(img,50,120,img_tmp);
    t_grafoPixels grafo;
    vector<par> pares_representantes= procesarPixeles(datos,img,grafo);
    cvtColor(img, img, CV_Lab2RGB);

    for (auto & par_representante : pares_representantes){

        //cout << par_representante.f << " "<<par_representante.s << endl;

        auto rep = grafo.m_nodos.find(par_representante);
        for (auto& arista : rep->second.m_aristas){
            //cout<<cont<<endl;
            (*(arista.s->m_pdestino->m_dato))[0]=0;
            (*(arista.s->m_pdestino->m_dato))[1]=0;
            (*(arista.s->m_pdestino->m_dato))[2]=0;
            
            
        }    
    }
    imshow("Resultado",img);
    waitKey(0);
    /*
    namedWindow("test", 1);


    imshow("test",img);
    setMouseCallback("test", CallBackFunc, NULL);
    waitKey(0);*/
}
