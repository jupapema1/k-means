//test2.cc
//g++ -std=c++11 test2-parallel.cc
//-Para actualizar la libreria
//sudo apt-get install libboost-dev


#include <limits>
#include <cstddef>
#include "timer.hh"
#include <tuple>
#include <iostream>
#include <string>
#include <fstream>
#include <bits/stdc++.h> 
#include <boost/algorithm/string.hpp> 

using namespace std;

#define MAX_NUM numeric_limits<double>::max();

using DataFrame=vector<double>;

void printPoint(DataFrame d,size_t pos, int dimens){
	cout<<"(";
	int posind=pos*dimens;
	for(int i=0;i<dimens;i++){
		cout<<d[posind];
		if(i<dimens-1)
			cout<<",";
		posind++;
	}
	cout<<")"<<endl;
}	

DataFrame read(string s,int t,string separator){
	ifstream infile(s);
	string line;
	DataFrame d;
	while (getline(infile, line)) {
		//Point p;
		string l=line;
		vector<string> result; 
		boost::split(result, l, boost::is_any_of(separator));
		//double r=stod(result[0]);
		//cout<<"vec: "<<r<<endl;
		//for(string s: result) d.push_back(stod(s));
		//cout<<"tam: "<<result.size()<<endl;
		//#pragma omp parallel for
		for(int i=0;i<t;i++)
			d.push_back(stod(result[i]));
		//print(p);
		//d.push_back(p);
	}
	cout<<"lines: "<<d.size()/t<<endl;
	return d;
}

double getPos(DataFrame& d,int i,int j,int dimens){
		return  d[i*dimens]+j;
	}

inline double square(double value){
	return value*value;
}

inline double newDistance(const DataFrame& first,int firstpoint,const DataFrame& second,int secondpoint,int dims){
	double d=0;
	//#pragma omp parallel for
	for(size_t dim=0;dim<dims;dim++)
		d+=square(first[firstpoint*dims+dim]-second[secondpoint*dims+dim]);	
	return d;
}

//void movePoint(DataFrame& d1,DataFrame& d2, int pos1, int pos2,int dimens){}


inline void printDF(const DataFrame& d,size_t dim){
	cout<<"Printing DataFrame...\n";
	for(int i=0;i<d.size()/dim;i++)
		printPoint(d,i,dim);
}

inline double compareDist(DataFrame& d1,DataFrame& d2,size_t dims){
	double sum=0.0;
	//#pragma omp parallel for
	for (int i=0;i<d1.size()/dims;i++)
		sum+=newDistance(d1,i,d2,i,dims);
	return sum;
}

pair<DataFrame,vector<size_t>> kmeans(DataFrame& data, size_t k, size_t iterations,double eps, int dimensions) {
	static random_device seed;
	static mt19937 random_gen(seed());
	uniform_int_distribution<size_t> indices(0,(data.size()/dimensions)-1);

	DataFrame means(k*dimensions);
	//for(Point& cluster:)
	size_t i=0;
	double sumdist=MAX_NUM;

	//#pragma omp parallel for
	for(size_t j=0;j<k;j++){
		i = indices(random_gen);
    	for(int j1=0;j1<dimensions;j1++)
    		means[j*dimensions+j1]=data[i*dimensions+j1];

    	//cout << "Punto seleccionado en la pos: " << i << endl;
		//printPoint(means,j,dimensions);
		i++;
	}

	//cout << "Inicio" << endl; 

	vector<size_t> assignments(data.size()/dimensions);
	DataFrame newMeans1(k*dimensions,0.0);
	size_t it=0;

	while(it<iterations&&sumdist>eps){
		//cout << "Iteracion " << it << endl;
		#pragma omp parallel for
		for(size_t p=0;p<data.size()/dimensions;p++){
			double best_distance=numeric_limits<double>::max();
			size_t best_cluster=0;
			for(size_t cluster=0;cluster<k;cluster++){
				//////////////////////////////////
				double distance=newDistance(data,p,means,cluster,dimensions);
				if(distance<best_distance){
					best_distance=distance;
					best_cluster=cluster;
				}
			}
			assignments[p]=best_cluster;
		}
		DataFrame newMeans(k*dimensions,0.0);
		vector<size_t> counts(k,0);

		newMeans1=means;
		for(size_t point=0;point<data.size()/dimensions;++point){
			const size_t cluster=assignments[point];
			for(size_t d=0;d<dimensions;d++)
				newMeans[cluster*dimensions+d]+=data[point*dimensions+d];
			counts[cluster] += 1;
		}
		//#pragma omp parallel for
		for(size_t cluster=0;cluster<k;cluster++){
			size_t count=max<size_t>(1,counts[cluster]);
			for(size_t d=0;d<dimensions;d++)
				means[cluster*dimensions+d]=newMeans[cluster*dimensions+d]/count;
		}
		//cout<<"Means!!\n";
		//printDF(means,dimensions);
		//cout<<endl;
		sumdist=compareDist(means,newMeans1,dimensions);
		//cout<<"sumdist: "<<sumdist<<" "<<endl;
		it++;
	}
	return {means,assignments};
}

int main(){
	int dimens=500;
	//MOCK_DATA.csv
	//test1.txt
	//data.txt
	//madelon_test.data	
	string s="dataset/madelon_test.data";
	DataFrame d=read(s,dimens," ");
	DataFrame c;  
  	vector<size_t> a;

  	for (int j=0;j<5;j++){
  		int k=(j+1)*20;
  		cout<<"-k:"<<k<<endl;
  		for (int i=0;i<10;i++){
		  	ScopedTimer t;
		  	tie(c,a)=kmeans(d,k,1000,0.000005,dimens);
		  	cout<<t.elapsed()<<endl;
		  	//cout <<  "tiempo: " << t.elapsed() << endl;

		  	//cout<<"test 2 - parallel!!\n";
		  	//cout<<"-MEANS RESULTS\n";
	  	}
	}


  	//printDF(c,dimens);

	return 0;
}
