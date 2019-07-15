//test1.cc


/*
COMANDOS

-Para compilar
g++ -std=c++11 test.cc
-Para actualizar la libreria
sudo apt-get install libboost-dev

*/

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

using Point = vector<double>;

using DataFrame = vector<Point>;

inline double square(double value) {
  return value * value;
}

void print(const Point& p) {
	cout <<"(";
	for( size_t i = 0; i < p.size(); i++) {
		cout << p[i];
		if(i<p.size()-1)
			cout <<",";
	}
	cout <<")";
	cout << endl;
}

inline double sdistance(const Point& first, const Point& second) {
  double d = 0.0;
  for(size_t dim = 0; dim < first.size(); dim++) 
    d += square(first[dim] - second[dim]);
  return d;
}

double epsilon(const DataFrame& dtf){
	double sum=0;
	for(int i=0;i<dtf.size()-1;i++){
		//cout<<"dif!!"<<endl;
		//print(dtf[i]);
		//print(dtf[i+1]);
		sum+=sdistance(dtf[i],dtf[i+1]);
		//cout<<"sum: "<<sum<<endl;
	}
	return sum;
}

inline void printDF(DataFrame& d){
  	for (Point pt:d)
  		print(pt);
}

double compareDist(DataFrame& d1,DataFrame& d2){
	double sum=0.0;
	//cout<<"d1\n";
	//for(Point pt:d1)
	//	print(pt);
	//cout<<"d2\n";
	//for(Point pt:d2)
	//	print(pt);
	for(int i=0;i<d1.size();i++)
		sum+=sdistance(d1[i],d2[i]);
	return sum;
}

pair<DataFrame,vector<size_t>> kmeans(const DataFrame& data, size_t k, size_t iterations,double eps) {

	size_t dimensions = data[0].size();
	static random_device seed;
	static mt19937 random_gen(seed());
	uniform_int_distribution<size_t> indices(0, data.size()-1);
	//double epsi_ant=MAX_NUM;
	double epsi=MAX_NUM;

	DataFrame means(k);

	size_t i=0;
	for(Point& cluster : means) {
		i = indices(random_gen);

		cluster = data[i];
		//cout << "Selected point at position " << i << endl;

		i++;
		//print(cluster);
	}
	//cout << " aqui" << endl; 
	
    Point p;
   
	vector<size_t> assignments(data.size());
	DataFrame newmeans1(k,vector<double>(dimensions,0.0));
	int it=0;


	while(it<iterations&&epsi>eps){
		//cout << "Iteracion " << it << endl;
 	
		//epsi=epsilon(means);
		//cout<<"sumdist: "<<abs(epsi_ant-epsi)<<endl;

		// Find assignments
		for (size_t point = 0; point < data.size(); point++) {
			double best_distance = numeric_limits<double>::max();
			size_t best_cluster = 0;
			for(size_t cluster = 0; cluster < k; cluster++) {
				double distance = sdistance(data[point], means[cluster]);
				if(distance < best_distance) {
					best_distance = distance;
					best_cluster = cluster;
				}	
			}
			assignments[point] = best_cluster;
		}

		DataFrame newmeans(k,vector<double>(dimensions,0.0));
		vector<size_t> counts(k, 0);

		newmeans1=means;

		for(size_t point = 0; point < data.size(); point++) {
			size_t cluster = assignments[point];
			for(size_t d = 0; d < dimensions; d++) {
				newmeans[cluster][d] += data[point][d];
			}
			counts[cluster] += 1;
		}

		for(size_t cluster = 0; cluster < k; cluster++) {
			size_t count = max<size_t>(1, counts[cluster]);
			for(size_t d = 0; d < dimensions; d++) {
				means[cluster][d] = newmeans[cluster][d] / count;
			}
		}

		it++;
		//cout<<"Means!!\n";
		//for(Point pt:means)
		//	print(pt);

		epsi=compareDist(means,newmeans1);

		//cout<<"sumdist: "<<epsi<<endl;

		//for(Point pt:means)
  			//print(pt);

	}
  	return {means, assignments};
}


DataFrame read(string s,int t,string separator){
	ifstream infile(s);
	string line;
	DataFrame d;
	while (getline(infile, line)) {
		Point p;
		string l=line;
		vector<string> result; 
		boost::split(result, l, boost::is_any_of(separator));
		//double r=stod(result[1]);
		//cout<<"vec: "<<r<<endl;
		//for(string s: result) p.push_back(stod(s));
		//cout<<"tam: "<<result.size()<<endl;
		for(int i=0;i<t;i++){
			p.push_back(stod(result[i]));
		}
		//print(p);
		d.push_back(p);
	}
	cout<<"lines: "<<d.size()<<endl;
	return d;
}

 
int main() {
	//MOCK_DATA.csv
	//wood-fibres.csv
	//test1.txt
	//data.txt
	//madelon_test.data
	int dimens=500;
	DataFrame d=read("dataset/madelon_test.data",dimens," ");
	DataFrame c;
  	vector<size_t> a;
	for(int j=0;j<5;j++){
  		int k=(j+1)*20;
  		cout<<"-k: "<<k<<endl;
	  	for(int i=0;i<10;i++){
	  		ScopedTimer t;

	  		tie(c,a)=kmeans(d,k,1000,0.000005);
	  		//cout<<i+1<<  "-tiempo: " << t.elapsed() << endl;
	  		cout<<t.elapsed()<<endl;
	  		//cout<< "a size: " << a.size() << endl;
	  		//cout<< "c size: " << c.size() << endl;

	  		//cout<<"test 1!!\n";
	  	}
	}
  	

  	

  	//cout<<"-MEANS RESULTS:\n";

  	//printDF(c);

  	//for(Point pt:c)
  	//	print(pt);
/*
  	cout<<"here!"<<endl;
  	for(int i=0;i<a.size();i++){
  		print(d[i]);
  		cout<<"st: ";
  		print(c[a[i]]);
  	}
*/  	
  	return 0;
}