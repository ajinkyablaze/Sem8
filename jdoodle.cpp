#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <fstream>

using namespace std;

class document{
    
    public:
    vector<string> terms;
    int degree = 0;
    int doc_id;
    string name;
    
    document(string file_name){
        string term;
        name = file_name.substr(0,file_name.length()-4); //removes the .txt
        //cout<<name<<endl;
        ifstream infile("conflation_output/" + name + ".txt");
        while(infile >> term){
            cout<<term<<endl;
            terms.push_back(term);
        }
        do_sort();
    }

    
    float size(){
       return sqrt(terms.size()); 
    }
    
    void print(){
        for (auto i = terms.begin(); i != terms.end(); ++i){
            cout << *i << ' ';
        }
        cout<<"\n";
    }
    
    void do_sort(){
        sort(terms.begin(), terms.end());
    }
    
    
};



class cluster{
    
    vector<document*> docs;
    document *cr = NULL;
    

    void new_cr(document *d){
        if(cr == NULL){
            cr = d;
        }      
        float cr_weight = 0,new_cr_weight = 0;        
        for(document *it:docs){
            cr_weight += dice_coeff(cr,it);
            new_cr_weight += dice_coeff(d,it);                   
        }
        if(new_cr_weight > cr_weight){
            cout<<"Cluster was cr but now is d\n";
            cr = d;
        }
    }
    
    public:
    
    int cluster_id;

    void add_doc(document *d){
        docs.push_back(d);
        new_cr(d);
    }
    
    float dice_coeff(document *r,document *d){
        int intersection = intersect(r->terms,d->terms);
        float dice = ((float)intersection)/(r->size() * d->size());
        //cout<<intersection<<" "<<r->size()<<" "<<d->size()<<" dice \n";
        return dice;
    }
        
    float dice_coeff(document *d){
        //intersection between cr and d
        int intersection = intersect(cr->terms,d->terms);
        //cout<<intersection<<"\n";
        //cr->print();
        //d->print();
        float dice = ((float)intersection)/(cr->size() * d->size());
        //cout<<intersection<<" "<<cr->size()<<" "<<d->size()<<" dice \n";
        return dice;
    }
    
    int intersect(vector<string> &v1, vector<string> &v2){

        //vector<string> v11 = new vector(v1);
        //vector<string> v22 = new vector(v2);
        vector<string> v3;
        
        //sort(v1.begin(), v1.end());
        //sort(v2.begin(), v2.end());
        
        set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),back_inserter(v3));
        
        return v3.size();
        
    }

    cluster(int id){
        cluster_id = id;
    }

    void print_docs(){
        for (document *it:docs){
            cout<<it->name<<" ";       
        }
    }
    
    
};


vector<cluster> cl;

void determine_cluster(document *d){
    int new_cluster=1; //flag
    float dc;
    cluster *best_cluster;
    float max = 0; //for best cluster
    float threshold = 0.6;
    //cout<<"yolo";
    if(cl.size() == 0){
        cluster c(cl.size());
        c.add_doc(d);
        cl.push_back(c);
    }else{
        for (vector<cluster>::iterator it = cl.begin() ; it != cl.end(); ++it){
            dc = it->dice_coeff(d);
            //cout<<dc<<"\n";
            if(dc > max){
                max = dc;
                best_cluster = &*it;
                //cout<<max<<"MAX"<<'\n';
                //it->add_doc(d);
                //new_cluster = 0;
                //break;
            }
        }
        
        if(max > threshold){
            best_cluster->add_doc(d);
            new_cluster = 0;
        }
        
        if(new_cluster){
            cluster c(cl.size());
            c.add_doc(d);
            cl.push_back(c);
            //cout<<"new cluster made"<<"\n";
        }
        
        
    }
    
    
    
}

void print_cluster_output(){

    for (vector<cluster>::iterator it = cl.begin() ; it != cl.end(); ++it){
        cout<<"Cluster ID-"<<it->cluster_id<<endl;
        it->print_docs();
        cout<<endl;
    }

}

int main() {
    string file_name;
    string index = "conflation_output/index.txt";
    ifstream infile(index);
    while(infile >> file_name){
        cout<<file_name<<endl;
        determine_cluster(new document(file_name)); //make document and add to cluster
    }
    /*
    for all file in folder
        determine_cluster(new document(filename))
    */
    print_cluster_output();
    cout<<"No. of clusters formed is/are "<<cl.size()<<"\n";
    
	return 0;
}
